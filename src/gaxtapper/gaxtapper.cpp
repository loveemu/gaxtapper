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
#include "path.hpp"

namespace gaxtapper {

void Gaxtapper::ConvertToGsfSet(Cartridge& cartridge,
                                const std::filesystem::path& basename,
                                agbptr_t driver_address,
                                agbptr_t work_address,
                                const std::filesystem::path& outdir,
                                const std::string_view& gsfby) {
  if (driver_address != agbnullptr) {
    if (!is_romptr(driver_address)) {
      throw std::invalid_argument(
          "The entrypoint address must point to a ROM section "
          "(0x8000000-0x9FFFFFF).");
    }

    if (driver_address % 4 != 0) {
      throw std::invalid_argument(
          "The entrypoint address must be a multiple of 4.");
    }
  }

  if (work_address != agbnullptr) {
    if (!is_ewramptr(work_address) && !is_iwramptr(work_address)) {
      throw std::invalid_argument(
          "The work RAM address must point to a RAM section.");
    }

    if (work_address % 4 != 0) {
      throw std::invalid_argument(
          "The work RAM address must be a multiple of 4.");
    }
  }

  const GaxDriverParam param = GaxDriver::Inspect(cartridge.rom());
  if (!param.ok()) {
    std::ostringstream message;
    message << "Identification of GAX Sound Engine is incomplete."
            << std::endl
            << std::endl;
    (void)param.WriteAsTable(message);
    throw std::runtime_error(message.str());
  }

  // If the entry point address is not specified, the original entrypoint of the ROM is used as is.
  if (driver_address == agbnullptr) {
    driver_address = cartridge.entrypoint();
    if (driver_address + GaxDriver::gsf_driver_size() >= cartridge.size()) {
      driver_address =
          to_romptr(cartridge.size() - GaxDriver::gsf_driver_size());
    }
  }

  GaxDriver::InstallGsfDriver(cartridge.rom(), driver_address, work_address, param);

  if (!outdir.empty())
    create_directories(outdir);

  std::filesystem::path gsflib_path{outdir};
  gsflib_path /= basename;
  gsflib_path += ".gsflib";

  constexpr agbptr_t kEntrypoint = to_romptr(0);
  const GsfHeader gsf_header{kEntrypoint, kEntrypoint, cartridge.size()};
  GsfWriter::SaveToFile(gsflib_path, gsf_header, cartridge.rom());

  const agbptr_t minigsf_address = GaxDriver::minigsf_address(driver_address);
  for (const GaxSongHeader& song : param.songs()) {
    if (song.num_channels() == 0) continue;

    std::filesystem::path minigsf_filename{
        ToSafeFilenameSegment(song.parsed_name())};
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

void Gaxtapper::InspectSimple(const Cartridge& cartridge,
                              std::string_view name) {
  if (const GaxDriverParam param = GaxDriver::Inspect(cartridge.rom());
      !param.version_text().empty()) {
    std::cout << std::left << std::setw(39) << param.version_text() << " "
              << std::left << std::setw(12) << cartridge.game_title() << " "
              << std::left << std::setw(12) << cartridge.full_game_code() << " "
              << name << std::endl;
  }
}

}  // namespace gaxtapper
