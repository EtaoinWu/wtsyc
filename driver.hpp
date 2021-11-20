#pragma once

#include <string>
#include <string_view>
#include <memory>
#include "lexer.hpp"
#include "semantics.hpp"

namespace SysY {
  class Driver {
    std::string_view source;

  public:
    Lexer lexer;
    AST::pointer<AST::CompUnit> unit;
    Driver(std::string_view source_)
        : source{source_}, lexer{source_},
          unit(std::make_unique<AST::CompUnit>()) {}
  };
} // namespace SysY
