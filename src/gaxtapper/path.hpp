// Gaxtapper: Automated GSF ripper for GAX Sound Engine.

#ifndef GAXTAPPER_PATH_HPP_
#define GAXTAPPER_PATH_HPP_

#include <filesystem>
#include <string>

namespace gaxtapper {

[[nodiscard]] static std::filesystem::path ToSafeFilenameSegment(
    std::string_view s, char new_char = '~') {
  std::string sanitized;
  sanitized.resize(s.size());
  std::transform(s.begin(), s.end(), sanitized.begin(), [&](char c) -> char {
    switch (c) {
      case '\\':
      case '/':
      case ':':
      case '*':
      case '?':
      case '"':
      case '<':
      case '>':
      case '|':
        return new_char;
      default:
        return c;
    }
  });
  return std::filesystem::path{sanitized};
}

}  // namespace gaxtapper

#endif
