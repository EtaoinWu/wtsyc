#include "location.hpp"
#include <fmt/core.h>

namespace SysY {
  std::string Position::toString() const {
    return fmt::format("{}:{}", line, character);
  }

  std::string Range::toString() const {
    return fmt::format("Range(begin={},end={})", begin, end);
  }
} // namespace SysY