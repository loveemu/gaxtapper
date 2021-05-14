// Gaxtapper: Automated GSF ripper for GAX Sound Engine.

#ifndef GAXTAPPER_GAX_SONG_PARAM_HPP_
#define GAXTAPPER_GAX_SONG_PARAM_HPP_

#include <string>
#include <utility>
#include "types.hpp"

namespace gaxtapper {

class GaxSongParam {
 public:
  GaxSongParam() = default;

  GaxSongParam(agbptr_t address, std::string name, std::string copyright)
      : address_(address),
        name_(std::move(name)),
        copyright_(std::move(copyright)) {}

  [[nodiscard]] bool ok() const noexcept {
    return address_ != agbnullptr;
  }

  [[nodiscard]] agbptr_t address() const noexcept { return address_; }

  [[nodiscard]] std::string name() const noexcept { return name_; }

  [[nodiscard]] std::string copyright() const noexcept { return copyright_; }

  void set_address(agbptr_t address) noexcept { address_ = address; }

  void set_name(std::string name) noexcept { name_ = std::move(name); }

  void set_copyright(std::string copyright) noexcept {
    copyright_ = std::move(copyright);
  }

 private:
  agbptr_t address_ = agbnullptr;
  std::string name_;
  std::string copyright_;
};

}  // namespace gaxtapper

#endif
