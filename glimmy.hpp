#pragma once

#include "boxed.hpp"
#include "error.hpp"
#include "primitive.hpp"
#include "util.hpp"
#include <vector>

namespace SysY {
  namespace Glimmy {
    inline bool is_int12(int x) { return -(1 << 11) <= x && x < (1 << 11); }
    // clang-format off
    enum Reg : short {
      x0,  
      s0, s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, 
      t0, t1, t2, t3, t4, t5, t6,
      a0, a1, a2, a3, a4, a5, a6, a7,
      sp, ra, imm
    };
    enum RRR : short {
      add = 0, sub, mul, div, rem, 
      slt, sgt, xoor, n_rrr
    };
    enum RRI : short {
      addi = 0x100, slti,
      sw, lw, n_rri
    };
    enum RI : short {
      li = 0x200, n_ri
    };
    enum RR : short {
      mv = 0x300, neg, seqz, snez, n_rr
    };
    enum Branch : short {
      j = 0x400, blt, bgt, ble, bge, bne, beq, n_branch
    };
    enum Misc : short {
      call = 0x500, ret, la, label, n_misc
    };
    enum Internal : short {
      _add = 0, _sub, _mul, _div, _rem, 
      _slt, _sgt, _xoor, 
      _addi = 0x100, _slti,
      _sw, _lw,
      _li = 0x200, 
      _mv = 0x300, _neg, _seqz, _snez,
      _j = 0x400, _blt, _bgt, _ble, _bge, _bne, _beq,
      _call = 0x500, _ret, _la, _label
    };

    const char *toString(Reg e);
    // clang-format on

    RR toRiscVOP(UnaryOp t);
    std::pair<RRR, RR> toRiscVOP(BinaryOp t);
    Branch toRiscVBranch(BinaryOp t);

    const char *toString(Internal e);
    struct Unit {
      enum Mode : short { rrr, rri, ri, rr, branch, misc };
      Internal code;
      Reg reg1;
      Reg reg2;
      Reg reg3;
      union {
        int imm;
        int label_id;
        int global_id;
        gc_str name;
      };
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wenum-compare"
      Mode mode() const {
        if (code <= n_rrr) {
          return rrr;
        } else if (code <= n_rri) {
          return rri;
        } else if (code <= n_ri) {
          return ri;
        } else if (code <= n_rr) {
          return rr;
        } else if (code <= n_branch) {
          return branch;
        } else if (code <= n_misc) {
          return misc;
        } else {
          C_ERROR("control reaches the end");
        }
      }
#pragma GCC diagnostic pop
    };

    std::string toString(const Unit &x);

    struct Function {
      gc_str name;
      int STK;
      std::vector<Unit> code;
    };
  } // namespace Glimmy
} // namespace SysY