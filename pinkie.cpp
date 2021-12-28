#include "pinkie.hpp"

namespace SysY::Pinkie {
  int UnitOpr::op_count(Unit::Type t) {
    static int op_count_data[Unit::n_unit_type] = {
      [Unit::unary] = 2, [Unit::binary] = 3, [Unit::copy] = 2,
      [Unit::aw] = 3,    [Unit::ar] = 3,     [Unit::label] = 0,
      [Unit::jmp] = 0,   [Unit::jc] = 1,     [Unit::param] = 1,
      [Unit::calli] = 1, [Unit::callv] = 0,  [Unit::reti] = 1,
      [Unit::retv] = 0,
    };
    return op_count_data[t];
  }
} // namespace SysY::Pinkie