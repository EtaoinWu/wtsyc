#pragma once
#include "primitive.hpp"
#include <string>
#include <variant>
#include <vector>

namespace SysY {
  namespace Eeyore {
    template <typename T> using container = std::vector<T>;

    template <typename... Types> using sum_t = std::variant<Types...>;

    enum class VCategory : char { temp = 't', param = 'p', var = 'T' };
    struct VariableI {
      VCategory cat;
      int id;
    };
    using VariableX = sum_t<VariableI>;

    using Literal = int;
    using Operand = sum_t<VariableI, Literal>;

    struct ExprU {
      VariableX dst;
      UnaryOp op;
      Operand src;
    };
    struct ExprB {
      VariableX dst;
      BinaryOp op;
      Operand src1;
      Operand src2;
    };
    // Copy
    struct ExprC {
      VariableX dst;
      Operand src;
    };
    // Array Write
    struct ExprAW {
      // base
      VariableX dst_b;
      // increment
      Operand dst_i;
      Operand src;
    };
    // Array Read
    struct ExprAR {
      VariableX dst;
      // base
      VariableX src_b;
      // increment
      Operand src_i;
    };
    using ExprX = sum_t<ExprU, ExprB, ExprC, ExprAW, ExprAR>;

    // External Label
    struct LabelE {
      int id;
    };
    using Label = LabelE;

    // Unconditional Jump
    struct JumpU {
      Label tgt;
    };
    // Conditional Jump
    struct JumpC {
      Label tgt;
      VariableX cnd;
    };

    struct Param {
      Operand param;
    };

    struct CallI {
      VariableX ret;
      std::string func;
    };
    struct CallV {
      std::string func;
    };

    struct RetI {
      Operand ret;
    };
    struct RetV {};

    using Statement = sum_t<ExprU, ExprB, ExprC, ExprAW, ExprAR, Label, JumpU,
                            JumpC, Param, CallI, CallV, RetI, RetV>;

    // Scalar Declaration
    struct DeclarationS {
      VariableX name;
    };
    // Array Declaration
    struct DeclarationA {
      VariableX name;
      int len;
    };
    using DeclarationX = sum_t<DeclarationS, DeclarationA>;

    struct Function {
      std::string name;
      int arity;
      container<DeclarationX> dec;
      container<Statement> code;
    };

    struct Program {
      container<DeclarationX> global;
      container<Function> func;
    };

    std::string output_program(const Program &prog);
  } // namespace Eeyore
} // namespace SysY
