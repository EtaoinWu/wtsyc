#pragma once

#include "fmt/core.h"
#include <iostream>
#include <string_view>
#include <cstdint>

namespace SysY {
  // 1-indexed offsets
  struct Position {
    uint32_t line;
    uint32_t character;
  };

  // range in a file, 0-indexed characters, clopen interval
  struct Range {
    static constexpr uint32_t invalid_offset = UINT32_MAX;
    uint32_t start, end;
    Range(): start{invalid_offset}, end{invalid_offset} {}
    Range(uint32_t start_, uint32_t end_): start{start_}, end{end_} {}
    bool exists() const {
      return start != invalid_offset && end != invalid_offset;
    }
    std::string toString() const {
      return fmt::format("Range(start={},end={})", start, end);
    }
  };

  struct Location {
    std::string_view source;
    Range range;
  };
}

inline std::ostream &operator<<(std::ostream &os, const SysY::Range r)  {
  return os << r.toString();
}
