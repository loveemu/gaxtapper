// Gaxtapper: Automated GSF ripper for GAX Sound Engine.

#ifndef CARTRIDGE_HPP_
#define CARTRIDGE_HPP_

#include <cstdint>
#include <cstring>
#include <filesystem>
#include <string>
#include "arm.hpp"
#include "bytes.hpp"
#include "types.hpp"

namespace gaxtapper {

class Cartridge {
 public:
  using size_type = agbsize_t;

  static constexpr agbsize_t kHeaderSize = 0xc0;
  static constexpr agbsize_t kMaximumSize = 0x2000000;

  Cartridge() = default;

  [[nodiscard]] std::string& rom() { return rom_; }
  [[nodiscard]] const std::string& rom() const { return rom_; }
  [[nodiscard]] size_type size() const { return static_cast<agbsize_t>(rom_.size()); }
  [[nodiscard]] std::string game_title() const {
    std::string_view title = std::string_view{rom_}.substr(0xa0, 12);
    if (const auto end = title.find_first_of('\0'); end != std::string_view::npos) {
      title.remove_suffix(title.size() - end);
    }
    return std::string{title};
  }
  [[nodiscard]] std::string game_code() const { return rom_.substr(0xac, 4); }
  [[nodiscard]] std::string full_game_code() const {
    char full_game_code[] = "AGB-XXXX-XXX";
    std::strncpy(&full_game_code[4], rom_.data() + 0xac, 4);
    std::strncpy(&full_game_code[9], decode_country_code(rom_[0xaf]), 3);
    return std::string{full_game_code};
  }
  [[nodiscard]] agbptr_t entrypoint() const noexcept {
    const armins_t ins = ReadInt32L(rom_.data());
    return is_arm_b(ins) ? arm_b_dest(to_romptr(0), ins) : agbnullptr;
  }

  [[nodiscard]] static const char* decode_country_code(char code) {
    switch (code) {
      case 'C':
        return "CHN";  // China
      case 'D':
        return "DEU";  // German
      case 'E':
        return "USA";  // USA
      case 'F':
        return "FRA";  // France
      case 'H':
        return "HOL";  // ?
      case 'I':
        return "ITA";  // Italy
      case 'J':
        return "JPN";  // Japan
      case 'K':
        return "KOR";  // Korea
      case 'P':
        return "EUR";  // Europe
      case 'Q':
        return "DEN";  // ?
      case 'S':
        return "ESP";  // Spain
      case 'U':
        return "AUS";  // ?
      case 'X':
      case 'Y':
      case 'Z':
        return "EUU";  // ?
      default:
        return "XXX";
    }
  }

  static Cartridge LoadFromFile(const std::filesystem::path& path);

 private:
  std::string rom_;

  static void ValidateSize(std::uintmax_t size);
};

}  // namespace gaxtapper

#endif
