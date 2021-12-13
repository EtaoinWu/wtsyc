#pragma once

#include <exception>
#include <stdexcept>

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