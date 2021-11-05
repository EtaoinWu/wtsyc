#pragma once

#include <string_view>
#include "parser.hpp"
#include "location.hpp"

namespace SysY {
  class Lexer
  {
  private:
    std::string_view source;

    // Ragel states
    std::string_view::const_iterator p;
    const std::string_view::const_iterator pe;
    const std::string_view::const_iterator eof;
    int cs;
    std::string_view::const_iterator ts;
    std::string_view::const_iterator te;
    int act;
  public:
    Lexer(std::string_view source_);
    Range tokenRange() const;
    Parser::symbol_type exec();
    Parser::symbol_type operator()();
  };
}