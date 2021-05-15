// Gaxtapper: Automated GSF ripper for GAX Sound Engine.

#include "gax_song_header.hpp"

#include "bytes.hpp"

namespace gaxtapper {

constexpr std::uint16_t kMaxChannels = 32;

std::optional<GaxSongHeader> GaxSongHeader::TryParse(std::string_view rom, std::string_view::size_type offset) {
  if (offset + 0x20 >= rom.size()) return std::nullopt;

  const std::uint16_t num_channels = ReadInt16L(&rom[offset]);
  if (num_channels == 0 || num_channels > kMaxChannels) return std::nullopt;
  if (offset + 0x20 + 4 * num_channels >= rom.size()) return std::nullopt;

  if (const std::uint16_t reserved = ReadInt16L(&rom[offset + 0x1e]);
      reserved != 0)
    return std::nullopt;

  const agbptr_t seq_address = ReadInt32L(&rom[offset + 0xc]);
  if (!is_romptr(seq_address) || to_offset(seq_address) >= rom.size() ||
      seq_address % 4 != 0)
    return std::nullopt;

  const agbptr_t instrument_address = ReadInt32L(&rom[offset + 0x10]);
  if (!is_romptr(instrument_address) ||
      to_offset(instrument_address) >= rom.size() ||
      instrument_address % 4 != 0)
    return std::nullopt;

  const agbptr_t sample_address = ReadInt32L(&rom[offset + 0x14]);
  if (!is_romptr(sample_address) || to_offset(sample_address) >= rom.size() ||
      sample_address % 4 != 0)
    return std::nullopt;

  std::vector<agbptr_t> pattern_table_address_of_channels;
  pattern_table_address_of_channels.reserve(num_channels);
  for (unsigned int channel = 0; channel < num_channels; channel++) {
    const agbptr_t address = ReadInt32L(&rom[offset + 0x20 + 4 * channel]);
    if (!is_romptr(address) || to_offset(address) >= rom.size() ||
        address % 4 != 0)
      return std::nullopt;
    pattern_table_address_of_channels.push_back(address);
  }

  GaxSongHeader header;
  header.set_address(to_romptr(static_cast<agbsize_t>(offset)));
  header.set_num_channels(num_channels);
  header.set_num_rows_per_pattern(ReadInt16L(&rom[offset + 2]));
  header.set_num_patterns_per_channel(ReadInt16L(&rom[offset + 4]));
  header.set_loop_point(ReadInt16L(&rom[offset + 6]));
  header.set_volume(ReadInt16L(&rom[offset + 8]));
  header.set_seq_address(seq_address);
  header.set_instrument_address(instrument_address);
  header.set_sample_address(sample_address);
  header.set_mixing_rate(ReadInt16L(&rom[offset + 0x18]));
  header.set_fx_mixing_rate(ReadInt16L(&rom[offset + 0x1a]));
  header.set_num_fx_voices(ReadInt8(&rom[offset + 0x1c]));
  header.set_pattern_table_address_of_channels(
      std::move(pattern_table_address_of_channels));
  return std::make_optional(header);
}

std::vector<GaxSongHeader> GaxSongHeader::Scan(
    std::string_view rom, std::string_view::size_type start) {
  start = (start + 3) & ~3;

  std::vector<GaxSongHeader> headers;
  for (auto offset = start; offset < rom.size(); offset += 4) {
    if (const auto header = TryParse(rom, offset); header)
      headers.push_back(header.value());
  }
  return headers;
}

}  // namespace gaxtapper
