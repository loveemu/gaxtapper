// Gaxtapper: Automated GSF ripper for GAX Sound Engine.

#ifndef CARTRIDGE_HPP_
#define CARTRIDGE_HPP_

#include <cstdint>
#include <filesystem>
#include <string>
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
  [[nodiscard]] std::string game_title() const { return rom_.substr(0xa0, 12); }
  [[nodiscard]] std::string game_code() const { return rom_.substr(0xac, 4); }

  static Cartridge LoadFromFile(const std::filesystem::path& path);

 private:
  std::string rom_;

  static void ValidateSize(std::uintmax_t size);
};

}  // namespace gaxtapper

#endif
