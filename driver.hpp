#pragma once

#include <string>
#include <string_view>
#include <memory>
#include "lexer.hpp"
#include "semantics.hpp"

namespace SysY
{
  class Driver {
    std::string_view source;
  public:
    Lexer lexer;
    std::unique_ptr<ASTNode> result;
    Driver(std::string_view source_): source{source_}, lexer{source_} {}
  };
} // namespace SysY
