#pragma once

#include "boxed.hpp"
#include "primitive.hpp"
#include "util.hpp"

namespace SysY {
  namespace Pinkie {
    struct Operand {
      enum Namespace : char { literal = 0, temp = 't', param = 'p', var = 'T' };
      int id;
      Namespace ns;
      bool operator<(const Operand &rhs) const {
        return ns < rhs.ns || (ns == rhs.ns && id < rhs.id);
      }
      bool operator==(const Operand &rhs) const {
        return ns == rhs.ns && id == rhs.id;
      }
    };

    inline Operand operand(char c, int id) {
      return Operand{id, static_cast<Operand::Namespace>(c)};
    }

    inline Operand literal(int id) { return Operand{id}; }

    struct Symb {
      Operand name;
      int len;
    };

    struct Unit {
      enum Type {
        unary,
        binary,
        copy,
        aw,
        ar,
        label,
        jmp,
        jc,
        param,
        calli,
        callv,
        reti,
        retv,
        n_unit_type
      } type;

      union {
        gc_str name;
        UnaryOp uop;
        BinaryOp bop;
      };
      union {
        int id;
        Operand dst;
      };
      union {
        Operand val;
        Operand src;
        Operand src1;
        Operand cond;
      };
      union {
        Operand incr;
        Operand src2;
      };
      PointerRange<Operand> ops();
      PointerRange<Operand const> ops() const;
      PointerRange<Operand> rops();
      PointerRange<Operand const> rops() const;
      PointerRange<Operand> hrops();
      PointerRange<Operand const> hrops() const;
      PointerRange<Operand> wops();
      PointerRange<Operand const> wops() const;
    };

    struct UnitOpr {
      Unit::Type type;
      void *_;
      Operand dst;
      Operand _ops[2];
      static int wop_count(Unit::Type t);
      static int hwop_count(Unit::Type t);
      static int rop_count(Unit::Type t);
      PointerRange<Operand> ops();
      PointerRange<Operand const> ops() const;
      PointerRange<Operand> rops();
      PointerRange<Operand const> rops() const;
      PointerRange<Operand> hrops();
      PointerRange<Operand const> hrops() const;
      PointerRange<Operand> wops();
      PointerRange<Operand const> wops() const;
    };

    static_assert(sizeof(UnitOpr) == sizeof(Unit));

    using Decl = std::vector<Symb>;
    using Code = std::vector<Unit>;

    struct Func {
      gc_str name;
      int arity;
      Decl decl;
      Code code;
    };

    struct Prog {
      int label_count;
      Decl globals;
      std::vector<Func> funcs;
    };
  } // namespace Pinkie
} // namespace SysY