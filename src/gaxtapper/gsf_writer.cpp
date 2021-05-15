// Gaxtapper: Automated GSF ripper for GAX Sound Engine.

#include "gsf_writer.hpp"

#include <filesystem>
#include <fstream>
#include <string_view>
#include <utility>
#include "gsf_header.hpp"
#include "psf_writer.hpp"
#include "types.hpp"

namespace gaxtapper {

void GsfWriter::SaveToFile(const std::filesystem::path& path,
                           const GsfHeader& header, std::string_view rom,
                           std::map<std::string, std::string> tags) {
  std::ofstream file(path, std::ios::out | std::ios::binary);
  file.exceptions(std::ios::badbit | std::ios::failbit);
  SaveToStream(file, header, rom, std::move(tags));
  file.close();
}

void GsfWriter::SaveToStream(std::ostream& out, const GsfHeader& header,
                             std::string_view rom,
                             std::map<std::string, std::string> tags) {
  PsfWriter psf{kVersion, std::move(tags)};
  auto& exe = psf.exe();
  exe.write(header.data(), header.size());
  exe.write(rom.data(), rom.size());
  psf.SaveToStream(out);
}

}  // namespace gaxtapper
