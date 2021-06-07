// Gaxtapper: Automated GSF ripper for GAX Sound Engine.

#include "gax_music_entry.hpp"

#include "bytes.hpp"
#include "gax_music_entry_v2.hpp"
#include "gax_song_header_v3.hpp"

namespace gaxtapper {

GaxMusicEntry::GaxMusicEntry(const GaxMusicEntryV2& song)
    : address_(song.address()),
      info_(song.info()),
      num_channels_(song.header().num_channels()) {}

GaxMusicEntry::GaxMusicEntry(const GaxSongHeaderV3& header)
    : address_(header.address()),
      info_(header.info()),
      num_channels_(header.num_channels()) {}

std::vector<GaxMusicEntry> GaxMusicEntry::Scan(
    std::string_view rom, const GaxVersion& version,
    std::string_view::size_type offset) {
  if (version.major_version() == 3) {
    std::vector headers{GaxSongHeaderV3::Scan(rom, offset)};
    return std::vector<GaxMusicEntry>{headers.begin(), headers.end()};
  } else {
    std::vector songs{GaxMusicEntryV2::Scan(rom, offset)};
    return std::vector<GaxMusicEntry>{songs.begin(), songs.end()};
  }
}

}  // namespace gaxtapper
