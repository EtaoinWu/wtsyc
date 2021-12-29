#include "error.hpp"
#include <fmt/core.h>
#include <iostream>

namespace SysY {
  void debug_log(
    std::string_view file, std::string_view func, int line,
    std::string_view msg) {
#ifndef NDEBUG
    std::cerr << fmt::format("[{}:{}] {}(): {}\n", file, line, func, msg);
#endif
  }
  [[noreturn]] void c_error(
    std::string_view file, std::string_view func, int line,
    std::string_view msg) {
#ifndef NDEBUG
    std::cerr << fmt::format("[{}:{}] {}(): {}\n", file, line, func, msg);
    exit(-1);
#endif
  }
  [[noreturn]] void c_assert_fail(
    std::string_view file, std::string_view func, int line,
    std::string_view cond) {
    std::cerr << fmt::format(
      "[{}:{}] {}(): assertion {} failed\n", file, line, func, cond);
    exit(-1);
  }
} // namespace SysY