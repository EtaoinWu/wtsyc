#pragma once
#include "dashie.hpp"
#include "pinkie.hpp"

namespace SysY {
  namespace Pinkie {
    namespace Pass {
      Pinkie::Prog optimize(Pinkie::Prog prog);
    }
  } // namespace Pinkie
  namespace Dashie {
    namespace Pass {
      Dashie::Program peephole(Dashie::Program prog);
    }
  } // namespace Dashie
  namespace Pass {
    using namespace SysY::Pinkie::Pass;
    using namespace SysY::Dashie::Pass;
  }
} // namespace SysY