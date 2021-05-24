// Gaxtapper: Automated GSF ripper for GAX Sound Engine.

#include "gax_driver.hpp"

#include <algorithm>
#include <array>
#include <cstring>
#include <sstream>
#include <string>
#include <string_view>
#include "arm.hpp"
#include "bytes.hpp"
#include "gax_driver_param.hpp"
#include "gax_minigsf_driver_param.hpp"
#include "gax_song_header.hpp"
#include "types.hpp"

namespace gaxtapper {

static constexpr std::string_view kVersionTextPrefixPattern{"GAX Sound Engine "};

GaxDriverParam GaxDriver::Inspect(std::string_view rom) {
  GaxDriverParam param;
  param.set_version_text(FindGaxVersionText(rom));
  param.set_version(ParseVersionText(param.version_text()));
  param.set_gax2_estimate(FindGax2Estimate(rom));
  const auto code_offset = to_offset(param.gax2_estimate());
  param.set_gax2_new(FindGax2New(rom, code_offset));
  param.set_gax2_init(FindGax2Init(rom, code_offset));
  param.set_gax_irq(FindGaxIrq(rom, code_offset));
  param.set_gax_play(FindGaxPlay(rom, code_offset));
  param.set_gax_wram_pointer(FindGaxWorkRamPointer(rom, param.gax_irq()));
  param.set_songs(GaxSongHeader::Scan(rom, param.version()));
  return param;
}

void GaxDriver::InstallGsfDriver(std::string& rom, agbptr_t address,
                                 agbptr_t work_address,
                                 const GaxDriverParam& param) {
  if (!is_romptr(address))
    throw std::invalid_argument("The gsf driver address is not valid.");
  if (!param.ok()) {
    std::ostringstream message;
    message << "Identification of GAX Sound Engine is incomplete."
            << std::endl << std::endl;
    (void)param.WriteAsTable(message);
    throw std::invalid_argument(message.str());
  }

  const agbsize_t offset = to_offset(address);
  if (offset + gsf_driver_size() > rom.size())
    throw std::out_of_range("The address of gsf driver block is out of range.");

  if (work_address == agbnullptr) {
    work_address = 0x3000000; // use IWRAM for default work address
    if (const agbptr_t gax_ptr = param.gax_wram_pointer();
        gax_ptr != agbnullptr && (gax_ptr & ~0xFFFFFF) == work_address) {
      // Since our work area exists in the same memory domain as GAX, there is a possibility of conflict.
      // Using EWRAM avoids collisions, but slower memory access may interfere with playback.
      // ("gameover", Maya the Bee: Sweet Gold, is probably the example of it.)
      if (gax_ptr < 0x3004000) {
        work_address = gax_ptr + 4;
      }
    }
  }

  std::memcpy(&rom[offset], gsf_driver_block, gsf_driver_size());
  WriteInt32L(&rom[offset + kGax2EstimateOffset], param.gax2_estimate() | 1);
  WriteInt32L(&rom[offset + kGax2NewOffset], param.gax2_new() | 1);
  WriteInt32L(&rom[offset + kGax2InitOffset], param.gax2_init() | 1);
  WriteInt32L(&rom[offset + kGaxIrqOffset], param.gax_irq() | 1);
  WriteInt32L(&rom[offset + kGaxPlayOffset], param.gax_play() | 1);

  WriteInt32L(&rom[offset + kMyWorkRamOffset], work_address);

  if (param.version().major_version() == 3) {
    const std::uint8_t sfx_offset =
        param.version().minor_version() < 5 ? 0x2c : 0x30;
    WriteInt8(&rom[offset + kGax2ParamFxImmOffset], sfx_offset);
  }

  WriteInt32L(rom.data(), make_arm_b(0x8000000, address));
}

std::string GaxDriver::NewMinigsfData(const GaxMinigsfDriverParam& param) {
  if (!param.ok()) {
    std::ostringstream message;
    message << "The parameters for creating minigsfs are not sufficient."
            << std::endl
            << std::endl;
    (void)param.WriteAsTable(message);
    throw std::invalid_argument(message.str());
  }

  std::array<char, kMinigsfParamSize> data{0};
  WriteInt32L(&data[kMinigsfParamMyMusicOffset], param.song().address());
  WriteInt32L(&data[kMinigsfParamMyFxOffset], param.fx().has_value() ? param.fx().value().address() : 0);
  WriteInt16L(&data[kMinigsfParamMyFxIdOffset], param.fxid());
  WriteInt16L(&data[kMinigsfParamMyFlagsOffset], param.flags());
  WriteInt16L(&data[kMinigsfParamMyMixingRateOffset], param.mixing_rate());
  WriteInt16L(&data[kMinigsfParamMyVolumeOffset], param.volume());
  return std::string(data.data(), kMinigsfParamSize);
}

std::ostream& GaxDriver::WriteGaxSongsAsTable(
    std::ostream& stream, const std::vector<GaxSongHeader>& songs) {
  using row_t = std::vector<std::string>;
  const row_t header{"Name", "Artist", "Full Name", "Address"};
  std::vector<row_t> items;
  items.reserve(songs.size());
  for (const auto& song : songs) {
    items.push_back(row_t{song.parsed_name(), song.parsed_artist(), song.name(), to_string(song.address())});
  }

  tabulate(stream, header, items);

  return stream;
}

GaxVersion GaxDriver::ParseVersionText(std::string_view version_text) {
  if (version_text.size() < kVersionTextPrefixPattern.size() + 1)
    return GaxVersion{};
  const char c = version_text[kVersionTextPrefixPattern.size()];
  const auto v = (c == 'v' || c == 'V') ? 1 : 0;
  return GaxVersion::Parse(version_text, kVersionTextPrefixPattern.size() + v);
}

std::string GaxDriver::FindGaxVersionText(std::string_view rom,
                                          std::string_view::size_type offset) {
  const auto start_offset = rom.find(kVersionTextPrefixPattern, offset);
  if (start_offset == std::string_view::npos) return std::string{};

  // Limit the maximum length of the text for safety and speed.
  const std::string_view version_text_with_noise{rom.substr(start_offset, 128)};

  // Get the null-terminated string.
  const auto end_offset = version_text_with_noise.find_first_of('\0');
  const std::string_view full_version_text{
      end_offset != std::string_view::npos
          ? version_text_with_noise.substr(0, end_offset)
          : version_text_with_noise};

  // Trim the copyright part.
  // " (C) Shin'en Multimedia. Code: B.Wodok"
  const auto copyright_offset = full_version_text.find_first_of('\xa9');
  return std::string{
      copyright_offset != std::string_view::npos
          ? full_version_text.substr(0, std::max<std::string_view::size_type>(
                                            0, copyright_offset - 1))
          : full_version_text};
}

agbptr_t GaxDriver::FindGax2Estimate(std::string_view rom,
                                std::string_view::size_type offset) {
  using namespace std::string_view_literals;
  constexpr std::string_view pattern{
      "\xf0\xb5\x57\x46\x4e\x46\x45\x46\xe0\xb4\x82\xb0\x07\x1c\x00\x24\x00\x20\x00\x90"sv};
  const auto start_offset = rom.find(pattern, offset);
  return start_offset != std::string_view::npos
             ? to_romptr(static_cast<uint32_t>(start_offset))
             : agbnullptr;
}

agbptr_t GaxDriver::FindGax2New(std::string_view rom,
                                std::string_view::size_type offset) {
  using namespace std::string_view_literals;
  constexpr std::string_view pattern{
      "\xf0\xb5\x47\x46\x80\xb4\x81\xb0\x06\x1c\x00\x2e\x08\xd1\x02\x48\x02\x49"sv};
  const auto start_offset = rom.find(pattern, offset);
  return start_offset != std::string_view::npos
             ? to_romptr(static_cast<uint32_t>(start_offset))
             : agbnullptr;
}

agbptr_t GaxDriver::FindGax2Init(std::string_view rom,
                                 std::string_view::size_type offset) {
  using namespace std::string_view_literals;
  constexpr std::string_view pattern{
      "\xf0\xb5\x57\x46\x4e\x46\x45\x46\xe0\xb4\x81\xb0\x07\x1c\x00\x26\x0e\x48\x39\x68\x01\x60"sv};
  const auto start_offset = rom.find(pattern, offset);
  return start_offset != std::string_view::npos
             ? to_romptr(static_cast<uint32_t>(start_offset))
             : agbnullptr;
}

agbptr_t GaxDriver::FindGaxIrq(std::string_view rom,
                               std::string_view::size_type offset) {
  using namespace std::string_view_literals;
  constexpr std::string_view pattern{
      "\xf0\xb5\x3b\x48\x02\x68\x11\x68\x3a\x48\x81\x42\x6d\xd1\x50\x6d\x00\x28\x6a\xd0\x50\x6d\x01\x28\x1a\xd1\x02\x20\x50\x65\x36\x49"sv};
  const auto start_offset = rom.find(pattern, offset);
  return start_offset != std::string_view::npos
             ? to_romptr(static_cast<uint32_t>(start_offset))
             : agbnullptr;
}

agbptr_t GaxDriver::FindGaxPlay(std::string_view rom,
                                std::string_view::size_type offset) {
  using namespace std::string_view_literals;
  constexpr std::string_view pattern{
      "\x70\xb5\x81\xb0\x47\x48\x01\x68\x48\x6d\x00\x28\x00\xd1"sv};
  const std::string_view::size_type start_offset = rom.find(pattern, offset);
  return start_offset != std::string_view::npos
             ? to_romptr(static_cast<uint32_t>(start_offset))
             : agbnullptr;
}

agbptr_t GaxDriver::FindGaxWorkRamPointer(std::string_view rom, agbptr_t gax_irq) {
  if (gax_irq == agbnullptr) return agbnullptr;

  const agbsize_t offset = to_offset(gax_irq + 0xf0);
  if (offset + 4 >= rom.size()) return agbnullptr;

  const agbptr_t ptr = ReadInt32L(&rom[offset]);
  return is_ewramptr(ptr) || is_iwramptr(ptr) ? ptr : agbnullptr;
}

}  // namespace gaxtapper
