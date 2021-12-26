#pragma once

#include "lexer.hpp"
#include "semantics.hpp"
#include <memory>
#include <string>
#include <string_view>

namespace SysY {

  // Parser Driver
  // 
  // Interact with parser and provide necessary help
  class Driver {
    std::string_view source;

  public:
    Lexer lexer;
    
    // Output of the parse
    AST::pointer<AST::CompUnit> unit;

    Driver(std::string_view source_)
        : source{source_}, lexer{source_},
          unit(std::make_shared<AST::CompUnit>()) {}
  };
} // namespace SysY
