// Gaxtapper: Automated GSF ripper for GAX Sound Engine.

#ifndef GAXTAPPER_GAX_DRIVER_HPP_
#define GAXTAPPER_GAX_DRIVER_HPP_

#include <array>
#include <string>
#include <string_view>
#include <vector>
#include "gax_driver_param.hpp"
#include "gax_minigsf_driver_param.hpp"
#include "types.hpp"

namespace gaxtapper {

class GaxDriver {
 public:
  static constexpr agbsize_t kMyWorkRamOffsetV2 = 0xc4;
  static constexpr agbsize_t kMyWorkRamSizeOffsetV2 = 0xc8;
  static constexpr agbsize_t kMinigsfParamBaseV2 = 0xcc;
  static constexpr agbsize_t kGax2NewOffsetV2 = 0x1e0;
  static constexpr agbsize_t kGax2InitOffsetV2 = 0x1e8;
  static constexpr agbsize_t kGaxIrqOffsetV2 = 0x1f0;
  static constexpr agbsize_t kGaxPlayOffsetV2 = 0x1f8;

  static constexpr agbsize_t kMyWorkRamOffsetV3 = 0xcc;
  static constexpr agbsize_t kMinigsfParamBaseV3 = 0xd0;
  static constexpr agbsize_t kGax2EstimateOffsetV3 = 0x1e4;
  static constexpr agbsize_t kGax2NewOffsetV3 = 0x1ec;
  static constexpr agbsize_t kGax2InitOffsetV3 = 0x1f4;
  static constexpr agbsize_t kGaxIrqOffsetV3 = 0x1fc;
  static constexpr agbsize_t kGaxPlayOffsetV3 = 0x204;
  static constexpr agbsize_t kGax2ParamFxImmOffsetV3 = 0x7c;

  static constexpr agbsize_t kMinigsfParamMyMusicOffset = 0;
  static constexpr agbsize_t kMinigsfParamMyFxOffset = 4;
  static constexpr agbsize_t kMinigsfParamMyFxIdOffset = 8;
  static constexpr agbsize_t kMinigsfParamMyFlagsOffset = 0xa;
  static constexpr agbsize_t kMinigsfParamMyMixingRateOffset = 0xc;
  static constexpr agbsize_t kMinigsfParamMyVolumeOffset = 0xe;
  static constexpr agbsize_t kMinigsfParamSize = 0x10;

  static constexpr std::array<unsigned char, 0x1FC> gax2_driver_block = {
      0x12, 0x00, 0xA0, 0xE3, 0x00, 0xF0, 0x29, 0xE1, 0x30, 0xD0, 0x9F, 0xE5,
      0x1F, 0x00, 0xA0, 0xE3, 0x00, 0xF0, 0x29, 0xE1, 0x20, 0xD0, 0x9F, 0xE5,
      0x24, 0x10, 0x9F, 0xE5, 0x42, 0x0F, 0x8F, 0xE2, 0x00, 0x00, 0x81, 0xE5,
      0xFF, 0x00, 0xA0, 0xE3, 0x00, 0x00, 0x01, 0xEF, 0x15, 0x10, 0x8F, 0xE2,
      0x0F, 0xE0, 0xA0, 0xE1, 0x11, 0xFF, 0x2F, 0xE1, 0xFE, 0xFF, 0xFF, 0xEA,
      0x00, 0x7F, 0x00, 0x03, 0xA0, 0x7F, 0x00, 0x03, 0xFC, 0x7F, 0x00, 0x03,
      0xF0, 0xB5, 0x90, 0xB0, 0x14, 0x48, 0x15, 0x49, 0x01, 0x80, 0x15, 0xA3,
      0x80, 0x20, 0x40, 0x04, 0x18, 0x68, 0x04, 0x33, 0xA0, 0x42, 0xFB, 0xDC,
      0x6C, 0x46, 0x20, 0x00, 0x00, 0xF0, 0xBA, 0xF8, 0x18, 0xA5, 0x68, 0x89,
      0xE0, 0x81, 0xA8, 0x89, 0x20, 0x81, 0xE8, 0x89, 0x60, 0x82, 0x04, 0x20,
      0x20, 0x82, 0x68, 0x68, 0xE0, 0x62, 0x28, 0x68, 0x20, 0x63, 0x11, 0x48,
      0x60, 0x60, 0x9C, 0x21, 0x0E, 0x48, 0x40, 0x18, 0x20, 0x60, 0x20, 0x00,
      0x00, 0xF0, 0xA8, 0xF8, 0x28, 0x89, 0x0B, 0x48, 0x00, 0xF0, 0x20, 0xF8,
      0x02, 0xDF, 0xFD, 0xE7, 0x04, 0x02, 0x00, 0x04, 0x14, 0x40, 0x00, 0x00,
      0x52, 0x69, 0x70, 0x70, 0x65, 0x64, 0x20, 0x62, 0x79, 0x20, 0x47, 0x61,
      0x78, 0x74, 0x61, 0x70, 0x70, 0x65, 0x72, 0x20, 0x30, 0x2E, 0x30, 0x31,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x20, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x08, 0xFF, 0xFF, 0xFF, 0xFF,
      0xFF, 0xFF, 0xFF, 0xFF, 0x10, 0xB5, 0x04, 0x00, 0x0D, 0x4B, 0x12, 0xA0,
      0x18, 0x60, 0x5C, 0x60, 0x0C, 0x48, 0x98, 0x60, 0x98, 0x68, 0x94, 0x20,
      0x23, 0x18, 0x36, 0xA0, 0x01, 0x30, 0x18, 0x60, 0x33, 0xA1, 0x01, 0x31,
      0x59, 0x60, 0x08, 0x48, 0x04, 0x60, 0x08, 0x4A, 0x08, 0x4B, 0x01, 0x21,
      0x19, 0x81, 0x19, 0x80, 0x08, 0x20, 0x90, 0x80, 0x10, 0xBC, 0x01, 0xBC,
      0x00, 0x47, 0x00, 0x00, 0xD4, 0x00, 0x00, 0x04, 0x25, 0x00, 0x00, 0x84,
      0xFC, 0x7F, 0x00, 0x03, 0x00, 0x00, 0x00, 0x04, 0x00, 0x02, 0x00, 0x04,
      0x01, 0x33, 0xA0, 0xE3, 0x02, 0x3C, 0x83, 0xE2, 0x00, 0x20, 0x93, 0xE5,
      0xB8, 0x10, 0xD3, 0xE1, 0x00, 0x00, 0x4F, 0xE1, 0x0F, 0x40, 0x2D, 0xE9,
      0x00, 0x00, 0xA0, 0xE3, 0xB8, 0x00, 0xC3, 0xE1, 0x22, 0x18, 0x02, 0xE0,
      0x00, 0xC0, 0xA0, 0xE3, 0x01, 0x00, 0x11, 0xE2, 0x03, 0x00, 0x00, 0x1A,
      0x04, 0xC0, 0x8C, 0xE2, 0x02, 0x0A, 0x11, 0xE2, 0x7C, 0x01, 0x43, 0x15,
      0xFE, 0xFF, 0xFF, 0x1A, 0xB2, 0x00, 0xC3, 0xE1, 0x00, 0x30, 0x0F, 0xE1,
      0xDF, 0x30, 0xC3, 0xE3, 0x1F, 0x30, 0x83, 0xE3, 0x03, 0xF0, 0x29, 0xE1,
      0x38, 0x10, 0x8F, 0xE2, 0x0C, 0x10, 0x81, 0xE0, 0x00, 0x00, 0x91, 0xE5,
      0x00, 0x40, 0x2D, 0xE9, 0x00, 0xE0, 0x8F, 0xE2, 0x10, 0xFF, 0x2F, 0xE1,
      0x00, 0x40, 0xBD, 0xE8, 0x00, 0x30, 0x0F, 0xE1, 0xDF, 0x30, 0xC3, 0xE3,
      0x92, 0x30, 0x83, 0xE3, 0x03, 0xF0, 0x29, 0xE1, 0x0F, 0x40, 0xBD, 0xE8,
      0xB0, 0x20, 0xC3, 0xE1, 0xB8, 0x10, 0xC3, 0xE1, 0x00, 0xF0, 0x69, 0xE1,
      0x1E, 0xFF, 0x2F, 0xE1, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x70, 0x47, 0x00, 0x00, 0x00, 0xB5, 0x00, 0xF0, 0x0D, 0xF8, 0x00, 0xF0,
      0x0F, 0xF8, 0x01, 0xBC, 0x00, 0x47, 0x00, 0x00, 0x00, 0x49, 0x08, 0x47,
      0x00, 0x00, 0x00, 0x08, 0x00, 0x49, 0x08, 0x47, 0x00, 0x00, 0x00, 0x08,
      0x00, 0x48, 0x00, 0x47, 0x00, 0x00, 0x00, 0x08, 0x00, 0x48, 0x00, 0x47,
      0x00, 0x00, 0x00, 0x08};

  static constexpr std::array<unsigned char, 0x208> gax3_driver_block = {
      0x12, 0x00, 0xA0, 0xE3, 0x00, 0xF0, 0x29, 0xE1, 0x30, 0xD0, 0x9F, 0xE5,
      0x1F, 0x00, 0xA0, 0xE3, 0x00, 0xF0, 0x29, 0xE1, 0x20, 0xD0, 0x9F, 0xE5,
      0x24, 0x10, 0x9F, 0xE5, 0x43, 0x0F, 0x8F, 0xE2, 0x00, 0x00, 0x81, 0xE5,
      0xFF, 0x00, 0xA0, 0xE3, 0x00, 0x00, 0x01, 0xEF, 0x15, 0x10, 0x8F, 0xE2,
      0x0F, 0xE0, 0xA0, 0xE1, 0x11, 0xFF, 0x2F, 0xE1, 0xFE, 0xFF, 0xFF, 0xEA,
      0x00, 0x7F, 0x00, 0x03, 0xA0, 0x7F, 0x00, 0x03, 0xFC, 0x7F, 0x00, 0x03,
      0xF0, 0xB5, 0x90, 0xB0, 0x16, 0x48, 0x17, 0x49, 0x01, 0x80, 0x17, 0xA3,
      0x80, 0x20, 0x40, 0x04, 0x18, 0x68, 0x04, 0x33, 0xA0, 0x42, 0xFB, 0xDC,
      0x6C, 0x46, 0x20, 0x00, 0x00, 0xF0, 0xC0, 0xF8, 0x19, 0xA5, 0x68, 0x89,
      0xE0, 0x81, 0xA8, 0x89, 0x20, 0x81, 0xE8, 0x89, 0x60, 0x82, 0x04, 0x20,
      0x20, 0x82, 0x68, 0x68, 0x30, 0x23, 0x1B, 0x19, 0x18, 0x60, 0x28, 0x68,
      0x04, 0x33, 0x18, 0x60, 0x20, 0x00, 0x00, 0xF0, 0xA9, 0xF8, 0x9C, 0x21,
      0x0E, 0x48, 0x40, 0x18, 0x20, 0x60, 0x20, 0x00, 0x00, 0xF0, 0xAA, 0xF8,
      0x28, 0x89, 0x0B, 0x48, 0x00, 0xF0, 0x1E, 0xF8, 0x02, 0xDF, 0xFD, 0xE7,
      0x04, 0x02, 0x00, 0x04, 0x14, 0x40, 0x00, 0x00, 0x52, 0x69, 0x70, 0x70,
      0x65, 0x64, 0x20, 0x62, 0x79, 0x20, 0x47, 0x61, 0x78, 0x74, 0x61, 0x70,
      0x70, 0x65, 0x72, 0x20, 0x30, 0x2E, 0x30, 0x31, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x08,
      0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x10, 0xB5, 0x04, 0x00,
      0x0D, 0x4B, 0x12, 0xA0, 0x18, 0x60, 0x5C, 0x60, 0x0C, 0x48, 0x98, 0x60,
      0x98, 0x68, 0x94, 0x20, 0x23, 0x18, 0x36, 0xA0, 0x01, 0x30, 0x18, 0x60,
      0x33, 0xA1, 0x01, 0x31, 0x59, 0x60, 0x08, 0x48, 0x04, 0x60, 0x08, 0x4A,
      0x08, 0x4B, 0x01, 0x21, 0x19, 0x81, 0x19, 0x80, 0x08, 0x20, 0x90, 0x80,
      0x10, 0xBC, 0x01, 0xBC, 0x00, 0x47, 0x00, 0x00, 0xD4, 0x00, 0x00, 0x04,
      0x25, 0x00, 0x00, 0x84, 0xFC, 0x7F, 0x00, 0x03, 0x00, 0x00, 0x00, 0x04,
      0x00, 0x02, 0x00, 0x04, 0x01, 0x33, 0xA0, 0xE3, 0x02, 0x3C, 0x83, 0xE2,
      0x00, 0x20, 0x93, 0xE5, 0xB8, 0x10, 0xD3, 0xE1, 0x00, 0x00, 0x4F, 0xE1,
      0x0F, 0x40, 0x2D, 0xE9, 0x00, 0x00, 0xA0, 0xE3, 0xB8, 0x00, 0xC3, 0xE1,
      0x22, 0x18, 0x02, 0xE0, 0x00, 0xC0, 0xA0, 0xE3, 0x01, 0x00, 0x11, 0xE2,
      0x03, 0x00, 0x00, 0x1A, 0x04, 0xC0, 0x8C, 0xE2, 0x02, 0x0A, 0x11, 0xE2,
      0x7C, 0x01, 0x43, 0x15, 0xFE, 0xFF, 0xFF, 0x1A, 0xB2, 0x00, 0xC3, 0xE1,
      0x00, 0x30, 0x0F, 0xE1, 0xDF, 0x30, 0xC3, 0xE3, 0x1F, 0x30, 0x83, 0xE3,
      0x03, 0xF0, 0x29, 0xE1, 0x38, 0x10, 0x8F, 0xE2, 0x0C, 0x10, 0x81, 0xE0,
      0x00, 0x00, 0x91, 0xE5, 0x00, 0x40, 0x2D, 0xE9, 0x00, 0xE0, 0x8F, 0xE2,
      0x10, 0xFF, 0x2F, 0xE1, 0x00, 0x40, 0xBD, 0xE8, 0x00, 0x30, 0x0F, 0xE1,
      0xDF, 0x30, 0xC3, 0xE3, 0x92, 0x30, 0x83, 0xE3, 0x03, 0xF0, 0x29, 0xE1,
      0x0F, 0x40, 0xBD, 0xE8, 0xB0, 0x20, 0xC3, 0xE1, 0xB8, 0x10, 0xC3, 0xE1,
      0x00, 0xF0, 0x69, 0xE1, 0x1E, 0xFF, 0x2F, 0xE1, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x70, 0x47, 0x00, 0x00, 0x00, 0xB5, 0x00, 0xF0,
      0x11, 0xF8, 0x00, 0xF0, 0x13, 0xF8, 0x01, 0xBC, 0x00, 0x47, 0x00, 0x00,
      0x00, 0x49, 0x08, 0x47, 0x00, 0x00, 0x00, 0x08, 0x00, 0x49, 0x08, 0x47,
      0x00, 0x00, 0x00, 0x08, 0x00, 0x49, 0x08, 0x47, 0x00, 0x00, 0x00, 0x08,
      0x00, 0x48, 0x00, 0x47, 0x00, 0x00, 0x00, 0x08, 0x00, 0x48, 0x00, 0x47,
      0x00, 0x00, 0x00, 0x08};

  GaxDriver() = default;

  [[nodiscard]] static agbsize_t gsf_driver_size(
      const GaxVersion& version) noexcept {
    return version.major_version() == 3
               ? static_cast<agbsize_t>(gax3_driver_block.size())
               : static_cast<agbsize_t>(gax2_driver_block.size());
  }

  [[nodiscard]] static constexpr agbptr_t minigsf_address(agbptr_t gsf_driver_address, const GaxVersion& version) noexcept {
    if (gsf_driver_address == agbnullptr) return agbnullptr;
    const agbsize_t offset = (version.major_version() == 3)
                                 ? kMinigsfParamBaseV3
                                 : kMinigsfParamBaseV2;
    return gsf_driver_address + offset;
  }

  [[nodiscard]] static std::string name() { return "GAX Sound Engine"; }

  [[nodiscard]] static GaxDriverParam Inspect(std::string_view rom);

  static void InstallGsfDriver(std::string& rom, agbptr_t address,
                               agbptr_t work_address, agbsize_t work_size,
                               const GaxDriverParam& param);

  static std::string NewMinigsfData(const GaxMinigsfDriverParam& param);

  static std::ostream& WriteGaxSongsAsTable(
      std::ostream& stream, const std::vector<GaxMusicEntry>& songs);

 private:
  static GaxVersion ParseVersionText(std::string_view version_text);
  static std::string FindGaxVersionText(std::string_view rom,
                                        std::string_view::size_type offset = 0);
  static agbptr_t FindGax2Estimate(std::string_view rom,
                              std::string_view::size_type offset = 0);
  static agbptr_t FindGax2New(std::string_view rom,
                              std::string_view::size_type offset = 0);
  static agbptr_t FindGax2Init(std::string_view rom,
                               std::string_view::size_type offset = 0);
  static agbptr_t FindGaxIrq(std::string_view rom,
                             std::string_view::size_type offset = 0);
  static agbptr_t FindGaxPlay(std::string_view rom,
                              std::string_view::size_type offset = 0);
  static agbptr_t FindGaxWorkRamPointer(std::string_view rom,
                                        const GaxVersion& version,
                                        agbptr_t gax_play = agbnullptr);
  static agbptr_t FindGaxWorkRamPointerV2(std::string_view rom,
                                          agbptr_t gax_play = agbnullptr);
  static agbptr_t FindGaxWorkRamPointerV3(std::string_view rom,
                                 agbptr_t gax_play = agbnullptr);
};

}  // namespace gaxtapper

#endif
