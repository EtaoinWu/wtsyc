#include "parser_wrapper.hpp"

namespace SysY {
  std::unique_ptr<ASTNode> parse(std::string_view source, bool trace) {
    Driver driver(source);
    Parser parser(driver);

    if(trace) {
      parser.set_debug_level(1);
    }

    auto error = parser.parse();

    if (error != 0) {
      return nullptr;
    }

    return move(driver.result);
  }
}
