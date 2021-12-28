#pragma once
#include "pinkie.hpp"

namespace SysY {
  namespace Pinkie {
    namespace Pass {
      Pinkie::Prog optimize(Pinkie::Prog prog);
    }
  } // namespace Pinkie
  namespace Pass {
    using namespace SysY::Pinkie::Pass;
  }
} // namespace SysY