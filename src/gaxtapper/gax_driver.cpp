// Gaxtapper: Automated GSF ripper for GAX Sound Engine.

#include "gax_driver.hpp"

#include <cstring>
#include <sstream>
#include <string>
#include <string_view>
#include "arm.hpp"
#include "bytes.hpp"
#include "gax_driver_param.hpp"
#include "gax_minigsf_driver_param.hpp"
#include "types.hpp"

namespace gaxtapper {

GaxDriverParam GaxDriver::Inspect(std::string_view rom) {
  GaxDriverParam param;
  param.set_gax2_new(FindGax2New(rom));
  param.set_gax2_init(FindGax2Init(rom));
  param.set_gax_irq(FindGaxIrq(rom));
  param.set_gax_play(FindGaxPlay(rom));
  return param;
}

void GaxDriver::InstallGsfDriver(std::string& rom, agbptr_t address,
                                 const GaxDriverParam& param) {
  if (!is_romptr(address))
    throw std::invalid_argument("The gsf driver address is not valid.");
  if (!param.ok()) {
    std::ostringstream message;
    message << "Identification of GAX Sound Engine is incomplete."
            << std::endl << std::endl;
    (void)param.WriteAsTable(message);
    throw std::invalid_argument(message.str());
  }

  const agbsize_t offset = to_offset(address);
  if (offset + gsf_driver_size() > rom.size())
    throw std::out_of_range("The address of gsf driver block is out of range.");

  std::memcpy(&rom[offset], gsf_driver_block, gsf_driver_size());
  WriteInt32L(&rom[offset + kGax2NewOffset], param.gax2_new() | 1);
  WriteInt32L(&rom[offset + kGax2InitOffset], param.gax2_init() | 1);
  WriteInt32L(&rom[offset + kGaxIrqOffset], param.gax_irq() | 1);
  WriteInt32L(&rom[offset + kGaxPlayOffset], param.gax_play() | 1);

  WriteInt32L(rom.data(), make_arm_b(0x8000000, address));
}

std::string GaxDriver::NewMinigsfData(const GaxMinigsfDriverParam& param) {
  if (!param.ok()) {
    std::ostringstream message;
    message << "The parameters for creating minigsfs are not sufficient." << std::endl
            << std::endl;
    (void)param.WriteAsTable(message);
    throw std::invalid_argument(message.str());
  }

  char data[4];
  WriteInt32L(data, param.song());
  return std::string(data);
}

agbptr_t GaxDriver::FindGax2New(std::string_view rom) {
  using namespace std::literals::string_view_literals;
  const std::string_view pattern = {
      "\xf0\xb5\x47\x46\x80\xb4\x81\xb0\x06\x1c\x00\x2e\x08\xd1\x02\x48\x02\x49"sv};
  const auto offset = rom.find(pattern);
  return offset != std::string_view::npos
             ? to_romptr(static_cast<uint32_t>(offset))
             : agbnullptr;
}

agbptr_t GaxDriver::FindGax2Init(std::string_view rom) {
  using namespace std::literals::string_view_literals;
  const std::string_view pattern = {
      "\xf0\xb5\x57\x46\x4e\x46\x45\x46\xe0\xb4\x81\xb0\x07\x1c\x00\x26\x0e\x48\x39\x68\x01\x60"sv};
  const auto offset = rom.find(pattern);
  return offset != std::string_view::npos
             ? to_romptr(static_cast<uint32_t>(offset))
             : agbnullptr;
}

agbptr_t GaxDriver::FindGaxIrq(std::string_view rom) {
  using namespace std::literals::string_view_literals;
  const std::string_view pattern = {
      "\xf0\xb5\x3b\x48\x02\x68\x11\x68\x3a\x48\x81\x42\x6d\xd1\x50\x6d\x00\x28\x6a\xd0\x50\x6d\x01\x28\x1a\xd1\x02\x20\x50\x65\x36\x49"sv};
  const auto offset = rom.find(pattern);
  return offset != std::string_view::npos
             ? to_romptr(static_cast<uint32_t>(offset))
             : agbnullptr;
}

agbptr_t GaxDriver::FindGaxPlay(std::string_view rom) {
  using namespace std::literals::string_view_literals;
  const std::string_view pattern = {
      "\x70\xb5\x81\xb0\x47\x48\x01\x68\x48\x6d\x00\x28\x00\xd1"sv};
  const std::string_view::size_type offset = rom.find(pattern);
  return offset != std::string_view::npos
             ? to_romptr(static_cast<uint32_t>(offset))
             : agbnullptr;
}

}  // namespace gaxtapper
