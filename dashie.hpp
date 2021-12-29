#pragma once
#include "boxed.hpp"
#include "pinkie.hpp"
#include <map>

namespace SysY {
  namespace Dashie {
    enum RegType : char {
      konst = 'x',
      arg = 'a',
      temp = 't',
      save = 's',
    };
    struct Reg {
      RegType ns;
      short id;
      bool operator<(const Reg &rhs) const {
        return ns < rhs.ns || (ns == rhs.ns && id < rhs.id);
      }
      bool operator==(const Reg &rhs) const {
        return ns == rhs.ns && id == rhs.id;
      }
    };
    struct Unit {
      enum Type {
        rbr, // reg - binaryop - reg
        rbi, // reg - binaryop - imm
        ur,  // unary - reg
        cr,  // copy - reg
        ci,  // copy - imm
        ar,  // array read
        aw,  // array write
        jc,  // jumpCC
        jmp, // jump
        label,
        call,
        ret,
        wstk,   // write stack
        rstk,   // read stack
        rglo,   // read global
        ptrstk, // load pointer of stack
        ptrglo, // load pointer of global
        n_unit_type
      };
      Type type;
      union {
        UnaryOp uop;
        BinaryOp bop;
      };
      union {
        int label_id;
        Reg dst;
      };
      union {
        Reg src;
        Reg src1;
      };
      union {
        Reg src2;
        int global_id;
        // in BYTE
        int stack_incr;
        // in BYTE
        int imm;
        // in BYTE
        int incr;
        gc_str name;
      };
    };

    struct Global {
      int id;
      // in BYTE
      int length;
    };

    struct Function {
      gc_str name;
      int arity;
      // in BYTE
      int frame;
      std::vector<Unit> code;
    };

    struct Program {
      int label_count;
      std::vector<Global> globals;
      std::vector<Function> funcs;
    };

    std::string to_tigger(const Program &prog);
  } // namespace Dashie
} // namespace SysY
