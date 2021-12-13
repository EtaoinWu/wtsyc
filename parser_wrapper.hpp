#pragma once
#include "driver.hpp"
#include "parser.hpp"

namespace SysY {
  std::shared_ptr<AST::CompUnit> parse(std::string_view source, bool trace = false);
}
