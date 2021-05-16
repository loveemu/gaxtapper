// Gaxtapper: Automated GSF ripper for GAX Sound Engine.

#include "gax_version.hpp"

#include <charconv>
#include <string>

namespace gaxtapper {

GaxVersion GaxVersion::Parse(std::string_view s,
                             std::string_view::size_type offset) {
  GaxVersion version;
  if (offset >= s.size()) return version;

  const char* major = s.data() + offset;
  const char* end = s.data() + s.size();
  int major_version;
  const auto [ptr, ec] = std::from_chars(major, end, major_version);
  if (ec == std::errc::invalid_argument) return version;
  version.set_major_version(major_version);

  if (ptr == end || *ptr != '.') return version;
  const char* minor = ptr + 1;
  int minor_version;
  const auto [_, ec2] = std::from_chars(minor, end, minor_version);
  if (ec2 == std::errc::invalid_argument) return version;
  version.set_minor_version(minor_version);

  return version;
}

}  // namespace gaxtapper
