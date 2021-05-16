// Gaxtapper: Automated GSF ripper for GAX Sound Engine.

#ifndef GAXTAPPER_GAX_MINIGSF_DRIVER_PARAM_HPP_
#define GAXTAPPER_GAX_MINIGSF_DRIVER_PARAM_HPP_

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

  void set_address(agbptr_t address) noexcept { address_ = address; }

  void set_song(GaxSongParam song) noexcept { song_ = std::move(song); }

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
};

}  // namespace gaxtapper

#endif
