// Gaxtapper: Automated GSF ripper for GAX Sound Engine.

#ifndef GAXTAPPER_GAX_SONG_INFO_TEXT_HPP_
#define GAXTAPPER_GAX_SONG_INFO_TEXT_HPP_

#include <string>

#include "types.hpp"

namespace gaxtapper {

class GaxSongInfoText {
 public:
  GaxSongInfoText() = default;

  GaxSongInfoText(std::string name) : name_(std::move(name)) {
    parsed_name_ = name_;
    parsed_artist_ = std::string{};

    if (name_.empty() || name_[0] != '"') return;

    const std::string::size_type offset = 1;
    const auto end_offset = name_.find_first_of('"', offset);
    if (end_offset == std::string::npos) return;

    parsed_name_ = name_.substr(offset, end_offset - offset);

    const std::string_view kCopyrightPrefix{" \xa9 "};
    if (name_.substr(end_offset + 1, kCopyrightPrefix.size()) !=
        kCopyrightPrefix)
      return;

    parsed_artist_ = name_.substr(end_offset + 1 + kCopyrightPrefix.size());
  }

  [[nodiscard]] std::string name() const noexcept { return name_; }

  [[nodiscard]] std::string parsed_name() const noexcept { return parsed_name_; }

  [[nodiscard]] std::string parsed_artist() const noexcept {
    return parsed_artist_;
  }

  [[nodiscard]] static GaxSongInfoText ParseInfoTextFromEnd(
      std::string_view rom, agbsize_t end_offset) {
    if (end_offset < 4 || end_offset > rom.size()) return GaxSongInfoText{};

    // The text is *not* null-terminated and the end is aligned to 32-bit
    // boundary. Find the padding characters at the end.
    //
    // Example value: "wiese" (C) Manfred Linzner
    while (end_offset > end_offset - 4 && rom[end_offset - 1] == 0)
      end_offset--;

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

    return GaxSongInfoText{
        std::string{rom.substr(offset, end_offset - offset)}};
  }

 private:
  std::string name_;
  std::string parsed_name_;
  std::string parsed_artist_;
};

}  // namespace gaxtapper

#endif
