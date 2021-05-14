// Gaxtapper: Automated GSF ripper for GAX Sound Engine.

#ifndef GAXTAPPER_GAX_VERSION_HPP_
#define GAXTAPPER_GAX_VERSION_HPP_

#include <string>

namespace gaxtapper {

enum GaxVersion {
  kGaxUnknownVersion,
  kGaxVersion3
};

static std::string to_string(GaxVersion version) {
  switch (version) {
    case kGaxUnknownVersion:
      return "(unknown)";

    case kGaxVersion3:
      return "3";
  }
  return std::to_string(version);
}

}  // namespace gaxtapper

#endif
