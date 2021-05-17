// Gaxtapper: Automated GSF ripper for GAX Sound Engine.

#ifndef GAXTAPPER_TABULATE_HPP_
#define GAXTAPPER_TABULATE_HPP_

#include <algorithm>
#include <iomanip>
#include <vector>

namespace gaxtapper {

static std::ostream& tabulate(
    std::ostream& stream, const std::vector<std::string>& header,
    const std::vector<std::vector<std::string>>& rows) {
  // Determine column lengths.
  //
  // Note that it doesn't calculate proper lengths for non-ASCII characters.
  std::vector<size_t> maxlength;
  for (auto cell = header.begin(); cell != header.end(); ++cell) {
    maxlength.push_back(cell->size());
  }
  for (const auto& row : rows) {
    for (auto [i, cell] = std::tuple{static_cast<size_t>(0), row.begin()};
         cell != row.end(); ++i, ++cell) {
      if (i >= maxlength.size())
        maxlength.push_back(cell->size());
      else
        maxlength[i] = std::max(cell->size(), maxlength[i]);
    }
  }

  // Header
  stream << std::setfill(' ');
  for (size_t i = 0; i < header.size(); i++) {
    stream << "|" << std::setw(maxlength[i]) << std::left << header[i] << " ";
  }
  stream << "|" << std::endl;

  // Separator
  stream << std::setfill('-');
  for (size_t i = 0; i < header.size(); i++) {
    stream << "|" << std::setw(maxlength[i] + 1) << "";
  }
  stream << "|" << std::endl;
  stream << std::setfill(' ');

  // Columns
  for (const auto& row : rows) {
    for (size_t i = 0; i < row.size(); i++) {
      stream << "|" << std::setw(maxlength[i]) << std::left << row[i] << " ";
    }
    stream << "|" << std::endl;
  }

  return stream;
}

}  // namespace gaxtapper

#endif
