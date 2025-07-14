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
#include "gax_music_entry.hpp"
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
  param.set_gax_wram_pointer(FindGaxWorkRamPointer(rom, param.version(), param.gax_play()));
  param.set_songs(GaxMusicEntry::Scan(rom, param.version()));
  return param;
}

void GaxDriver::InstallGsfDriver(std::string& rom, agbptr_t address,
                                 agbptr_t work_address, agbsize_t work_size,
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
  if (offset + gsf_driver_size(param.version()) > rom.size())
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

  if (param.version().major_version() == 3) {
    std::memcpy(&rom[offset], gax3_driver_block.data(),
                gax3_driver_block.size());
    WriteInt32L(&rom[offset + kGax2EstimateOffsetV3],
                param.gax2_estimate() | 1);
    WriteInt32L(&rom[offset + kGax2NewOffsetV3], param.gax2_new() | 1);
    WriteInt32L(&rom[offset + kGax2InitOffsetV3], param.gax2_init() | 1);
    WriteInt32L(&rom[offset + kGaxIrqOffsetV3], param.gax_irq() | 1);
    WriteInt32L(&rom[offset + kGaxPlayOffsetV3], param.gax_play() | 1);

    WriteInt32L(&rom[offset + kMyWorkRamOffsetV3], work_address);

    const std::uint8_t sfx_offset =
        param.version().minor_version() >= 5 ? 0x30 : 0x2c;
    WriteInt8(&rom[offset + kGax2ParamFxImmOffsetV3], sfx_offset);
  } else {
    std::memcpy(&rom[offset], gax2_driver_block.data(),
                gax2_driver_block.size());
    WriteInt32L(&rom[offset + kGax2NewOffsetV2], param.gax2_new() | 1);
    WriteInt32L(&rom[offset + kGax2InitOffsetV2], param.gax2_init() | 1);
    WriteInt32L(&rom[offset + kGaxIrqOffsetV2], param.gax_irq() | 1);
    WriteInt32L(&rom[offset + kGaxPlayOffsetV2], param.gax_play() | 1);

    WriteInt32L(&rom[offset + kMyWorkRamOffsetV2], work_address);
    WriteInt32L(&rom[offset + kMyWorkRamSizeOffsetV2], work_size);
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
  WriteInt32L(&data[kMinigsfParamMyFxOffset], param.fx().has_value() ? param.fx()->address() : 0);
  WriteInt16L(&data[kMinigsfParamMyFxIdOffset], param.fxid());
  WriteInt16L(&data[kMinigsfParamMyFlagsOffset], param.flags());
  WriteInt16L(&data[kMinigsfParamMyMixingRateOffset], param.mixing_rate());
  WriteInt16L(&data[kMinigsfParamMyVolumeOffset], param.volume());
  return std::string(data.data(), kMinigsfParamSize);
}

std::ostream& GaxDriver::WriteGaxSongsAsTable(
    std::ostream& stream, const std::vector<GaxMusicEntry>& songs) {
  using row_t = std::vector<std::string>;
  const row_t header{"Name", "Artist", "Full Name", "Address"};
  std::vector<row_t> items;
  items.reserve(songs.size());
  for (const auto& song : songs) {
    items.push_back(row_t{song.info().parsed_name(),
                          song.info().parsed_artist(), song.info().name(),
                          to_string(song.address())});
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
  constexpr std::array<std::string_view, 5> patterns{
      "\xf0\xb5\x57\x46\x4e\x46\x45\x46\xe0\xb4\x82\xb0\x07\x1c\x00\x24\x00\x20\x00\x90"sv, // GAX 3
      "\xf0\xb5\x57\x46\x4e\x46\x45\x46\xe0\xb4\x8b\xb0\x00\x90\x00\x20\x80\x46\x00\x21"sv, // GAX 2.3
      "\xf0\xb5\x57\x46\x4e\x46\x45\x46\xe0\xb4\x8a\xb0\x81\x46\x00\x27\x00\x20\x02\x90"sv, // GAX 2.2
      "\xf0\xb5\x57\x46\x4e\x46\x45\x46\xe0\xb4\x88\xb0\x00\x90\x00\x27\x00\x20\x02\x90"sv, // GAX 2.1
      "\xf0\xb5\x57\x46\x4e\x46\x45\x46\xe0\xb4\x87\xb0\x00\x90\x00\x27\x00\x20\x02\x90"sv  // GAX 2.02
  };
  for (const std::string_view& pattern : patterns) {
    if (const auto start_offset = rom.find(pattern, offset);
        start_offset != std::string_view::npos) {
      return to_romptr(static_cast<uint32_t>(start_offset));
    }
  }
  return agbnullptr;
}

agbptr_t GaxDriver::FindGax2New(std::string_view rom,
                                std::string_view::size_type offset) {
  using namespace std::string_view_literals;
  constexpr std::array<std::string_view, 2> patterns{
      "\xf0\xb5\x47\x46\x80\xb4\x81\xb0\x06\x1c\x00\x2e"sv, // GAX 2.3 and GAX 3
      "\x10\xb5\x04\x1c\x00\x2c\x09\xd1\x02\x48\x03\x49"sv  // GAX 2.2
  };
  for (const std::string_view& pattern : patterns) {
    if (const auto start_offset = rom.find(pattern, offset);
        start_offset != std::string_view::npos) {
      return to_romptr(static_cast<uint32_t>(start_offset));
    }
  }
  return agbnullptr;
}

agbptr_t GaxDriver::FindGax2Init(std::string_view rom,
                                 std::string_view::size_type offset) {
  using namespace std::string_view_literals;
  constexpr std::array<std::string_view, 6> patterns{
      "\xf0\xb5\x57\x46\x4e\x46\x45\x46\xe0\xb4\x81\xb0\x07\x1c\x00\x26\x0e\x48\x39\x68"sv, // GAX 3
      "\xf0\xb5\x57\x46\x4e\x46\x45\x46\xe0\xb4\x81\xb0\x07\x1c\x00\x22\x0e\x48\x39\x68"sv, // GAX 3.05-ND
      "\xf0\xb5\x57\x46\x4e\x46\x45\x46\xe0\xb4\x86\xb0\x07\x1c\x00\x20\x05\x90\x3a\x68"sv, // GAX 2.3
      "\xf0\xb5\x57\x46\x4e\x46\x45\x46\xe0\xb4\x84\xb0\x07\x1c\x00\x20\x82\x46\x3c\x68"sv, // GAX 2.2
      "\xf0\xb5\x57\x46\x4e\x46\x45\x46\xe0\xb4\x84\xb0\x07\x1c\x00\x20\x81\x46\x3b\x68"sv, // GAX 2.1
      "\xf0\xb5\x57\x46\x4e\x46\x45\x46\xe0\xb4\x83\xb0\x07\x1c\x00\x20\x81\x46\x3b\x68"sv  // GAX 2.02

  };
  for (const std::string_view& pattern : patterns) {
    if (const auto start_offset = rom.find(pattern, offset);
        start_offset != std::string_view::npos) {
      return to_romptr(static_cast<uint32_t>(start_offset));
    }
  }
  return agbnullptr;
}

agbptr_t GaxDriver::FindGaxIrq(std::string_view rom,
                               std::string_view::size_type offset) {
  using namespace std::string_view_literals;
  constexpr std::array<std::string_view, 5> patterns{
      "\xf0\xb5\x3b\x48\x02\x68\x11\x68\x3a\x48\x81\x42\x6d\xd1\x50\x6d\x00\x28\x6a\xd0\x50\x6d\x01\x28\x1a\xd1\x02\x20\x50\x65\x36\x49"sv, // GAX 3
      "\xf0\xb5\x33\x48\x03\x68\x1a\x68\x32\x49\x07\x1c\x8a\x42\x5b\xd1\x58\x6d\x00\x28\x58\xd0\x58\x6d\x01\x28\x1a\xd1\x02\x20\x58\x65"sv, // GAX 3.05-ND
      "\xf0\xb5\x3f\x48\x02\x68\x11\x68\x3e\x48\x81\x42\x75\xd1\x90\x6b\x00\x28\x72\xd0\x90\x6b\x01\x28\x1a\xd1\x3b\x49\x80\x20\x08\x80"sv, // GAX 2.2 and 2.3
      "\x10\xb5\x27\x4c\x23\x68\x19\x68\x26\x48\x81\x42\x44\xd1\x18\x6b\x00\x28\x41\xd0\x18\x6b\x01\x28\x10\xd1\x23\x49\x80\x20\x08\x80"sv, // GAX 2.1
      "\x10\xb5\x25\x4c\x23\x68\x19\x68\x24\x48\x81\x42\x40\xd1\x18\x6b\x00\x28\x3d\xd0\x18\x6b\x01\x28\x10\xd1\x21\x49\x80\x20\x08\x80"sv  // GAX 2.02
  };
  for (const std::string_view & pattern : patterns) {
    if (const auto start_offset = rom.find(pattern, offset);
        start_offset != std::string_view::npos) {
      return to_romptr(static_cast<uint32_t>(start_offset));
    }
  }
  return agbnullptr;
}

agbptr_t GaxDriver::FindGaxPlay(std::string_view rom,
                                std::string_view::size_type offset) {
  using namespace std::string_view_literals;
  constexpr std::array<std::string_view, 4> patterns{
      "\x70\xb5\x81\xb0\x47\x48\x01\x68\x48\x6d\x00\x28\x00\xd1"sv, // GAX 3
      "\xf0\xb5\x81\xb0\x3a\x48\x01\x68\x88\x6b\x00\x28\x00\xd1"sv, // GAX 2.3
      "\xf0\xb5\x30\x4d\x29\x68\x88\x6b\x00\x28\x00\xd1\xd4\xe0"sv, // GAX 2.2
      "\x70\xb5\x4c\x4e\x31\x68\x08\x6b\x00\x28\x00\xd1\x8e\xe0"sv  // GAX 2.1
  };
  for (const std::string_view& pattern : patterns) {
    if (const auto start_offset = rom.find(pattern, offset);
        start_offset != std::string_view::npos) {
      return to_romptr(static_cast<uint32_t>(start_offset));
    }
  }
  return agbnullptr;
}

agbptr_t GaxDriver::FindGaxWorkRamPointer(std::string_view rom,
                                          const GaxVersion& version,
                                          agbptr_t gax_play) {
  if (version.major_version() == 3)
    return FindGaxWorkRamPointerV3(rom, gax_play);
  else
    return FindGaxWorkRamPointerV2(rom, gax_play);
}

agbptr_t GaxDriver::FindGaxWorkRamPointerV2(std::string_view rom,
                                            agbptr_t gax_play) {
  if (gax_play == agbnullptr) return agbnullptr;

  const agbsize_t gax_play_offset = to_offset(gax_play);
  if (gax_play_offset + 0xf4 >= rom.size()) return agbnullptr;

  const agbptr_t rel = ReadInt8(&rom[gax_play_offset + 2]);
  const agbsize_t offset = (rel == 0x30) ? 0xc4 : (rel == 0x4c) ? 0x134 : 0xf0;
  const agbptr_t ptr = ReadInt32L(&rom[gax_play_offset + offset]);
  return is_ewramptr(ptr) || is_iwramptr(ptr) ? ptr : agbnullptr;
}

agbptr_t GaxDriver::FindGaxWorkRamPointerV3(std::string_view rom, agbptr_t gax_play) {
  if (gax_play == agbnullptr) return agbnullptr;

  const agbsize_t offset = to_offset(gax_play + 0x124);
  if (offset + 4 >= rom.size()) return agbnullptr;

  const agbptr_t ptr = ReadInt32L(&rom[offset]);
  return is_ewramptr(ptr) || is_iwramptr(ptr) ? ptr : agbnullptr;
}

}  // namespace gaxtapper
