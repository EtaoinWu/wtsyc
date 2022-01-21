#include "error.hpp"
#include "optimize.hpp"
#include "primitive.hpp"
#include <map>
#include <vector>

namespace SysY {
  namespace Dashie {
    namespace Pass {
      struct VarAlias {
        std::map<Reg, int> alias_of;
        std::vector<Unit> operator()(Unit u) {
          switch (u.type) {
          case Unit::jc:
          case Unit::jmp:
          case Unit::call:
          case Unit::ret:
          case Unit::label:
            alias_of.clear();
            return {u};
          case Unit::rbr:
          case Unit::rbi:
          case Unit::ur:
          case Unit::cr:
          case Unit::ci:
          case Unit::ar:
          case Unit::rglo:
          case Unit::ptrstk:
          case Unit::ptrglo:
            alias_of.erase(u.dst);
            return {u};
          case Unit::aw:
            return {u};
          case Unit::wstk:
            for (auto it = alias_of.cbegin(); it != alias_of.cend();) {
              if (it->second == u.stack_incr) {
                alias_of.erase(it++);
              } else {
                it++;
              }
            }
            alias_of[u.src] = u.stack_incr;
            return {u};
          case Unit::rstk:
            if (alias_of.count(u.dst) && alias_of[u.dst] == u.stack_incr) {
              return {};
            }
            for (auto a : alias_of) {
              if (a.second == u.stack_incr) {
                alias_of[u.dst] = u.stack_incr;
                return {
                  Unit{
                    .type = Unit::cr,
                    .dst = u.dst,
                    .src = a.first,
                  },
                };
              }
            }
            alias_of[u.dst] = u.stack_incr;
            return {u};
          default:
            C_ERROR("bad Dashie Program Unit Type");
          }
        }
      };

      struct StkPtrAlias {
        std::map<Reg, int> alias_of;
        std::vector<Unit> operator()(Unit u) {
          switch (u.type) {
          case Unit::ptrstk:
            if (alias_of.count(u.dst) && alias_of[u.dst] == u.stack_incr) {
              return {};
            }
            alias_of[u.dst] = u.stack_incr;
            return {u};
          case Unit::jc:
          case Unit::jmp:
          case Unit::call:
          case Unit::ret:
          case Unit::label:
            alias_of.clear();
            return {u};
          case Unit::rbr:
          case Unit::rbi:
          case Unit::ur:
          case Unit::cr:
          case Unit::ci:
          case Unit::ar:
          case Unit::rstk:
          case Unit::rglo:
          case Unit::ptrglo:
            alias_of.erase(u.dst);
            return {u};
          case Unit::aw:
          case Unit::wstk:
            return {u};
          default:
            C_ERROR("bad Dashie Program Unit Type");
          }
        }
      };
      Dashie::Program peephole(Dashie::Program prog) {
        for (auto &func : prog.funcs) {
          func.code = func.code >> StkPtrAlias{} >> VarAlias{};
        }
        return prog;
      }
    } // namespace Pass
  }   // namespace Dashie
} // namespace SysY
