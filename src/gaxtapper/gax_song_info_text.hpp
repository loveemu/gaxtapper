// Gaxtapper: Automated GSF ripper for GAX Sound Engine.

#ifndef GAXTAPPER_GAX_SONG_INFO_TEXT_HPP_
#define GAXTAPPER_GAX_SONG_INFO_TEXT_HPP_

#include <string>

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

 private:
  std::string name_;
  std::string parsed_name_;
  std::string parsed_artist_;
};

}  // namespace gaxtapper

#endif
