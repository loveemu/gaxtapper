// Gaxtapper: Automated GSF ripper for GAX Sound Engine.

#include "gax_music_entry_v2.hpp"

#include <algorithm>

#include "bytes.hpp"
#include "gax_song_header_v2.hpp"
#include "gax_sound_handler_v2.hpp"
#include "types.hpp"

namespace gaxtapper {

std::optional<GaxMusicEntryV2> GaxMusicEntryV2::TryParse(
    std::string_view rom, std::string_view::size_type offset) {
  if (offset + 0x4 >= rom.size()) return std::nullopt;

  const std::uint32_t num_handlers = ReadInt32L(&rom[offset]);
  if (num_handlers < 3 || num_handlers > 255) return std::nullopt;
  if (offset + 4 + num_handlers * 4 >= rom.size()) return std::nullopt;

  std::vector<GaxSoundHandlerV2> handlers;
  handlers.reserve(num_handlers);
  for (uint32_t i = 0; i < num_handlers; i++) {
    const agbptr_t address = ReadInt32L(&rom[offset + 4 + i * 4]);
    if (address != 0 && !is_romptr(address)) return std::nullopt;

    const auto handler = GaxSoundHandlerV2::TryParse(rom, to_offset(address));
    if (!handler) return std::nullopt;
    handlers.push_back(handler.value());
  }

  const GaxSoundHandlerV2 song_header_handler = handlers[1];
  const agbptr_t song_header_address = song_header_handler.data_address();
  const auto maybe_song_header =
      GaxSongHeaderV2::TryParse(rom, to_offset(song_header_address));
  if (!maybe_song_header.has_value()) return std::nullopt;

  GaxSongInfoText info;
  if (!handlers[0].linked_handlers().empty()) {
    std::vector<agbptr_t> pattern_tables;
    pattern_tables.reserve(handlers[0].linked_handlers().size());
    std::transform(handlers[0].linked_handlers().begin(),
                   handlers[0].linked_handlers().end(),
                   std::back_inserter(pattern_tables),
                   [](const GaxSoundHandlerV2& x) { return x.data_address(); });

    const auto top_pattern_table = to_offset(
        *std::min_element(pattern_tables.begin(), pattern_tables.end()));
    info = GaxSongInfoText::ParseInfoTextFromEnd(rom, top_pattern_table);
  }

  GaxMusicEntryV2 song;
  song.set_address(to_romptr(static_cast<agbsize_t>(offset)));
  song.set_info(std::move(info));
  song.set_header(maybe_song_header.value());
  return std::make_optional(song);
}

std::vector<GaxMusicEntryV2> GaxMusicEntryV2::Scan(
    std::string_view rom, std::string_view::size_type start) {
  start = (start + 3) & ~3;

  std::vector<GaxMusicEntryV2> songs;
  for (auto offset = start; offset < rom.size(); offset += 4) {
    if (const auto song = TryParse(rom, offset); song)
      songs.push_back(song.value());
  }
  return songs;
}

}  // namespace gaxtapper
