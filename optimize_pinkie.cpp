#include "optimize.hpp"
#include "primitive.hpp"
#include <unordered_map>
#include <vector>

namespace SysY {
  namespace Pinkie {
    namespace Pass {

      Pinkie::Prog flip_binop(Pinkie::Prog prog) {
        for (auto &f : prog.funcs) {
          for (auto &c : f.code) {
            if (c.type == c.binary) {
              switch (c.bop) {
              case BinaryOp::Plus:
              case BinaryOp::Mult:
                if (c.src1.ns == Operand::literal) {
                  std::swap(c.src1, c.src2);
                }
                break;
              case BinaryOp::Minus:
                if (c.src2.ns == Operand::literal) {
                  c.bop = BinaryOp::Plus;
                  c.src2.id = -c.src2.id;
                }
              default:;
              }
            }
          }
        }
        return prog;
      }

      Pinkie::Prog clear_temp(Pinkie::Prog prog) {
        int count = 0;
        std::unordered_map<int, int> compress;
        for (auto &f : prog.funcs) {
          std::vector<int> local_temps;
          for (auto &c : f.code) {
            for (auto &op : c.ops()) {
              if (op.ns == Operand::temp) {
                if (compress.emplace(op.id, count).second) {
                  local_temps.push_back(count);
                  ++count;
                }
                op.id = compress[op.id];
              }
            }
          }
          Decl new_decl;
          for (const auto &d : f.decl) {
            if (d.name.ns == Operand::temp) {
              continue;
            }
            new_decl.push_back(d);
          }
          for (auto id : local_temps) {
            new_decl.push_back(Symb{Operand{id, Operand::temp}, 0});
          }
          f.decl = new_decl;
        }
        return prog;
      }

      std::optional<int> literal_calc(const Unit &u) {
        switch (u.type) {
        case Unit::unary:
          if (u.src.ns == Operand::literal) {
            return toOP(u.uop)(u.src.id);
          }
          return std::nullopt;
        case Unit::binary:
        case Unit::jc:
          if (u.src1.ns == Operand::literal && u.src2.ns == Operand::literal) {
            return toOP(u.bop)(u.src1.id, u.src2.id);
          }
          return std::nullopt;
        case Unit::copy:
          if (u.src.ns == Operand::literal) {
            return u.src.id;
          }
          return std::nullopt;
        default:
          return std::nullopt;
        }
      }

      Pinkie::Prog const_fold(Pinkie::Prog prog) {
        for (auto &f : prog.funcs) {
          std::unordered_map<int, int> assign_count;
          for (const auto &c : f.code) {
            for (auto op : c.wops()) {
              if (op.ns == Operand::temp) {
                assign_count[op.id]++;
              }
            }
          }
          std::unordered_map<int, int> literal_map;
          for (auto &c : f.code) {
            for (auto &op : c.rops()) {
              if (
                op.ns == Operand::temp && assign_count[op.id] == 1 &&
                literal_map.count(op.id)) {
                op = literal(literal_map[op.id]);
              }
            }
            auto val = literal_calc(c);
            if (val.has_value()) {
              if (c.type == c.jc) {
                bool v = val.value();
                if (v) {
                  c.type = c.jmp;
                } else {
                  // we use n_unit_type as NOP
                  c.type = c.n_unit_type;
                }
              } else {
                c.type = Unit::copy;
                c.src = literal(val.value());
                const auto &op = c.dst;
                if (op.ns == Operand::temp && assign_count[op.id] == 1) {
                  literal_map[op.id] = val.value();
                }
              }
            }
          }
        }
        return prog;
      }

      Pinkie::Prog unused_temp_remove(Pinkie::Prog prog) {
        for (auto &f : prog.funcs) {
          std::unordered_map<int, int> use_count;
          for (const auto &c : f.code) {
            for (auto op : c.hrops()) {
              if (op.ns == Operand::temp) {
                use_count[op.id]++;
              }
            }
          }
          Code new_code;
          for (const auto &c : f.code) {
            // u, b, c, aw, ar
            // into temporary
            // that is never read
            // -> remove this code
            if (
              c.type != c.calli && c.type != c.aw &&
              UnitOpr::wop_count(c.type) && c.dst.ns == Operand::temp &&
              use_count.count(c.dst.id) == 0) {
              continue;
            }
            // remove nop
            if (c.type == c.n_unit_type) {
              continue;
            }
            new_code.push_back(c);
          }
          f.code = new_code;
        }
        return prog;
      }

      Pinkie::Prog
      operator|(const Pinkie::Prog &prog, Pinkie::Prog (*f)(Pinkie::Prog)) {
        return f(prog);
      }
      Pinkie::Prog
      operator|(Pinkie::Prog &&prog, Pinkie::Prog (*f)(Pinkie::Prog)) {
        return f(prog);
      }
      Pinkie::Prog optimize(Pinkie::Prog prog) {
        return prog | const_fold | unused_temp_remove | clear_temp | flip_binop;
      }
    } // namespace Pass
  }   // namespace Pinkie
} // namespace SysY