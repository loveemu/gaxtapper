// Gaxtapper: Automated GSF ripper for GAX Sound Engine.

#ifndef GAXTAPPER_GAX_VERSION_HPP_
#define GAXTAPPER_GAX_VERSION_HPP_

#include <string_view>

namespace gaxtapper {

class GaxVersion {
 public:
  GaxVersion() = default;

  GaxVersion(int major_version, int minor_version)
      : major_version_(major_version), minor_version_(minor_version) {}

  constexpr operator bool() const { return major_version_ != 0 || minor_version_ != 0; }

  [[nodiscard]] int major_version() const noexcept { return major_version_; }

  [[nodiscard]] int minor_version() const noexcept { return minor_version_; }

  void set_major_version(int major_version) noexcept {
    major_version_ = major_version;
  }

  void set_minor_version(int minor_version) noexcept {
    minor_version_ = minor_version;
  }

  static GaxVersion Parse(std::string_view s,
                          std::string_view::size_type offset = 0);

 private:
  int major_version_ = 0;
  int minor_version_ = 0;
};

}  // namespace gaxtapper

#endif
