// Gaxtapper: Automated GSF ripper for GAX Sound Engine.

#ifndef GAXTAPPER_GAX_MUSIC_ENTRY_V2_HPP_
#define GAXTAPPER_GAX_MUSIC_ENTRY_V2_HPP_

#include <optional>
#include <vector>

#include "gax_song_header_v2.hpp"
#include "gax_song_info_text.hpp"

namespace gaxtapper {

class GaxMusicEntryV2 {
 public:
  GaxMusicEntryV2() = default;

  constexpr operator bool() const { return address_ != agbnullptr; }

  [[nodiscard]] agbptr_t address() const noexcept { return address_; }

  [[nodiscard]] const GaxSongInfoText& info() const noexcept { return info_; }

  [[nodiscard]] const GaxSongHeaderV2& header() const noexcept { return header_; }

  void set_address(agbptr_t address) noexcept { address_ = address; }

  void set_info(GaxSongInfoText info) { info_ = std::move(info); }

  void set_header(GaxSongHeaderV2 header) { header_ = std::move(header); }

  static std::optional<GaxMusicEntryV2> TryParse(
    std::string_view rom, std::string_view::size_type offset);

  static std::vector<GaxMusicEntryV2> Scan(
    std::string_view rom, std::string_view::size_type offset = 0);

 private:
  agbptr_t address_ = agbnullptr;
  GaxSongInfoText info_;
  GaxSongHeaderV2 header_;
};

}  // namespace gaxtapper

#endif
