// Gaxtapper: Automated GSF ripper for GAX Sound Engine.

#ifndef GAXTAPPER_GAX_MUSIC_ENTRY_HPP_
#define GAXTAPPER_GAX_MUSIC_ENTRY_HPP_

#include <vector>

#include "gax_song_info_text.hpp"
#include "gax_version.hpp"
#include "types.hpp"

namespace gaxtapper {
class GaxMusicEntryV2;
class GaxSongHeaderV3;

class GaxMusicEntry {
 public:
  GaxMusicEntry() = default;
  GaxMusicEntry(const GaxMusicEntryV2& song);
  GaxMusicEntry(const GaxSongHeaderV3& header);

  constexpr operator bool() const {
    return address_ != agbnullptr;
  }

  [[nodiscard]] agbptr_t address() const noexcept { return address_; }

  [[nodiscard]] const GaxSongInfoText& info() const noexcept { return info_; }

  [[nodiscard]] std::uint16_t num_channels() const noexcept {
    return num_channels_;
  }

  static std::vector<GaxMusicEntry> Scan(
      std::string_view rom, const GaxVersion& version,
      std::string_view::size_type offset = 0);

 private:
  agbptr_t address_ = agbnullptr;
  GaxSongInfoText info_;
  std::uint16_t num_channels_ = 0;
};

}  // namespace gaxtapper

#endif
