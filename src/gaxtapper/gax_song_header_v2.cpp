// Gaxtapper: Automated GSF ripper for GAX Sound Engine.

#include "gax_song_header_v2.hpp"

#include "bytes.hpp"

namespace gaxtapper {

constexpr std::uint16_t kMaxChannels = 32;

std::optional<GaxSongHeaderV2> GaxSongHeaderV2::TryParse(std::string_view rom, std::string_view::size_type offset) {
  if (offset + 0x20 >= rom.size()) return std::nullopt;

  const std::uint16_t num_channels = ReadInt16L(&rom[offset]);
  if (num_channels == 0 || num_channels > kMaxChannels) return std::nullopt;

  const agbptr_t notes_address = ReadInt32L(&rom[offset + 0xc]);
  if (!is_romptr(notes_address) || to_offset(notes_address) >= rom.size() ||
      notes_address % 4 != 0)
    return std::nullopt;

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

  const std::uint16_t num_rows_per_pattern = ReadInt16L(&rom[offset + 2]);
  const std::uint16_t num_patterns_per_channel = ReadInt16L(&rom[offset + 4]);

  GaxSongHeaderV2 header;
  header.set_address(to_romptr(static_cast<agbsize_t>(offset)));
  header.set_num_channels(num_channels);
  header.set_num_rows_per_pattern(num_rows_per_pattern);
  header.set_num_patterns_per_channel(num_patterns_per_channel);
  header.set_loop_point(ReadInt16L(&rom[offset + 6]));
  header.set_volume(ReadInt16L(&rom[offset + 8]));
  header.set_notes_address(notes_address);
  header.set_instrument_address(instrument_address);
  header.set_sample_address(sample_address);
  header.set_mixing_rate(ReadInt16L(&rom[offset + 0x18]));
  header.set_num_fx_voices(ReadInt8(&rom[offset + 0x1a]));
  return std::make_optional(header);
}

}  // namespace gaxtapper
