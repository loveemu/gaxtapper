// Gaxtapper: Automated GSF ripper for GAX Sound Engine.

#include "gax_sound_handler_v2.hpp"

#include "bytes.hpp"
#include "types.hpp"

namespace gaxtapper {

std::optional<GaxSoundHandlerV2> GaxSoundHandlerV2::TryParse(
    std::string_view rom, std::string_view::size_type offset) {
  if (offset + 4 >= rom.size()) return std::nullopt;
  const agbptr_t init_handler = ReadInt32L(&rom[offset]);
  if (init_handler == 0) {
    const agbptr_t address = to_romptr(static_cast<agbsize_t>(offset));
    return GaxSoundHandlerV2{address};
  }
  if (!is_romptr(init_handler))
    return std::nullopt;

  if (offset + 0x1c >= rom.size()) return std::nullopt;
  const agbptr_t unknown_handler = ReadInt32L(&rom[offset + 4]);
  const agbptr_t play_handler = ReadInt32L(&rom[offset + 8]);
  if (!is_romptr(unknown_handler) || !is_romptr(play_handler))
    return std::nullopt;

  const agbptr_t data_address = ReadInt32L(&rom[offset + 0x18]);
  if (!is_romptr(data_address)) return std::nullopt;
  const agbsize_t data_offset = to_offset(data_address);
  if (data_offset >= rom.size()) return std::nullopt;

  const uint32_t num_linked_handlers = ReadInt32L(&rom[offset + 0xc]);
  const agbptr_t linked_handlers_address = ReadInt32L(&rom[offset + 0x10]);
  if (num_linked_handlers > 255) return std::nullopt;

  std::vector<GaxSoundHandlerV2> linked_handlers;
  if (num_linked_handlers != 0) {
    if (!is_romptr(linked_handlers_address)) return std::nullopt;

    const agbsize_t linked_handlers_offset = to_offset(linked_handlers_address);
    if (linked_handlers_offset + num_linked_handlers * 4 >= rom.size())
      return std::nullopt;

    linked_handlers.reserve(num_linked_handlers);
    for (uint32_t i = 0; i < num_linked_handlers; i++) {
      const agbptr_t address = ReadInt32L(&rom[linked_handlers_offset + i * 4]);
      const auto maybe_handler = TryParse(rom, to_offset(address));
      if (!maybe_handler.has_value()) return std::nullopt;
      linked_handlers.push_back(*maybe_handler);
    }
  }

  GaxSoundHandlerV2 handler;
  handler.set_address(to_romptr(static_cast<agbsize_t>(offset)));
  handler.set_linked_handlers(std::move(linked_handlers));
  handler.set_data_address(data_address);
  return std::make_optional(handler);
}

}  // namespace gaxtapper
