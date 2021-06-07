// Gaxtapper: Automated GSF ripper for GAX Sound Engine.

#ifndef GAXTAPPER_GAX_MINIGSF_DRIVER_PARAM_HPP_
#define GAXTAPPER_GAX_MINIGSF_DRIVER_PARAM_HPP_

#include <algorithm>
#include <optional>
#include <string>
#include <utility>

#include "gax_song_param.hpp"
#include "tabulate.hpp"
#include "types.hpp"

namespace gaxtapper {

class GaxMinigsfDriverParam {
 public:
  GaxMinigsfDriverParam() = default;

  GaxMinigsfDriverParam(agbptr_t address, GaxSongParam song)
      : address_(address), song_(std::move(song)) {}

  [[nodiscard]] bool ok() const noexcept {
    return address_ != agbnullptr && song_.address() != agbnullptr;
  }

  [[nodiscard]] agbptr_t address() const noexcept { return address_; }

  [[nodiscard]] GaxSongParam song() const noexcept { return song_; }

  [[nodiscard]] std::optional<GaxSongParam> fx() const noexcept { return fx_; }

  [[nodiscard]] std::uint16_t fxid() const noexcept { return fxid_; }

  [[nodiscard]] std::uint16_t flags() const noexcept { return flags_; }

  [[nodiscard]] std::uint16_t mixing_rate() const noexcept { return mixing_rate_; }

  [[nodiscard]] std::uint16_t volume() const noexcept { return volume_; }

  void set_address(agbptr_t address) noexcept { address_ = address; }

  void set_song(GaxSongParam song) noexcept { song_ = std::move(song); }

  void set_fx(std::optional<GaxSongParam> fx) noexcept { fx_ = std::move(fx); }

  void set_fxid(std::uint16_t fxid) noexcept { fxid_ = fxid; }

  void set_flags(std::uint16_t flags) noexcept { flags_ = flags; }

  void set_mixing_rate(std::uint16_t mixing_rate) noexcept {
    mixing_rate_ = mixing_rate;
  }

  void set_volume(std::uint16_t volume) noexcept { volume_ = volume; }

  std::ostream& WriteAsTable(std::ostream& stream) const {
    using row_t = std::vector<std::string>;
    const row_t header{"Name", "Address / Value"};
    const std::vector items{
        row_t{"minigsf address", to_string(this->address())},
        row_t{"song address", to_string(this->song().address())},
    };

    tabulate(stream, header, items);
    return stream;
  }

 private:
  agbptr_t address_ = agbnullptr;
  GaxSongParam song_;
  std::optional<GaxSongParam> fx_;
  std::uint16_t fxid_ = 0xffff;
  std::uint16_t flags_ = 0;
  std::uint16_t mixing_rate_ = 0xffff;
  std::uint16_t volume_ = 0xffff;
};

}  // namespace gaxtapper

#endif
