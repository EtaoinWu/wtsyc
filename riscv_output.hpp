#pragma once

#include "dashie.hpp"
#include "glimmy.hpp"

namespace SysY {
  namespace Glimmy {
    std::string to_riscv(const Dashie::Program &prog);
  }
}