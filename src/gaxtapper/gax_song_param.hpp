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

  GaxSongParam(agbptr_t address, std::string name = "", std::string artist = "")
      : address_(address), name_(std::move(name)), artist_(std::move(artist)) {}

  [[nodiscard]] bool ok() const noexcept {
    return address_ != agbnullptr;
  }

  [[nodiscard]] agbptr_t address() const noexcept { return address_; }

  [[nodiscard]] std::string name() const noexcept { return name_; }

  [[nodiscard]] std::string artist() const noexcept { return artist_; }

  void set_address(agbptr_t address) noexcept { address_ = address; }

  void set_name(std::string name) noexcept { name_ = std::move(name); }

  void set_artist(std::string artist) noexcept { artist_ = std::move(artist); }

  static GaxSongParam Of(const GaxSongHeader& header) noexcept {
    return GaxSongParam{header.address(), header.parsed_name(), header.parsed_artist()};
  }

 private:
  agbptr_t address_ = agbnullptr;
  std::string name_;
  std::string artist_;
};

}  // namespace gaxtapper

#endif
