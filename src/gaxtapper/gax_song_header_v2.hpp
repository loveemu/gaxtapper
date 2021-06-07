// Gaxtapper: Automated GSF ripper for GAX Sound Engine.

#ifndef GAXTAPPER_GAX_SONG_HEADER_V2_HPP_
#define GAXTAPPER_GAX_SONG_HEADER_V2_HPP_

#include <optional>

#include "types.hpp"

namespace gaxtapper {

class GaxSongHeaderV2 {
 public:
  GaxSongHeaderV2() = default;

  constexpr operator bool() const {
    return address_ != agbnullptr;
  }

  [[nodiscard]] agbptr_t address() const noexcept { return address_; }

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

  [[nodiscard]] std::uint8_t num_fx_voices() const noexcept {
    return num_fx_voices_;
  }

  void set_address(agbptr_t address) noexcept { address_ = address; }

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

  void set_num_fx_voices(std::uint8_t num_fx_voices) noexcept {
    num_fx_voices_ = num_fx_voices;
  }

  static std::optional<GaxSongHeaderV2> TryParse(
      std::string_view rom, std::string_view::size_type offset);

 private:
  agbptr_t address_ = agbnullptr;
  std::uint16_t num_channels_ = 0;
  std::uint16_t num_rows_per_pattern_ = 0;
  std::uint16_t num_patterns_per_channel_ = 0;
  std::uint16_t loop_point_ = 0;
  std::uint16_t volume_ = 0;
  agbptr_t notes_address_ = agbnullptr;
  agbptr_t instrument_address_ = agbnullptr;
  agbptr_t sample_address_ = agbnullptr;
  std::uint16_t mixing_rate_ = 0xffff;
  std::uint8_t num_fx_voices_ = 0;
};

}  // namespace gaxtapper

#endif
