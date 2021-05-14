// Gaxtapper: Automated GSF ripper for GAX Sound Engine.

#ifndef GAXTAPPER_GAX_DRIVER_PARAM_HPP_
#define GAXTAPPER_GAX_DRIVER_PARAM_HPP_

#include <array>
#include <string>
#include <string_view>
#include <utility>

#include "gax_song_param.hpp"
#include "gax_version.hpp"
#include "tabulate.hpp"
#include "types.hpp"

namespace gaxtapper {

class GaxDriverParam {
 public:
  GaxDriverParam() = default;

  [[nodiscard]] bool ok() const noexcept {
    return version_ != kGaxUnknownVersion && gax2_new_ != agbnullptr &&
           gax2_init_ != agbnullptr && gax_irq_ != agbnullptr &&
           gax_play_ != agbnullptr && !songs_.empty();
  }

  [[nodiscard]] GaxVersion version() const noexcept { return version_; }

  [[nodiscard]] std::string version_text() const noexcept { return version_text_; }

  [[nodiscard]] agbptr_t gax2_new() const noexcept { return gax2_new_; }

  [[nodiscard]] agbptr_t gax2_init() const noexcept { return gax2_init_; }

  [[nodiscard]] agbptr_t gax_irq() const noexcept { return gax_irq_; }

  [[nodiscard]] agbptr_t gax_play() const noexcept { return gax_play_; }

  [[nodiscard]] const std::vector<GaxSongParam> & songs() const noexcept { return songs_; }

  void set_version(GaxVersion version) noexcept { version_ = version; }

  void set_version_text(std::string_view version_text) noexcept { version_text_ = version_text; }

  void set_gax2_new(agbptr_t address) noexcept { gax2_new_ = address; }

  void set_gax2_init(agbptr_t address) noexcept { gax2_init_ = address; }

  void set_gax_irq(agbptr_t address) noexcept { gax_irq_ = address; }

  void set_gax_play(agbptr_t address) noexcept { gax_play_ = address; }

  void set_songs(std::vector<GaxSongParam> songs) noexcept {
    songs_ = std::move(songs);
  }

  std::ostream& WriteAsTable(std::ostream& stream) const {
    using row_t = std::array<std::string, 2>;
    const row_t header{"Name", "Address / Value"};
    const std::array items{
        row_t{"version", to_string(this->version())},
        row_t{"version_text", this->version_text()},
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
  GaxVersion version_ = kGaxUnknownVersion;
  std::string version_text_;
  agbptr_t gax2_init_ = agbnullptr;
  agbptr_t gax2_new_ = agbnullptr;
  agbptr_t gax_irq_ = agbnullptr;
  agbptr_t gax_play_ = agbnullptr;
  std::vector<GaxSongParam> songs_;
};

}  // namespace gaxtapper

#endif
