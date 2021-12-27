#pragma once
#include "eeyore.hpp"
#include "semantics.hpp"

namespace SysY {
  namespace Pass {
    Eeyore::Program generate_eeyore(std::shared_ptr<AST::CompUnit> cu);
  }
} // namespace SysY