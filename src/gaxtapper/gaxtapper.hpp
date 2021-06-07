// Gaxtapper: Automated GSF ripper for GAX Sound Engine.

#ifndef GAXTAPPER_GAXTAPPER_HPP_
#define GAXTAPPER_GAXTAPPER_HPP_

#include <filesystem>
#include "cartridge.hpp"

namespace gaxtapper {

class GaxMusicEntry;

class Gaxtapper {
 public:
  static void ConvertToGsfSet(Cartridge& cartridge,
                              const std::filesystem::path& basename,
                              agbptr_t driver_address = agbnullptr,
                              agbptr_t work_address = agbnullptr,
                              const std::filesystem::path& outdir = "",
                              const std::string_view& gsfby = "");
  static void Inspect(const Cartridge& cartridge);
  static void InspectSimple(const Cartridge& cartridge, std::string_view name);
  static std::filesystem::path GetMinigsfFilename(
      const GaxMusicEntry& song, const std::filesystem::path& default_name);
};

}  // namespace gaxtapper

#endif
