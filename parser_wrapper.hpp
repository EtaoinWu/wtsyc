#pragma once
#include "driver.hpp"
#include "parser.hpp"

namespace SysY {
  std::unique_ptr<ASTNode> parse(std::string_view source, bool trace = false);
}
