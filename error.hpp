#pragma once

#include <exception>
#include <stdexcept>
#include <string>


namespace SysY {
  void debug_log(std::string_view file, std::string_view func, int line, std::string_view msg);
}

#ifdef NDEBUG
// We are in release
#define DEBUG_LOG(s) do; while(0)
#else
// We are in debug
#define DEBUG_LOG(s) do SysY::debug_log(__FILE__, __func__, __LINE__, s); while(0)
#endif

namespace SysY::Exception {
  class VerificationError : public std::runtime_error {
    using std::runtime_error::runtime_error;
  };

  class BadAST : public std::runtime_error {
    using std::runtime_error::runtime_error;
  };

  class BadName : public std::runtime_error {
    using std::runtime_error::runtime_error;
  };

  class BadOP : public std::runtime_error {
    using std::runtime_error::runtime_error;
  };
}