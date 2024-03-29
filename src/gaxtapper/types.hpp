// Gaxtapper: Automated GSF ripper for GAX Sound Engine.

#ifndef GAXTAPPER_TYPES_HPP_
#define GAXTAPPER_TYPES_HPP_

#include <cstdint>
#include <sstream>

namespace gaxtapper {

using agbptr_t = std::uint32_t;

using agbsize_t = std::uint32_t;

static constexpr auto agbnullptr{static_cast<agbptr_t>(-1)};

static constexpr auto agbnpos{static_cast<agbsize_t>(-1)};

static constexpr bool is_ewramptr(agbptr_t addr) {
  return addr >= 0x2000000 && addr <= 0x203ffff;
}

static constexpr bool is_iwramptr(agbptr_t addr) {
  return addr >= 0x3000000 && addr <= 0x3007fff;
}

static constexpr bool is_romptr(agbptr_t addr) {
  return addr >= 0x8000000 && addr <= 0x9ffffff;
}

static constexpr agbsize_t to_offset(agbptr_t addr) { return addr & 0x1ffffff; }

static constexpr agbptr_t to_romptr(agbsize_t offset) {
  return 0x8000000 | (offset & 0x1ffffff);
}

static std::string to_string(agbptr_t addr) {
  if (addr == agbnullptr) {
    return "null";
  } else {
    std::stringstream sstream;
    sstream << std::showbase << std::hex << addr;
    return sstream.str();
  }
}

}  // namespace gaxtapper

#endif
