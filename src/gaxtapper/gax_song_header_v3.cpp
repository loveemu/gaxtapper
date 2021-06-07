// Gaxtapper: Automated GSF ripper for GAX Sound Engine.

#include "gax_song_header_v3.hpp"

#include <algorithm>

#include "bytes.hpp"

namespace gaxtapper {

constexpr std::uint16_t kMaxChannels = 32;

std::optional<GaxSongHeaderV3> GaxSongHeaderV3::TryParse(std::string_view rom, std::string_view::size_type offset) {
  if (offset + 0x20 >= rom.size()) return std::nullopt;

  const std::uint16_t num_channels = ReadInt16L(&rom[offset]);
  if (num_channels > kMaxChannels) return std::nullopt;
  if (offset + 0x20 + 4 * num_channels >= rom.size()) return std::nullopt;

  if (const std::uint16_t reserved = ReadInt16L(&rom[offset + 0x1e]);
      reserved != 0)
    return std::nullopt;

  const agbptr_t notes_address = ReadInt32L(&rom[offset + 0xc]);
  // Headers with only instruments and samples are allowed specifically for FX.
  if (num_channels != 0 || notes_address != 0) {
    if (!is_romptr(notes_address) || to_offset(notes_address) >= rom.size() ||
        notes_address % 4 != 0)
      return std::nullopt;
  }

  const agbptr_t instrument_address = ReadInt32L(&rom[offset + 0x10]);
  if (!is_romptr(instrument_address) || instrument_address % 4 != 0)
    return std::nullopt;
  const agbsize_t instrument_offset = to_offset(instrument_address);
  if (instrument_offset + 4 >= rom.size())
    return std::nullopt;
  if (const agbptr_t instrument_ptr = ReadInt32L(&rom[instrument_offset]); !is_romptr(instrument_ptr))
    return std::nullopt;

  const agbptr_t sample_address = ReadInt32L(&rom[offset + 0x14]);
  if (!is_romptr(sample_address) || instrument_address % 4 != 0)
    return std::nullopt;
  const agbsize_t sample_offset = to_offset(sample_address);
  if (sample_offset + 8 >= rom.size())
    return std::nullopt;
  if (const agbptr_t sample_ptr = ReadInt32L(&rom[sample_offset]); !is_romptr(sample_ptr))
    return std::nullopt;
  if (const agbsize_t sample_size = ReadInt32L(&rom[sample_offset + 4]); sample_size != 0)
    return std::nullopt;

  std::vector<agbptr_t> seq_of_channels;
  seq_of_channels.reserve(num_channels);
  for (unsigned int channel = 0; channel < num_channels; channel++) {
    const agbptr_t address = ReadInt32L(&rom[offset + 0x20 + 4 * channel]);
    if (!is_romptr(address) || to_offset(address) >= rom.size() ||
        address % 4 != 0)
      return std::nullopt;
    seq_of_channels.push_back(address);
  }

  const std::uint16_t num_rows_per_pattern = ReadInt16L(&rom[offset + 2]);
  const std::uint16_t num_patterns_per_channel = ReadInt16L(&rom[offset + 4]);
  if (num_channels == 0) {
    if (num_rows_per_pattern != 0 || num_patterns_per_channel != 0)
      return std::nullopt;

    if (const auto s = rom.substr(offset + 0x18, 8);
        s.find_first_not_of(static_cast<char>(0)) != std::string_view::npos)
      return std::nullopt;
  }

  GaxSongHeaderV3 header;
  header.set_address(to_romptr(static_cast<agbsize_t>(offset)));
  header.set_num_channels(num_channels);
  header.set_num_rows_per_pattern(num_rows_per_pattern);
  header.set_num_patterns_per_channel(num_patterns_per_channel);
  header.set_loop_point(ReadInt16L(&rom[offset + 6]));
  header.set_volume(ReadInt16L(&rom[offset + 8]));
  header.set_notes_address(notes_address != 0 ? notes_address : agbnullptr);
  header.set_instrument_address(instrument_address);
  header.set_sample_address(sample_address);
  header.set_mixing_rate(ReadInt16L(&rom[offset + 0x18]));
  header.set_fx_mixing_rate(ReadInt16L(&rom[offset + 0x1a]));
  header.set_num_fx_voices(ReadInt8(&rom[offset + 0x1c]));
  header.set_seq_of_channels(std::move(seq_of_channels));
  if (num_channels != 0)
    header.set_info(header.TryFindInfoText(rom));
  return std::make_optional(header);
}

std::vector<GaxSongHeaderV3> GaxSongHeaderV3::Scan(
    std::string_view rom, std::string_view::size_type start) {
  start = (start + 3) & ~3;

  std::vector<GaxSongHeaderV3> headers;
  for (auto offset = start; offset < rom.size(); offset += 4) {
    if (const auto header = TryParse(rom, offset); header)
      headers.push_back(header.value());
  }
  return headers;
}

GaxSongInfoText GaxSongHeaderV3::TryFindInfoText(std::string_view rom) const {
  // Example value: "wiese" (C) Manfred Linzner
  if (seq_of_channels().empty()) return std::string{};
  const auto seq_offset = to_offset(
      *std::min_element(seq_of_channels().begin(), seq_of_channels().end()));

  // The text is *not* null-terminated and the end is aligned to 32-bit boundary.
  // Find the padding characters at the end.
  if (seq_offset < 4) return std::string{};
  agbsize_t end_offset = seq_offset;
  while (end_offset > seq_offset - 4 && rom[end_offset - 1] == 0) end_offset--;

  // Read backwards while readable characters appear.
  // Note: The start position of the text is not aligned at all.
  agbsize_t offset = end_offset;
  int num_quotes = 0;
  while (offset > 0) {
    const std::uint8_t c = rom[offset - 1];
    const bool ascii_printable = c >= 0x20 && c <= 0x7e;
    const bool western_printable = c >= 0x80 && c != 0x81 && c != 0x8d &&
                                   c != 0x8f && c != 0x90 && c != 0x9d;
    if (!ascii_printable && !western_printable) break;

    offset--;

    if (c == '"') {
      num_quotes++;
      if (num_quotes == 2) break;
    }
  }

  return GaxSongInfoText{std::string{rom.substr(offset, end_offset - offset)}};
}

}  // namespace gaxtapper
