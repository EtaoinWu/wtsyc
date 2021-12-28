#pragma once

#include "primitive.hpp"
#include "boxed.hpp"

namespace SysY {
  namespace Pinkie {
    struct Operand {
      enum Namespace : char { literal = 0, temp = 't', param = 'p', var = 'T' };
      int id;
      Namespace ns;
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
    };


    struct UnitOpr {
      Unit::Type type;
      void * _;
      Operand ops[3];
      static int op_count(Unit::Type t);
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
      Decl globals;
      std::vector<Func> funcs;
    };
  } // namespace Pinkie
} // namespace SysY