// Gaxtapper: Automated GSF ripper for GAX Sound Engine.

#ifndef GAXTAPPER_GAX_SONG_HEADER_V3_HPP_
#define GAXTAPPER_GAX_SONG_HEADER_V3_HPP_

#include <optional>
#include <utility>
#include <vector>

#include "gax_song_info_text.hpp"
#include "gax_version.hpp"
#include "types.hpp"

namespace gaxtapper {

class GaxSongHeaderV3 {
 public:
  GaxSongHeaderV3() = default;

  constexpr operator bool() const {
    return address_ != agbnullptr;
  }

  [[nodiscard]] agbptr_t address() const noexcept { return address_; }

  [[nodiscard]] const GaxSongInfoText& info() const noexcept { return info_; }

  [[nodiscard]] std::uint16_t num_channels() const noexcept {
    return num_channels_;
  }

  [[nodiscard]] std::uint16_t num_rows_per_pattern() const noexcept {
    return num_rows_per_pattern_;
  }

  [[nodiscard]] std::uint16_t num_patterns_per_channel() const noexcept {
    return num_patterns_per_channel_;
  }

  [[nodiscard]] std::uint16_t loop_point() const noexcept {
    return loop_point_;
  }

  [[nodiscard]] std::uint16_t volume() const noexcept { return volume_; }

  [[nodiscard]] agbptr_t notes_address() const noexcept { return notes_address_; }

  [[nodiscard]] agbptr_t instrument_address() const noexcept {
    return instrument_address_;
  }

  [[nodiscard]] agbptr_t sample_address() const noexcept {
    return sample_address_;
  }

  [[nodiscard]] std::uint16_t mixing_rate() const noexcept {
    return mixing_rate_;
  }

  [[nodiscard]] std::uint16_t fx_mixing_rate() const noexcept {
    return fx_mixing_rate_;
  }

  [[nodiscard]] std::uint8_t num_fx_voices() const noexcept {
    return num_fx_voices_;
  }

  [[nodiscard]] const std::vector<agbptr_t>& seq_of_channels() const noexcept {
    return seq_of_channels_;
  }

  void set_address(agbptr_t address) noexcept { address_ = address; }

  void set_info(GaxSongInfoText info) { info_ = std::move(info); }

  void set_num_channels(std::uint16_t num_channels) noexcept {
    num_channels_ = num_channels;
  }

  void set_num_rows_per_pattern(std::uint16_t num_rows_per_pattern) noexcept {
    num_rows_per_pattern_ = num_rows_per_pattern;
  }

  void set_num_patterns_per_channel(
      std::uint16_t num_patterns_per_channel) noexcept {
    num_patterns_per_channel_ = num_patterns_per_channel;
  }

  void set_loop_point(std::uint16_t loop_point) noexcept {
    loop_point_ = loop_point;
  }

  void set_volume(std::uint16_t volume) noexcept { volume_ = volume; }

  void set_notes_address(agbptr_t notes_address) noexcept {
    notes_address_ = notes_address;
  }

  void set_instrument_address(agbptr_t instrument_address) noexcept {
    instrument_address_ = instrument_address;
  }

  void set_sample_address(agbptr_t sample_address) noexcept {
    sample_address_ = sample_address;
  }

  void set_mixing_rate(std::uint16_t mixing_rate) noexcept {
    mixing_rate_ = mixing_rate;
  }

  void set_fx_mixing_rate(std::uint16_t fx_mixing_rate) noexcept {
    fx_mixing_rate_ = fx_mixing_rate;
  }

  void set_num_fx_voices(std::uint8_t num_fx_voices) noexcept {
    num_fx_voices_ = num_fx_voices;
  }

  void set_seq_of_channels(std::vector<agbptr_t> seq_of_channels) noexcept {
    seq_of_channels_ = std::move(seq_of_channels);
  }

  static std::optional<GaxSongHeaderV3> TryParse(
      std::string_view rom, std::string_view::size_type offset);

  static std::vector<GaxSongHeaderV3> Scan(
      std::string_view rom, std::string_view::size_type offset = 0);

 private:
  [[nodiscard]] GaxSongInfoText TryFindInfoText(std::string_view rom) const;

  agbptr_t address_ = agbnullptr;
  GaxSongInfoText info_;
  std::uint16_t num_channels_ = 0;
  std::uint16_t num_rows_per_pattern_ = 0;
  std::uint16_t num_patterns_per_channel_ = 0;
  std::uint16_t loop_point_ = 0;
  std::uint16_t volume_ = 0;
  agbptr_t notes_address_ = agbnullptr;
  agbptr_t instrument_address_ = agbnullptr;
  agbptr_t sample_address_ = agbnullptr;
  std::uint16_t mixing_rate_ = 0xffff;
  std::uint16_t fx_mixing_rate_ = 0xffff;
  std::uint8_t num_fx_voices_ = 0;
  std::vector<agbptr_t> seq_of_channels_;
};

}  // namespace gaxtapper

#endif
