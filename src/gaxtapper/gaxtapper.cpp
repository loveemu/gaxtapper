// Gaxtapper: Automated GSF ripper for GAX Sound Engine.

#include "gaxtapper.hpp"

#include <filesystem>
#include <iostream>
#include <sstream>
#include <string>
#include "cartridge.hpp"
#include "gsf_header.hpp"
#include "gsf_writer.hpp"
#include "gax_driver.hpp"
#include "gax_driver_param.hpp"
#include "gax_minigsf_driver_param.hpp"

namespace gaxtapper {

void Gaxtapper::ConvertToGsfSet(Cartridge& cartridge,
                                const std::filesystem::path& basename,
                                const std::filesystem::path& outdir,
                                const std::string_view& gsfby) {
  const GaxDriverParam param = GaxDriver::Inspect(cartridge.rom());
  if (!param.ok()) {
    std::ostringstream message;
    message << "Identification of GAX Sound Engine is incomplete."
            << std::endl
            << std::endl;
    (void)param.WriteAsTable(message);
    throw std::runtime_error(message.str());
  }

  std::filesystem::path basepath{outdir};
  basepath /= basename;
  create_directories(basepath.parent_path());

  std::filesystem::path gsflib_path{basepath};
  gsflib_path += ".gsflib";

  constexpr agbptr_t kEntrypoint = to_romptr(0);
  const GsfHeader gsf_header{kEntrypoint, kEntrypoint, cartridge.size()};
  GsfWriter::SaveToFile(gsflib_path, gsf_header, cartridge.rom());

  constexpr agbptr_t minigsf_address =
      GaxDriver::minigsf_address(kGaxtapperGsfDriverAddress);
  for (const agbptr_t song : param.songs()) {
    std::ostringstream song_id;
    song_id << std::setfill('0') << std::setw(8) << std::hex << song;

    std::filesystem::path minigsf_path{basepath};
    minigsf_path += "-";
    minigsf_path += song_id.str();
    minigsf_path += ".minigsf";

    std::map<std::string, std::string> minigsf_tags{
        {"_lib", gsflib_path.filename().string()}};
    if (!gsfby.empty()) minigsf_tags["gsfby"] = gsfby;

    GaxMinigsfDriverParam minigsf{minigsf_address, song};
    std::string minigsf_rom{GaxDriver::NewMinigsfData(minigsf)};
    GsfHeader minigsf_header{kEntrypoint, minigsf_address,
                             static_cast<agbsize_t>(minigsf_rom.size())};
    GsfWriter::SaveToFile(minigsf_path, minigsf_header, minigsf_rom,
                          std::move(minigsf_tags));
  }
}

void Gaxtapper::Inspect(const Cartridge& cartridge) {
  const GaxDriverParam param = GaxDriver::Inspect(cartridge.rom());
  const std::vector<agbptr_t> & songs = param.songs();

  GaxMinigsfDriverParam minigsf;
  constexpr agbptr_t minigsf_address =
      GaxDriver::minigsf_address(kGaxtapperGsfDriverAddress);
  minigsf.set_address(minigsf_address);
  minigsf.set_song(!songs.empty() ? songs[0] : agbnullptr);

  std::cout << "Status: " << (param.ok() ? "OK" : "FAILED") << std::endl
            << std::endl;

  (void)param.WriteAsTable(std::cout);
  std::cout << std::endl;

  std::cout << "minigsf information:" << std::endl << std::endl;
  (void)minigsf.WriteAsTable(std::cout);
}

}  // namespace gaxtapper
