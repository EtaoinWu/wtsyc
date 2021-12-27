#pragma once
#include "eeyore.hpp"
#include <memory>

namespace SysY {
  namespace AST {
    class CompUnit;
  }
  namespace Pass {
    Eeyore::Program generate_eeyore(std::shared_ptr<AST::CompUnit> cu);
  }
} // namespace SysY