#pragma once
#include "driver.hpp"
#include "parser.hpp"

namespace SysY {
  std::shared_ptr<AST::CompUnit> parse(Driver &driver,
                                       bool trace = false);
}
