#include "riscv_output.hpp"
#include "error.hpp"
#include "primitive.hpp"
#include "util.hpp"
#include <fmt/core.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wenum-compare"
namespace SysY {
  namespace Glimmy {
    Reg reg(Dashie::Reg r) {
      switch (r.ns) {
      case Dashie::RegType::konst:
        return x0;
      case Dashie::RegType::arg:
        C_ASSERT(r.id <= 7);
        return (Reg)(short)(a0 + r.id);
      case Dashie::RegType::temp:
        C_ASSERT(r.id <= 6);
        return (Reg)(short)(t0 + r.id);
      case Dashie::RegType::save:
        C_ASSERT(r.id <= 11);
        return (Reg)(short)(s0 + r.id);
      default:
        C_ERROR("bad Dashie::RegType");
      }
    }

    Unit imm_opt(Unit x) {
      if (
        (x.code == add || x.code == slt) && x.reg3 == imm && is_int12(x.imm)) {
        x.code = Internal(x.code == add ? addi : slti);
      }
      return x;
    }

    std::vector<Unit> imm_shrink(Unit x) {
      if (x.mode() == Unit::rrr && x.reg3 == imm) {
        x.reg3 = t0;
        return {
          Unit{
            .code = _li,
            .reg1 = t0,
            .imm = x.imm,
          },
          x};
      }
      return {x};
    }

    std::vector<Unit> imm_elim(const Unit &x) {
      if (x.mode() == Unit::rri && !is_int12(x.imm)) {
        switch (x.code) {
        case addi:
          return {
            Unit{.code = _li, .reg1 = t0, .imm = x.imm},
            Unit{
              .code = _add,
              .reg1 = x.reg1,
              .reg2 = x.reg2,
              .reg3 = t0,
            },
          };
        case slti:
          return {
            Unit{.code = _li, .reg1 = t0, .imm = x.imm},
            Unit{
              .code = _slt,
              .reg1 = x.reg1,
              .reg2 = x.reg2,
              .reg3 = t0,
            },
          };
        case lw:
          return {
            Unit{
              .code = _li,
              .reg1 = t0,
              .imm = x.imm,
            },
            Unit{
              .code = _add,
              .reg1 = t0,
              .reg2 = x.reg2,
              .reg3 = t0,
            },
            Unit{.code = _lw, .reg1 = x.reg1, .reg2 = t0, .imm = 0},
          };
        case sw:
          return {
            Unit{
              .code = _li,
              .reg1 = t0,
              .imm = x.imm,
            },
            Unit{
              .code = _add,
              .reg1 = t0,
              .reg2 = x.reg1,
              .reg3 = t0,
            },
            Unit{.code = _sw, .reg1 = t0, .reg2 = x.reg2, .imm = 0},
          };
        default:
          return {x};
        }
      }
      return {x};
    }

    template <typename T, typename Func>
    std::vector<T> operator>>(const std::vector<T> &l, Func f) {
      std::vector<T> result;
      for (const auto &item : l) {
        merge_into(result, f(item));
      }
      return result;
    }

    template <typename T>
    std::vector<T> monad_join(const std::vector<std::vector<T>> v) {
      std::vector<T> result;
      for (const auto &item : v) {
        merge_into(result, item);
      }
      return result;
    }

    std::vector<Unit> convert(const Dashie::Unit &x, int STK) {
      switch (x.type) {
      case Dashie::Unit::rbr: {
        auto [ins1, ins2] = toRiscVOP(x.bop);
        if (ins2 != n_rr) {
          return {
            Unit{
              .code = (Internal)ins1,
              .reg1 = reg(x.dst),
              .reg2 = reg(x.src1),
              .reg3 = reg(x.src2),
            },
            Unit{
              .code = (Internal)ins2,
              .reg1 = reg(x.dst),
              .reg2 = reg(x.dst),
            },
          };
        } else {
          return {Unit{
            .code = (Internal)ins1,
            .reg1 = reg(x.dst),
            .reg2 = reg(x.src1),
            .reg3 = reg(x.src2)}};
        }
      }
      case Dashie::Unit::rbi: {
        auto [ins1, ins2] = toRiscVOP(x.bop);
        if (ins2 != n_rr) {
          return {
            Unit{
              .code = (Internal)ins1,
              .reg1 = reg(x.dst),
              .reg2 = reg(x.src1),
              .reg3 = imm,
              .imm = x.imm,
            },
            Unit{
              .code = (Internal)ins2,
              .reg1 = reg(x.dst),
              .reg2 = reg(x.dst),
            },
          };
        } else {
          return {Unit{
            .code = (Internal)ins1,
            .reg1 = reg(x.dst),
            .reg2 = reg(x.src1),
            .reg3 = imm,
            .imm = x.imm}};
        }
      }
      case Dashie::Unit::ur:
        return {Unit{
          .code = (Internal)toRiscVOP(x.uop),
          .reg1 = reg(x.dst),
          .reg2 = reg(x.src),
        }};
      case Dashie::Unit::cr:
        return {Unit{
          .code = _mv,
          .reg1 = reg(x.dst),
          .reg2 = reg(x.src),
        }};
      case Dashie::Unit::ci:
        return {Unit{
          .code = _li,
          .reg1 = reg(x.dst),
          .imm = x.imm,
        }};
      case Dashie::Unit::ar:
        return {Unit{
          .code = _lw,
          .reg1 = reg(x.dst),
          .reg2 = reg(x.src),
          .imm = x.incr,
        }};
      case Dashie::Unit::aw:
        return {Unit{
          .code = _sw,
          .reg1 = reg(x.dst),
          .reg2 = reg(x.src),
          .imm = x.incr,
        }};
      case Dashie::Unit::jc:
        return {Unit{
          .code = (Internal)toRiscVBranch(x.bop),
          .reg1 = reg(x.src1),
          .reg2 = reg(x.src2),
          .label_id = x.label_id,
        }};
      case Dashie::Unit::jmp:
        return {Unit{
          .code = _j,
          .label_id = x.label_id,
        }};
      case Dashie::Unit::label:
        return {Unit{
          .code = _label,
          .label_id = x.label_id,
        }};
      case Dashie::Unit::call:
        return {Unit{.code = _call, .name = x.name}};
      case Dashie::Unit::ret:
        return {
          Unit{.code = _lw, .reg1 = ra, .reg2 = sp, .imm = STK - 4},
          Unit{.code = _addi, .reg1 = sp, .reg2 = sp, .imm = STK},
          Unit{.code = _ret}};
      case Dashie::Unit::wstk:
        return {Unit{
          .code = _sw,
          .reg1 = sp,
          .reg2 = reg(x.src),
          .imm = x.stack_incr,
        }};
      case Dashie::Unit::rstk:
        return {Unit{
          .code = _lw,
          .reg1 = reg(x.dst),
          .reg2 = sp,
          .imm = x.stack_incr,
        }};
      case Dashie::Unit::rglo:
        return {
          Unit{
            .code = _la,
            .reg1 = reg(x.dst),
            .global_id = x.global_id,
          },
          Unit{
            .code = _lw,
            .reg1 = reg(x.dst),
            .reg2 = reg(x.dst),
            .imm = 0,
          }};
      case Dashie::Unit::ptrstk:
        return {Unit{
          .code = _addi,
          .reg1 = reg(x.dst),
          .reg2 = sp,
          .imm = x.stack_incr,
        }};
      case Dashie::Unit::ptrglo:
        return {Unit{
          .code = _la,
          .reg1 = reg(x.dst),
          .global_id = x.global_id,
        }};
      default:
        C_ERROR("control reaches the end");
      }
    }
#pragma GCC diagnostic pop

    Function convert_func(const Dashie::Function &f) {
      Function g;
      int stk = (f.frame / 4 + 1) * 16;
      g.STK = stk;
      g.name = f.name;
      auto convert_with_stk = [stk](const Dashie::Unit &x) {
        return convert(x, stk);
      };
      g.code = concat(
                 std::vector{
                   Unit{.code = _addi, .reg1 = sp, .reg2 = sp, .imm = -stk},
                   Unit{.code = _sw, .reg1 = sp, .reg2 = ra, .imm = stk - 4}},
                 monad_join(map_to_vector<std::vector<Unit>>(
                   f.code, convert_with_stk))) >>
               imm_elim >> imm_opt >> imm_shrink;
      return g;
    }

    std::string to_riscv_function(const Function &f) {
      std::string result;
      // prologue
      result += fmt::format(
        "  .text\n"
        "  .align  2\n"
        "  .global {}\n"
        "  .type   {}, @function\n"
        "{}:\n",
        f.name.raw(), f.name.raw(), f.name.raw());
      for (auto c : f.code) {
        if (c.code == _label) {
          result += toString(c);
        } else {
          result += "  " + toString(c);
        }
        result += "\n";
      }
      result += fmt::format(".size   {}, .-{}\n", f.name.raw(), f.name.raw());
      return result;
    }

    std::string to_riscv_dec(const Dashie::Global &dec) {
      if (dec.length == 0) {
        return fmt::format(
          "  .global   v{}\n"
          "  .section  .sdata\n"
          "  .align    2\n"
          "  .type     v{}, @object\n"
          "  .size     v{}, 4\n"
          "v{}:\n"
          "  .word     0\n",
          dec.id, dec.id, dec.id, dec.id);
      } else {
        return fmt::format("  .comm v{}, {}, 4\n", dec.id, dec.length);
      }
    }

    std::string to_riscv(const Dashie::Program &prog) {
      return join(
               map_to_vector<std::string>(prog.globals, to_riscv_dec), "\n") +
             "\n\n" +
             join(
               map_to_vector<std::string>(
                 map_to_vector<Function>(prog.funcs, convert_func),
                 to_riscv_function),
               "\n\n");
    }
  } // namespace Glimmy
} // namespace SysY