#pragma once

#include <cstdint>
#include <fmt/core.h>
#include <iostream>
#include <string_view>

namespace SysY {
  // 1-indexed offsets
  struct Position {
    ptrdiff_t line;
    ptrdiff_t character;
    std::string toString() const {
      return fmt::format("{}:{}", line, character);
    }
  };

  // range in a file, 0-indexed characters, clopen interval
  struct Range {
    static constexpr ptrdiff_t invalid_offset = UINT32_MAX;
    ptrdiff_t begin, end;
    Range() : begin{invalid_offset}, end{invalid_offset} {}
    Range(ptrdiff_t start_, ptrdiff_t end_) : begin{start_}, end{end_} {}
    bool exists() const {
      return begin != invalid_offset && end != invalid_offset;
    }
    std::string toString() const {
      return fmt::format("Range(begin={},end={})", begin, end);
    }
  };

  struct Location {
    std::string_view source;
    Range range;
  };
} // namespace SysY

inline std::ostream &operator<<(std::ostream &os, const SysY::Range r) {
  return os << r.toString();
}
