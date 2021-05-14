// Gaxtapper: Automated GSF ripper for GAX Sound Engine.

#ifndef GAXTAPPER_GAX_MINIGSF_DRIVER_PARAM_HPP_
#define GAXTAPPER_GAX_MINIGSF_DRIVER_PARAM_HPP_

#include <array>
#include <string>
#include "tabulate.hpp"
#include "types.hpp"

namespace gaxtapper {

class GaxMinigsfDriverParam {
 public:
  GaxMinigsfDriverParam() = default;

  GaxMinigsfDriverParam(agbptr_t address, agbptr_t song)
      : address_(address), song_(song) {}

  [[nodiscard]] bool ok() const noexcept {
    return address_ != agbnullptr && song_ != agbnullptr;
  }

  [[nodiscard]] agbptr_t address() const noexcept { return address_; }

  [[nodiscard]] agbptr_t song() const noexcept { return song_; }

  void set_address(agbptr_t address) noexcept { address_ = address; }

  void set_song(agbptr_t song) noexcept { song_ = song; }

  std::ostream& WriteAsTable(std::ostream& stream) const {
    using row_t = std::array<std::string, 2>;
    const row_t header{"Name", "Address / Value"};
    std::array items{
        row_t{"minigsf address", to_string(this->address())},
        row_t{"song", to_string(this->song())},
    };

    tabulate(stream, header, items);
    return stream;
  }

 private:
  agbptr_t address_ = agbnullptr;
  agbptr_t song_ = agbnullptr;
};

}  // namespace gaxtapper

#endif
