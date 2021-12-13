#pragma once

#include <string_view>
#include <set>
#include "parser.hpp"
#include "location.hpp"

namespace SysY {
  class Lexer
  {
  private:
    std::string_view source;

    // Ragel states
    // Pointer
    std::string_view::const_iterator p;
    // Pointer End
    const std::string_view::const_iterator pe;
    const std::string_view::const_iterator eof;
    // Current State
    int cs;
    // Token Start
    std::string_view::const_iterator ts;
    // Token End
    std::string_view::const_iterator te;
    // Acti
    int act;

    // Line tracing
    void line_end(const std::string_view::const_iterator &it);
  public:
    std::vector<ptrdiff_t> lines = {0};
    Lexer(std::string_view source_);
    Range tokenRange() const;
    Parser::symbol_type exec();
    Parser::symbol_type operator()();
    Position toPosition(ptrdiff_t d) const;
  };
}