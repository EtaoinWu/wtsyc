// ragel_subtype=cpp

#include <string>
#include <vector>
#include <memory>
#include <cstdlib>
#include <cstring>
#include <climits>

#include "lexer.hpp"

using namespace std;

%%{
  machine rule;
  action line_count {
    line_end(p);
  }
  main := |*
    "\n"    => line_count;
    space;
    "/*" ( "\n" @line_count | any )* :>> "*/";
    "//" (any *) :>> "\n" @line_count;
    
    "["         => { return SysY::Parser::make_LBRACKET(tokenRange()); };
    "]"         => { return SysY::Parser::make_RBRACKET(tokenRange()); };
    "("         => { return SysY::Parser::make_LPAREN(tokenRange()); };
    ")"         => { return SysY::Parser::make_RPAREN(tokenRange()); };
    "{"         => { return SysY::Parser::make_LCURLY(tokenRange()); };
    "}"         => { return SysY::Parser::make_RCURLY(tokenRange()); };
    ","         => { return SysY::Parser::make_COMMA(tokenRange()); };
    "."         => { return SysY::Parser::make_PERIOD(tokenRange()); };
    ";"         => { return SysY::Parser::make_SEMICOLON(tokenRange()); };
    "="         => { return SysY::Parser::make_ASSIGN(tokenRange()); };
    "+"         => { return SysY::Parser::make_PLUS(tokenRange()); };
    "-"         => { return SysY::Parser::make_MINUS(tokenRange()); };
    "*"         => { return SysY::Parser::make_STAR(tokenRange()); };
    "/"         => { return SysY::Parser::make_SLASH(tokenRange()); };
    "%"         => { return SysY::Parser::make_MOD(tokenRange()); };
    ">"         => { return SysY::Parser::make_GT(tokenRange()); };
    ">="        => { return SysY::Parser::make_GE(tokenRange()); };
    "<"         => { return SysY::Parser::make_LT(tokenRange()); };
    "<="        => { return SysY::Parser::make_LE(tokenRange()); };
    "=="        => { return SysY::Parser::make_EQ(tokenRange()); };
    "!="        => { return SysY::Parser::make_NEQ(tokenRange()); };
    "!"         => { return SysY::Parser::make_LOGICAL_NOT(tokenRange()); };
    "&&"        => { return SysY::Parser::make_LOGICAL_AND(tokenRange()); };
    "||"        => { return SysY::Parser::make_LOGICAL_OR(tokenRange()); };
    "if"        => { return SysY::Parser::make_IF(tokenRange()); };
    "else"      => { return SysY::Parser::make_ELSE(tokenRange()); };
    "while"     => { return SysY::Parser::make_WHILE(tokenRange()); };
    "break"     => { return SysY::Parser::make_BREAK(tokenRange()); };
    "continue"  => { return SysY::Parser::make_CONTINUE(tokenRange()); };
    "return"    => { return SysY::Parser::make_RETURN(tokenRange()); };
    "const"     => { return SysY::Parser::make_CONST(tokenRange()); };
    "int"       => { return SysY::Parser::make_TYPE(PrimitiveType::INT, tokenRange()); };
    "void"      => { return SysY::Parser::make_TYPE(PrimitiveType::VOID, tokenRange()); };

    (('0' . [0-7]+) | (("0x" | "0X") xdigit+) | ([1-9] digit*) | "0")
      => { return make_INT_LITERAL(std::string(ts, te - ts), tokenRange()); };

    (alpha | "_") (alnum | "_") *
      => { return SysY::Parser::make_IDENT(std::string(ts, te - ts), tokenRange()); };
  *|;
}%%

namespace {
// Static, const tables of data.
%% write data;
}

namespace SysY {
  Parser::symbol_type make_INT_LITERAL(const std::string &val, Range r) {
    errno = 0;
    long n = strtol (val.c_str(), NULL, 0);
    // if (! (INT_MIN <= n && n <= INT_MAX && errno != ERANGE))
    //   throw Parser::syntax_error(r, "integer is out of range: " + val);
    return Parser::make_INT_LITERAL(static_cast<int>(n), r);
  }

  Lexer::Lexer(string_view source) : source(source), p(source.begin()), pe(source.end()), eof(source.end()),
    cs(rule_start), ts(source.begin()), te(source.begin()), act(0) {}

  Range Lexer::tokenRange() const {
    return {ts - source.begin(), te - source.begin()};
  }

  Parser::symbol_type Lexer::exec() {
    %% write exec;
    return Parser::make_EOF(tokenRange());
  }

  Parser::symbol_type Lexer::operator()() {
    auto result = this->exec();
    this->p++;
    return result;
  }

  void Lexer::line_end(const std::string_view::const_iterator &it) {
    lines.push_back(it - source.begin() + 1);
  }

  Position Lexer::toPosition(ptrdiff_t d) const {
    auto it = std::upper_bound(lines.begin(), lines.end(), d) - 1;
    return Position{it - lines.begin() + 1, d - *it + 1};
  }
}
