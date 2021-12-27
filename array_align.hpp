#pragma once

#include "semantics.hpp"

namespace SysY {
  namespace ArrayAlign {
    Result array_align(const AST::pointer<AST::ArrayLiteral> &exp, const SemanticType &type);
    Result array_align_expand(const AST::pointer<AST::ArrayLiteral> &exp, const SemanticType &type);
  }
}
