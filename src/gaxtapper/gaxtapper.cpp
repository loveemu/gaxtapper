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

  GaxDriver::InstallGsfDriver(cartridge.rom(), kGaxtapperGsfDriverAddress,
                              param);

  if (!outdir.empty())
    create_directories(outdir);

  std::filesystem::path gsflib_path{outdir};
  gsflib_path /= basename;
  gsflib_path += ".gsflib";

  constexpr agbptr_t kEntrypoint = to_romptr(0);
  const GsfHeader gsf_header{kEntrypoint, kEntrypoint, cartridge.size()};
  GsfWriter::SaveToFile(gsflib_path, gsf_header, cartridge.rom());

  constexpr agbptr_t minigsf_address =
      GaxDriver::minigsf_address(kGaxtapperGsfDriverAddress);
  for (const GaxSongHeader & song : param.songs()) {
    if (song.num_channels() == 0) continue;

    std::filesystem::path minigsf_filename{song.parsed_name()};
    if (minigsf_filename.empty()) {
      std::ostringstream song_id;
      song_id << std::setfill('0') << std::setw(8) << std::hex
              << song.address();
      minigsf_filename += basename;
      minigsf_filename += "-";
      minigsf_filename += song_id.str();
    }
    minigsf_filename += ".minigsf";

    std::filesystem::path minigsf_path{outdir};
    minigsf_path /= minigsf_filename;

    std::map<std::string, std::string> minigsf_tags{
        {"_lib", gsflib_path.filename().string()}};
    if (!gsfby.empty()) minigsf_tags["gsfby"] = gsfby;

    GaxMinigsfDriverParam minigsf{minigsf_address, GaxSongParam::Of(song)};
    std::string minigsf_rom{GaxDriver::NewMinigsfData(minigsf)};
    GsfHeader minigsf_header{kEntrypoint, minigsf_address,
                             static_cast<agbsize_t>(minigsf_rom.size())};
    if (!minigsf.song().artist().empty())
      minigsf_tags["artist"] = minigsf.song().artist();
    GsfWriter::SaveToFile(minigsf_path, minigsf_header, minigsf_rom,
                          std::move(minigsf_tags));
  }
}

void Gaxtapper::Inspect(const Cartridge& cartridge) {
  const GaxDriverParam param = GaxDriver::Inspect(cartridge.rom());
  const std::vector<GaxSongHeader> & songs = param.songs();

  std::cout << "Status: " << (param.ok() ? "OK" : "FAILED") << std::endl
            << std::endl;

  (void)param.WriteAsTable(std::cout);

  if (!songs.empty()) {
    std::cout << std::endl;
    std::cout << "Songs and Samples:" << std::endl << std::endl;
    (void)GaxDriver::WriteGaxSongsAsTable(std::cout, songs);
  }
}

void Gaxtapper::InspectSimple(const Cartridge& cartridge, std::string_view name) {
  if (const GaxDriverParam param = GaxDriver::Inspect(cartridge.rom()); !param.version_text().empty())
    std::cout << std::left << std::setw(39) << param.version_text() << " " << name << std::endl;
}

}  // namespace gaxtapper
