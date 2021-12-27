#include "parser_wrapper.hpp"

namespace SysY {
  std::shared_ptr<AST::CompUnit> parse(Driver &driver, bool trace) {
    Parser parser(driver);

    if (trace) {
#if YYDEBUG
      parser.set_debug_level(1);
#endif
    }

    auto error = parser.parse();

    if (error != 0) {
      return nullptr;
    }

    return std::move(driver.unit);
  }

  SysY::Parser::symbol_type yylex(SysY::Driver &driver) {
    return driver.lexer();
  }

  void
  SysY::Parser::error(const SysY::Range &l, const std::string &err_message) {
    auto begin = driver.lexer.toPosition(l.begin);
    auto end = driver.lexer.toPosition(l.end - 1);
    std::cerr << "Error: " << err_message << " at " << begin.toString()
              << " to " << end.toString() << "\n";
  }
  void insert_declaration(
    SysY::Driver &driver, AST::pointer<AST::CompileTimeDeclaration> ptr) {
    driver.unit->globals.emplace_back(move(ptr));
  }
  void insert_function(SysY::Driver &driver, AST::pointer<AST::Function> ptr) {
    driver.unit->functions.emplace_back(move(ptr));
  }
} // namespace SysY
