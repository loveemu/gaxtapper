// Gaxtapper: Automated GSF ripper for GAX Sound Engine.

#ifndef GAXTAPPER_GAX_DRIVER_HPP_
#define GAXTAPPER_GAX_DRIVER_HPP_

#include <string>
#include <string_view>
#include <vector>
#include "gax_driver_param.hpp"
#include "gax_minigsf_driver_param.hpp"
#include "types.hpp"

namespace gaxtapper {

class GaxDriver {
 public:
  static constexpr agbsize_t kGax2NewOffset = 0x00;
  static constexpr agbsize_t kGax2InitOffset = 0x04;
  static constexpr agbsize_t kGaxIrqOffset = 0x08;
  static constexpr agbsize_t kGaxPlayOffset = 0x0c;
  static constexpr agbsize_t kMinigsfParamOffset = 0x10;

  GaxDriver() = default;

  [[nodiscard]] static constexpr agbsize_t gsf_driver_size() noexcept {
    return sizeof(gsf_driver_block);
  }

  [[nodiscard]] static constexpr agbptr_t minigsf_address(agbptr_t gsf_driver_address) noexcept {
    return (gsf_driver_address != agbnullptr)
               ? (gsf_driver_address + kMinigsfParamOffset)
               : agbnullptr;
  }

  [[nodiscard]] static std::string name() { return "GAX Sound Engine"; }

  [[nodiscard]] static GaxDriverParam Inspect(std::string_view rom);

  static void InstallGsfDriver(std::string& rom, agbptr_t address,
                               const GaxDriverParam& param);

  static std::string NewMinigsfData(const GaxMinigsfDriverParam& param);

 private:
  static constexpr unsigned char gsf_driver_block[20] = {
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; // TODO: write a real driver

  static GaxVersion ParseVersionText(std::string_view version_text);
  static std::string_view FindGaxVersionText(std::string_view rom);
  static agbptr_t FindGax2New(std::string_view rom);
  static agbptr_t FindGax2Init(std::string_view rom);
  static agbptr_t FindGaxIrq(std::string_view rom);
  static agbptr_t FindGaxPlay(std::string_view rom);
  static std::vector<GaxSongParam> FindGaxSongs(std::string_view rom);
};

}  // namespace gaxtapper

#endif
