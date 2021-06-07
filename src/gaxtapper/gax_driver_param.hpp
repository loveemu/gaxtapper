// Gaxtapper: Automated GSF ripper for GAX Sound Engine.

#ifndef GAXTAPPER_GAX_DRIVER_PARAM_HPP_
#define GAXTAPPER_GAX_DRIVER_PARAM_HPP_

#include <string>
#include <utility>

#include "gax_music_entry.hpp"
#include "gax_version.hpp"
#include "tabulate.hpp"
#include "types.hpp"

namespace gaxtapper {

class GaxDriverParam {
 public:
  GaxDriverParam() = default;

  [[nodiscard]] bool ok() const noexcept {
    return version_ && gax2_estimate_ != agbnullptr &&
           gax2_new_ != agbnullptr && gax2_init_ != agbnullptr &&
           gax_irq_ != agbnullptr && gax_play_ != agbnullptr && !songs_.empty();
  }

  [[nodiscard]] GaxVersion version() const noexcept { return version_; }

  [[nodiscard]] std::string version_text() const noexcept { return version_text_; }

  [[nodiscard]] agbptr_t gax2_estimate() const noexcept { return gax2_estimate_; }

  [[nodiscard]] agbptr_t gax2_new() const noexcept { return gax2_new_; }

  [[nodiscard]] agbptr_t gax2_init() const noexcept { return gax2_init_; }

  [[nodiscard]] agbptr_t gax_irq() const noexcept { return gax_irq_; }

  [[nodiscard]] agbptr_t gax_play() const noexcept { return gax_play_; }

  [[nodiscard]] agbptr_t gax_wram_pointer() const noexcept { return gax_wram_pointer_; }

  [[nodiscard]] const std::vector<GaxMusicEntry> & songs() const noexcept { return songs_; }

  [[nodiscard]] GaxMusicEntry fx() const noexcept { return fx_; }

  void set_version(GaxVersion version) noexcept { version_ = version; }

  void set_version_text(std::string version_text) noexcept {
    version_text_ = std::move(version_text);
  }

  void set_gax2_estimate(agbptr_t address) noexcept { gax2_estimate_ = address; }

  void set_gax2_new(agbptr_t address) noexcept { gax2_new_ = address; }

  void set_gax2_init(agbptr_t address) noexcept { gax2_init_ = address; }

  void set_gax_irq(agbptr_t address) noexcept { gax_irq_ = address; }

  void set_gax_play(agbptr_t address) noexcept { gax_play_ = address; }

  void set_gax_wram_pointer(agbptr_t address) noexcept { gax_wram_pointer_ = address; }

  void set_songs(std::vector<GaxMusicEntry> songs) {
    songs_ = std::move(songs);

    fx_ = GaxMusicEntry{};
    for (const auto & song : songs_) {
      if (song.num_channels() == 0) {
        fx_ = song;
        break;
      }
    }
  }

  std::ostream& WriteAsTable(std::ostream& stream) const {
    using row_t = std::vector<std::string>;
    const row_t header{"Name", "Address / Value"};
    const std::vector items{
        row_t{"major_version", std::to_string(this->version().major_version())},
        row_t{"minor_version", std::to_string(this->version().minor_version())},
        row_t{"version_text", this->version_text()},
        row_t{"gax2_estimate", to_string(this->gax2_estimate())},
        row_t{"gax2_new", to_string(this->gax2_new())},
        row_t{"gax2_init", to_string(this->gax2_init())},
        row_t{"gax_irq", to_string(this->gax_irq())},
        row_t{"gax_play", to_string(this->gax_play())},
        row_t{"wram_pointer", to_string(this->gax_wram_pointer())},
        row_t{"len(songs)", std::to_string(this->songs().size())},
        row_t{"fx", to_string(this->fx() ? this->fx().address() : agbnullptr)}
    };

    tabulate(stream, header, items);

    return stream;
  }

 private:
  GaxVersion version_;
  std::string version_text_;
  agbptr_t gax2_estimate_ = agbnullptr;
  agbptr_t gax2_init_ = agbnullptr;
  agbptr_t gax2_new_ = agbnullptr;
  agbptr_t gax_irq_ = agbnullptr;
  agbptr_t gax_play_ = agbnullptr;
  agbptr_t gax_wram_pointer_ = agbnullptr;
  std::vector<GaxMusicEntry> songs_;
  GaxMusicEntry fx_;
};

}  // namespace gaxtapper

#endif
