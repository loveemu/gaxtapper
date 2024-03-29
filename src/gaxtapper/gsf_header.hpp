// Gaxtapper: Automated GSF ripper for GAX Sound Engine.

#ifndef GAXTAPPER_GSF_HEADER_HPP_
#define GAXTAPPER_GSF_HEADER_HPP_

#include <string>
#include "bytes.hpp"
#include "types.hpp"

namespace gaxtapper {

class GsfHeader {
 public:
  using size_type = agbsize_t;

  GsfHeader() : entrypoint_{0}, load_offset_{0}, load_size_{0} {};

  GsfHeader(agbptr_t entrypoint, agbptr_t load_offset, agbsize_t load_size)
      : str_{NewHeader(entrypoint, load_offset, load_size)},
        entrypoint_{entrypoint},
        load_offset_{load_offset},
        load_size_{load_size} {}

  [[nodiscard]] const char* data() const noexcept { return str_.data(); }
  [[nodiscard]] constexpr size_type size() const noexcept { return kSize; }
  [[nodiscard]] agbptr_t entrypoint() const noexcept { return entrypoint_; }
  [[nodiscard]] agbptr_t load_offset() const noexcept { return load_offset_; }
  [[nodiscard]] agbptr_t load_size() const noexcept { return load_size_; }

 private:
  static constexpr size_type kSize = 12;

  std::string str_;
  agbptr_t entrypoint_;
  agbptr_t load_offset_;
  agbsize_t load_size_;

  static std::string NewHeader(agbptr_t entrypoint, agbptr_t load_offset,
                               agbsize_t load_size) {
    std::string header(kSize, 0);
    WriteInt32L(&header[0], entrypoint);
    WriteInt32L(&header[4], load_offset);
    WriteInt32L(&header[8], load_size);
    return header;
  }
};

}  // namespace gaxtapper

#endif
