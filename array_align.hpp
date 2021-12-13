#pragma once

#include "semantics.hpp"

namespace SysY {
  namespace ArrayAlign {
    Result array_align(const AST::pointer<AST::ArrayLiteral> &exp, const SemanticType &type);
  }
}
