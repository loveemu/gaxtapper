// Gaxtapper: Automated GSF ripper for GAX Sound Engine.

#ifndef GAXTAPPER_GAXTAPPER_HPP_
#define GAXTAPPER_GAXTAPPER_HPP_

#include <filesystem>
#include "cartridge.hpp"

namespace gaxtapper {

static constexpr agbptr_t kGaxtapperGsfDriverAddress = to_romptr(Cartridge::kHeaderSize);

class Gaxtapper {
 public:
  static void ConvertToGsfSet(Cartridge& cartridge,
                              const std::filesystem::path& basename,
                              const std::filesystem::path& outdir = "",
                              const std::string_view& gsfby = "");
  static void Inspect(const Cartridge& cartridge);
  static void InspectSimple(const Cartridge& cartridge, std::string_view name);
};

}  // namespace gaxtapper

#endif
