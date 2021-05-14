// Gaxtapper: Automated GSF ripper for GAX Sound Engine.

#ifndef GAXTAPPER_GAX_DRIVER_PARAM_HPP_
#define GAXTAPPER_GAX_DRIVER_PARAM_HPP_

#include <array>
#include <string>
#include "tabulate.hpp"
#include "types.hpp"

namespace gaxtapper {

class GaxDriverParam {
 public:
  GaxDriverParam() = default;

  [[nodiscard]] bool ok() const noexcept {
    return gax2_new_ != agbnullptr && gax2_init_ != agbnullptr &&
           gax_irq_ != agbnullptr && gax_play_ != agbnullptr && !songs_.empty();
  }

  [[nodiscard]] agbptr_t gax2_new() const noexcept { return gax2_new_; }

  [[nodiscard]] agbptr_t gax2_init() const noexcept { return gax2_init_; }

  [[nodiscard]] agbptr_t gax_irq() const noexcept { return gax_irq_; }

  [[nodiscard]] agbptr_t gax_play() const noexcept { return gax_play_; }

  [[nodiscard]] const std::vector<agbptr_t> & songs() const noexcept { return songs_; }

  void set_gax2_new(agbptr_t address) noexcept { gax2_new_ = address; }

  void set_gax2_init(agbptr_t address) noexcept { gax2_init_ = address; }

  void set_gax_irq(agbptr_t address) noexcept { gax_irq_ = address; }

  void set_gax_play(agbptr_t address) noexcept { gax_play_ = address; }

  void set_songs(std::vector<agbptr_t> songs) noexcept {
    songs_ = std::move(songs);
  }

  std::ostream& WriteAsTable(std::ostream& stream) const {
    using row_t = std::array<std::string, 2>;
    const row_t header{"Name", "Address / Value"};
    const std::array items{
        row_t{"gax2_new", to_string(this->gax2_new())},
        row_t{"gax2_init", to_string(this->gax2_init())},
        row_t{"gax_irq", to_string(this->gax_irq())},
        row_t{"gax_play", to_string(this->gax_play())},
        row_t{"len(songs)", std::to_string(this->songs().size())}
    };

    tabulate(stream, header, items);

    return stream;
  }

 private:
  agbptr_t gax2_init_ = agbnullptr;
  agbptr_t gax2_new_ = agbnullptr;
  agbptr_t gax_irq_ = agbnullptr;
  agbptr_t gax_play_ = agbnullptr;
  std::vector<agbptr_t> songs_;
};

}  // namespace gaxtapper

#endif
