#include "interop.hpp"
#include "error.hpp"
#include <map>

namespace SysY {
  namespace Pinkie {
    FromEeyore from_eeyore;
    ToEeyore to_eeyore;
  } // namespace Pinkie

  namespace Dashie {
    enum class VarStyle {
      reg_saved,
      reg_temp,
      stk_saved,
    };
    struct FrameItem {
      Pinkie::Symb origin;
      VarStyle style;
      Reg reg;
      int offset;
    };

    struct AnnotatedFunc : Function {
      int n_callee_saved;
      std::map<Pinkie::Operand, FrameItem> locals;
      bool leaf;
      int callee_saved_offset(int x) const { return x * 4; }
    };

    struct PinkieCompiler {
      int label_count;
      std::map<Pinkie::Operand, Dashie::Global> g_lookup;
      Program p;
      std::vector<Dashie::AnnotatedFunc> annotated;
      std::vector<Reg> tempos = {
        Reg{temp, 1}, Reg{temp, 2}, Reg{temp, 3},
        Reg{temp, 4}, Reg{temp, 5}, Reg{temp, 6},
      };
      int tempo_cnt = 0;

      Reg next_reg() {
        tempo_cnt++;
        if (tempo_cnt >= tempos.size())
          tempo_cnt = 0;
        return tempos[tempo_cnt];
      }

      static Reg abi_argument(int i) {
        if (i < 8) {
          return Reg{arg, short(i)};
        } else if (i < 12) {
          return Reg{temp, short(i - 8 + 3)};
        } else {
          C_ERROR("bad ABI argument number!");
        }
      }

      void convert_global(const Pinkie::Decl &src) {
        int c_global = 0;
        p.globals.reserve(src.size());
        for (auto symb : src) {
          auto glob = Dashie::Global{
            .id = c_global++,
            .length = symb.len,
          };
          g_lookup.insert(std::make_pair(symb.name, glob));
          p.globals.push_back(glob);
        }
      }

      static bool is_leaf(const Pinkie::Func &func) {
        return std::none_of(
          func.code.begin(), func.code.end(), [](const Pinkie::Unit &c) {
            return c.type == Pinkie::Unit::calli ||
                   c.type == Pinkie::Unit::callv;
          });
      }

      int callee_saved_heuristic(const Pinkie::Func &func) {
        int n_loc = func.decl.size();
        int x = (n_loc - 6) * 0.8;
        if (x < 1)
          x = 1;
        if (x > 12)
          x = 12;
        return x;
      }

      void
      annotate_variable(Dashie::AnnotatedFunc &a, const Pinkie::Func &func) {
        a.n_callee_saved = callee_saved_heuristic(func);
        tempos = {
          Reg{temp, 1}, Reg{temp, 2}, Reg{temp, 3},
          Reg{temp, 4}, Reg{temp, 5}, Reg{temp, 6},
        };
        for (int i = 0; i < a.n_callee_saved; i++) {
          tempos.push_back(Reg{save, short(i)});
        }
        tempo_cnt = 0;
        for (auto i = 0; i < func.arity; i++) {
          auto symb = Pinkie::Symb{Pinkie::operand('p', i), 0};
          a.locals.emplace(
            symb.name, FrameItem{.origin = symb, .style = VarStyle::stk_saved});
        }
        for (auto &symb : func.decl) {
          a.locals.emplace(
            symb.name, FrameItem{.origin = symb, .style = VarStyle::stk_saved});
        }
      }

      void compute_offset(Dashie::AnnotatedFunc &a) {
        int offset = a.n_callee_saved * 4;
        for (auto &c : a.locals) {
          c.second.offset = offset;
          if (c.second.origin.len) {
            offset += c.second.origin.len;
          } else {
            offset += 4;
          }
        }
        a.frame = offset;
      }

      int new_label() { return label_count++; }

      void generate_code(Dashie::AnnotatedFunc &a, const Pinkie::Func &func) {
        int epilogue_id = new_label();
        // Prologue
        for (int i = 0; i < a.arity; i++) {
          a.code.push_back(Unit{
            .type = Unit::wstk,
            .src = abi_argument(i),
            .stack_incr = a.locals.at(Pinkie::operand('p', i)).offset});
        }
        for (int i = 0; i < a.n_callee_saved; i++) {
          a.code.push_back(Unit{
            .type = Unit::wstk,
            .src = Reg{save, short(i)},
            .stack_incr = a.callee_saved_offset(i)});
        }

        auto load = [this, &func, &a](Reg reg, Pinkie::Operand opr) -> void {
          if (opr.ns == Pinkie::Operand::literal) {
            if (opr.id == 0) {
              a.code.push_back(Unit{
                .type = Unit::cr, .dst = reg, .src = Reg{RegType::konst, 0}});
            } else {
              a.code.push_back(
                Unit{.type = Unit::ci, .dst = reg, .imm = opr.id});
            }
          } else if (g_lookup.count(opr)) {
            const auto &glob = g_lookup.at(opr);
            if (glob.length == 0) {
              a.code.push_back(
                Unit{.type = Unit::rglo, .dst = reg, .global_id = glob.id});
            } else {
              a.code.push_back(
                Unit{.type = Unit::ptrglo, .dst = reg, .global_id = glob.id});
            }
          } else if (a.locals.count(opr)) {
            const auto &loc = a.locals.at(opr);
            C_ASSERT(loc.style == VarStyle::stk_saved);
            if (loc.origin.len == 0) {
              a.code.push_back(
                Unit{.type = Unit::rstk, .dst = reg, .stack_incr = loc.offset});
            } else {
              a.code.push_back(Unit{
                .type = Unit::ptrstk, .dst = reg, .stack_incr = loc.offset});
            }
          } else {
            C_ERROR("control reaches the end");
          }
        };

        auto store = [this, &func, &a](Reg reg, Pinkie::Operand opr) -> void {
          auto t1 = next_reg();
          if (g_lookup.count(opr)) {
            const auto &glob = g_lookup.at(opr);
            C_ASSERT(glob.length == 0);
            a.code.push_back(
              Unit{.type = Unit::ptrglo, .dst = t1, .global_id = glob.id});
            a.code.push_back(
              Unit{.type = Unit::aw, .dst = t1, .src = reg, .incr = 0});
          } else if (a.locals.count(opr)) {
            const auto &loc = a.locals.at(opr);
            C_ASSERT(loc.style == VarStyle::stk_saved);
            C_ASSERT(loc.origin.len == 0);
            a.code.push_back(
              Unit{.type = Unit::wstk, .src = reg, .stack_incr = loc.offset});
          } else {
            C_ERROR("control reaches the end");
          }
        };

        auto retv = Reg{RegType::arg, 0};

        int param_c = 0;

        for (auto &c : func.code) {
          auto tmp0 = next_reg();
          auto tmp1 = next_reg();
          switch (c.type) {
          case Pinkie::Unit::unary:
            C_ASSERT(c.src.ns != Pinkie::Operand::literal);
            load(tmp0, c.src);
            a.code.push_back(
              Unit{.type = Unit::ur, .uop = c.uop, .dst = tmp0, .src = tmp0});
            store(tmp0, c.dst);
            break;
          case Pinkie::Unit::binary:
            if (c.src2.ns == Pinkie::Operand::literal) {
              load(tmp0, c.src1);
              a.code.push_back(Unit{
                .type = Unit::rbi,
                .bop = c.bop,
                .dst = tmp0,
                .src1 = tmp0,
                .imm = c.src2.id});
              store(tmp0, c.dst);
            } else {
              load(tmp0, c.src1);
              load(tmp1, c.src2);
              a.code.push_back(Unit{
                .type = Unit::rbr,
                .bop = c.bop,
                .dst = tmp0,
                .src1 = tmp0,
                .src2 = tmp1});
              store(tmp0, c.dst);
            }
            break;
          case Pinkie::Unit::copy:
            load(tmp0, c.src);
            store(tmp0, c.dst);
            break;
          case Pinkie::Unit::aw:
            if (c.incr.ns == Pinkie::Operand::literal) {
              load(tmp0, c.dst);
              load(tmp1, c.src);
              a.code.push_back(Unit{
                .type = Unit::aw, .dst = tmp0, .src = tmp1, .incr = c.incr.id});
            } else {
              load(tmp0, c.dst);
              load(tmp1, c.incr);
              a.code.push_back(Unit{
                .type = Unit::rbr,
                .bop = BinaryOp::Plus,
                .dst = tmp0,
                .src1 = tmp0,
                .src2 = tmp1});
              load(tmp1, c.src);
              a.code.push_back(
                Unit{.type = Unit::aw, .dst = tmp0, .src = tmp1, .incr = 0});
            }
            break;
          case Pinkie::Unit::ar:
            if (c.incr.ns == Pinkie::Operand::literal) {
              load(tmp1, c.src);
              a.code.push_back(Unit{
                .type = Unit::ar, .dst = tmp0, .src = tmp1, .incr = c.incr.id});
              store(tmp0, c.dst);
            } else {
              load(tmp1, c.src);
              load(tmp0, c.incr);
              a.code.push_back(Unit{
                .type = Unit::rbr,
                .bop = BinaryOp::Plus,
                .dst = tmp1,
                .src1 = tmp1,
                .src2 = tmp0});
              a.code.push_back(
                Unit{.type = Unit::ar, .dst = tmp0, .src = tmp1, .incr = 0});
              store(tmp0, c.dst);
            }
            break;
          case Pinkie::Unit::label:
            a.code.push_back(Unit{.type = Unit::label, .label_id = c.id});
            break;
          case Pinkie::Unit::jmp:
            a.code.push_back(Unit{.type = Unit::jmp, .label_id = c.id});
            break;
          case Pinkie::Unit::jc:
            C_ASSERT(c.src2 == Pinkie::literal(0));
            load(tmp0, c.src1);
            a.code.push_back(Unit{
              .type = Unit::jc,
              .bop = c.bop,
              .label_id = c.id,
              .src1 = tmp0,
              .src2 = Reg{RegType::konst, 0}});
            break;
          case Pinkie::Unit::param:
            load(abi_argument(param_c), c.src);
            param_c++;
            break;
          case Pinkie::Unit::calli:
            a.code.push_back(Unit{.type = Unit::call, .name = c.name});
            store(retv, c.dst);
            param_c = 0;
            break;
          case Pinkie::Unit::callv:
            a.code.push_back(Unit{.type = Unit::call, .name = c.name});
            param_c = 0;
            break;
          case Pinkie::Unit::reti:
            load(retv, c.src);
            // fall through
          case Pinkie::Unit::retv:
            a.code.push_back(Unit{.type = Unit::jmp, .label_id = epilogue_id});
            break;
          default:
            C_ERROR("bad Pinkie::Type");
          }
        }
        // Epilogue
        a.code.push_back(Unit{
          .type = Unit::label,
          .label_id = epilogue_id,
        });
        for (int i = 0; i < a.n_callee_saved; i++) {
          a.code.push_back(Unit{
            .type = Unit::rstk,
            .dst = Reg{save, short(i)},
            .stack_incr = i * 4});
        }
        a.code.push_back(Unit{
          .type = Unit::ret,
        });
      }

      Dashie::AnnotatedFunc compile_func(const Pinkie::Func &func) {
        auto result = AnnotatedFunc{{
          .name = func.name,
          .arity = func.arity,
        }};
        result.leaf = is_leaf(func);

        annotate_variable(result, func);
        compute_offset(result);
        generate_code(result, func);

        return result;
      }

      Program compile(const Pinkie::Prog &src) {
        label_count = src.label_count;
        convert_global(src.globals);
        annotated.reserve(p.funcs.size());
        for (const auto &f : src.funcs) {
          annotated.push_back(compile_func(f));
        }
        p.label_count = label_count;
        for (const auto &f : annotated) {
          p.funcs.push_back(f);
        }
        return p;
      }
    };

    Program compile(const Pinkie::Prog &src) {
      return PinkieCompiler{}.compile(src);
    }
  } // namespace Dashie
} // namespace SysY