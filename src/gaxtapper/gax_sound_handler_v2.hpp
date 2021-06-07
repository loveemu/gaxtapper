// Gaxtapper: Automated GSF ripper for GAX Sound Engine.

#ifndef GAXTAPPER_GAX_SOUND_HANDLER_V2_HPP_
#define GAXTAPPER_GAX_SOUND_HANDLER_V2_HPP_

#include <optional>
#include <vector>

#include "types.hpp"

namespace gaxtapper {

class GaxSoundHandlerV2 {
 public:
  GaxSoundHandlerV2() = default;
  GaxSoundHandlerV2(agbptr_t address) : address_(address) {}

  constexpr operator bool() const {
    return address_ != agbnullptr;
  }

  [[nodiscard]] agbptr_t address() const noexcept { return address_; }

  [[nodiscard]] const std::vector<GaxSoundHandlerV2>& linked_handlers()
      const noexcept {
    return linked_handlers_;
  }

  [[nodiscard]] agbptr_t data_address() const noexcept { return data_address_; }

  void set_address(agbptr_t address) noexcept { address_ = address; }

  void set_linked_handlers(
      std::vector<GaxSoundHandlerV2> linked_handlers) noexcept {
    linked_handlers_ = std::move(linked_handlers);
  }

  void set_data_address(agbptr_t data_address) noexcept {
    data_address_ = data_address;
  }

  static std::optional<GaxSoundHandlerV2> TryParse(
      std::string_view rom, std::string_view::size_type offset);

 private:
  agbptr_t address_ = agbnullptr;
  std::vector<GaxSoundHandlerV2> linked_handlers_;
  agbptr_t data_address_ = agbnullptr;
};

}  // namespace gaxtapper

#endif
