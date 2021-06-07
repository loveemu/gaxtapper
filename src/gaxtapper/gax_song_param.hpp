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

  GaxSongParam(agbptr_t address, GaxSongInfoText info)
      : address_(address), info_(std::move(info)) {}

  [[nodiscard]] bool ok() const noexcept {
    return address_ != agbnullptr;
  }

  [[nodiscard]] agbptr_t address() const noexcept { return address_; }

  [[nodiscard]] const GaxSongInfoText& info() const noexcept { return info_; }

  void set_address(agbptr_t address) noexcept { address_ = address; }

  void set_info(GaxSongInfoText info) noexcept { info_ = std::move(info); }

  static GaxSongParam Of(const GaxMusicEntry& header) noexcept {
    return GaxSongParam{header.address(), header.info()};
  }

 private:
  agbptr_t address_ = agbnullptr;
  GaxSongInfoText info_;
};

}  // namespace gaxtapper

#endif
