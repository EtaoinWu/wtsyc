#include "glimmy.hpp"
#include <fmt/core.h>

namespace SysY {
  namespace Glimmy {
    // clang-format off
    const char *toString(Reg e) {
      switch (e) {
      case x0: return "x0";
      case s0: return "s0";
      case s1: return "s1";
      case s2: return "s2";
      case s3: return "s3";
      case s4: return "s4";
      case s5: return "s5";
      case s6: return "s6";
      case s7: return "s7";
      case s8: return "s8";
      case s9: return "s9";
      case s10: return "s10";
      case s11: return "s11";
      case t0: return "t0";
      case t1: return "t1";
      case t2: return "t2";
      case t3: return "t3";
      case t4: return "t4";
      case t5: return "t5";
      case t6: return "t6";
      case a0: return "a0";
      case a1: return "a1";
      case a2: return "a2";
      case a3: return "a3";
      case a4: return "a4";
      case a5: return "a5";
      case a6: return "a6";
      case a7: return "a7";
      case sp: return "sp";
      case ra: return "ra";
      default: C_ERROR("bad Glimmy register");
      }
    }
    // clang-format on
    const char *toString(Internal e) {
      switch (e) {
      case _add:
        return "add";
      case _sub:
        return "sub";
      case _mul:
        return "mul";
      case _div:
        return "div";
      case _rem:
        return "rem";
      case _slt:
        return "slt";
      case _sgt:
        return "sgt";
      case _xoor:
        return "xor";
      case _addi:
        return "addi";
      case _slti:
        return "slti";
      case _sw:
        return "sw";
      case _lw:
        return "lw";
      case _li:
        return "li";
      case _mv:
        return "mv";
      case _neg:
        return "neg";
      case _seqz:
        return "seqz";
      case _snez:
        return "snez";
      case _j:
        return "j";
      case _blt:
        return "blt";
      case _bgt:
        return "bgt";
      case _ble:
        return "ble";
      case _bge:
        return "bge";
      case _bne:
        return "bne";
      case _beq:
        return "beq";
      case _call:
        return "call";
      case _ret:
        return "ret";
      case _la:
        return "la";
      case _label:
        return "label";
      default:
        C_ERROR("bad Internal");
      }
    }

    RR toRiscVOP(UnaryOp t) {
      switch (t) {
      case UnaryOp::Plus:
        return mv;
      case UnaryOp::Minus:
        return neg;
      case UnaryOp::Not:
        return seqz;
      default:
        C_ERROR("bad risc-v BinaryOp");
      }
    }

    std::pair<RRR, RR> toRiscVOP(BinaryOp t) {
      switch (t) {
      case BinaryOp::Plus:
        return {add, n_rr};
      case BinaryOp::Minus:
        return {sub, n_rr};
      case BinaryOp::Mult:
        return {mul, n_rr};
      case BinaryOp::Div:
        return {div, n_rr};
      case BinaryOp::Mod:
        return {rem, n_rr};
      case BinaryOp::LT:
        return {slt, n_rr};
      case BinaryOp::GT:
        return {sgt, n_rr};
      case BinaryOp::LE:
        return {sgt, seqz};
      case BinaryOp::GE:
        return {slt, seqz};
      case BinaryOp::NE:
        return {xoor, snez};
      case BinaryOp::EQ:
        return {xoor, seqz};
      case BinaryOp::And:
      case BinaryOp::Or:
      default:
        C_ERROR("bad risc-v BinaryOp");
      }
    }

    Branch toRiscVBranch(BinaryOp t) {
      switch (t) {
      case BinaryOp::LT:
        return blt;
      case BinaryOp::GT:
        return bgt;
      case BinaryOp::LE:
        return ble;
      case BinaryOp::GE:
        return bge;
      case BinaryOp::NE:
        return bne;
      case BinaryOp::EQ:
        return beq;
      default:
        C_ERROR("bad risc-v branch");
      }
    }

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wenum-compare"
    std::string toString(const Unit &x) {
      switch (x.mode()) {
      case Unit::rrr:
        return fmt::format(
          "{} {}, {}, {}", toString(x.code), toString(x.reg1), toString(x.reg2),
          toString(x.reg3));
      case Unit::rri:
        if (x.code == addi || x.code == slti)
          return fmt::format(
            "{} {}, {}, {}", toString(x.code), toString(x.reg1),
            toString(x.reg2), x.imm);
        else if (x.code == lw) {
          return fmt::format(
            "{} {}, {}({})", toString(x.code), toString(x.reg1), x.imm,
            toString(x.reg2));
        } else {
          return fmt::format(
            "{} {}, {}({})", toString(x.code), toString(x.reg2), x.imm,
            toString(x.reg1));
        }
      case Unit::ri:
        return fmt::format(
          "{} {}, {}", toString(x.code), toString(x.reg1), x.imm);
      case Unit::rr:
        return fmt::format(
          "{} {}, {}", toString(x.code), toString(x.reg1), toString(x.reg2));
      case Unit::branch:
        if (x.code == j) {
          return fmt::format("{} .l{}", toString(x.code), x.label_id);
        } else {
          return fmt::format(
            "{} {}, {}, .l{}", toString(x.code), toString(x.reg1),
            toString(x.reg2), x.label_id);
        }
      case Unit::misc:
        switch (x.code) {
        case call:
          return fmt::format("{} {}", toString(x.code), x.name.raw());
        case ret:
          return fmt::format("{}", toString(x.code));
        case la:
          return fmt::format(
            "{} {}, v{}", toString(x.code), toString(x.reg1), x.global_id);
        case label:
          return fmt::format(".l{}:", x.label_id);
        default:
          C_ERROR("bad ENUM");
        }
      default:
        C_ERROR("bad Unit::mode()");
      }
    }
#pragma GCC diagnostic pop
  } // namespace Glimmy
} // namespace SysY