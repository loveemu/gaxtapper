// Gaxtapper: Automated GSF ripper for GAX Sound Engine.

#ifndef GAXTAPPER_GSF_WRITER_HPP_
#define GAXTAPPER_GSF_WRITER_HPP_

#include <cstdint>
#include <filesystem>
#include <map>
#include <string>
#include "gsf_header.hpp"

namespace gaxtapper {

class GsfWriter {
 public:
  static void SaveToFile(const std::filesystem::path& path,
                         const GsfHeader& header, std::string_view rom,
                         std::map<std::string, std::string> tags = {});

  static void SaveToStream(std::ostream& out, const GsfHeader& header,
                           std::string_view rom,
                           std::map<std::string, std::string> tags = {});

 private:
  static constexpr std::uint8_t kVersion = 0x22;
};

}  // namespace gaxtapper

#endif
