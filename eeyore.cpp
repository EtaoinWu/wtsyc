#include "eeyore.hpp"
#include "error.hpp"
#include "util.hpp"
#include <fmt/core.h>
#include <fpp/fstring.h>

namespace SysY::Eeyore {
  const F::String EMPTY = F::string("");
  const F::String EOL = F::string("\n");
  const F::String INDENT = F::string("  ");

  std::string varName(const VariableX &var) {
    return std::visit(
      overloaded{
        [](const VariableI &v) {
          return fmt::format("{}{}", to_underlying(v.cat), v.id);
        },
      },
      var);
  }

  std::string operandName(const Operand &oprand) {
    return std::visit(
      overloaded{
        [](const VariableX &v) { return varName(v); },
        [](const Literal &v) { return fmt::format("{}", v); },
      },
      oprand);
  }

  F::String outputDecl(const DeclarationX &dec) {
    return F::string(std::visit(
                       overloaded{
                         [](const DeclarationS &dec) {
                           return fmt::format("var {}", varName(dec.name));
                         },
                         [](const DeclarationA &dec) {
                           return fmt::format(
                             "var {} {}", dec.len, varName(dec.name));
                         }},
                       dec)
                       .c_str());
  }

  F::String outputStmt(const Statement &stmt) {
    return F::string(
      std::visit(
        overloaded{
          [](const ExprU &p) {
            return fmt::format(
              "{} = {} {}", varName(p.dst), toString(p.op), operandName(p.src));
          },
          [](const ExprB &p) {
            return fmt::format(
              "{} = {} {} {}", varName(p.dst), operandName(p.src1),
              toString(p.op), operandName(p.src2));
          },
          [](const ExprC &p) {
            return fmt::format("{} = {}", varName(p.dst), operandName(p.src));
          },
          [](const ExprAW &p) {
            return fmt::format(
              "{}[{}] = {}", varName(p.dst_b), operandName(p.dst_i),
              operandName(p.src));
          },
          [](const ExprAR &p) {
            return fmt::format(
              "{} = {}[{}]", varName(p.dst), varName(p.src_b),
              operandName(p.src_i));
          },
          [](const Label &p) { return fmt::format("l{}:", p.id); },
          [](const JumpU &p) { return fmt::format("goto l{}", p.tgt.id); },
          [](const JumpC &p) {
            return fmt::format("if {} == 0 goto l{}", varName(p.cnd), p.tgt.id);
          },
          [](const Param &p) {
            return fmt::format("param {}", operandName(p.param));
          },
          [](const CallI &p) {
            return fmt::format("{} = call f_{}", varName(p.ret), p.func);
          },
          [](const CallV &p) { return fmt::format("call f_{}", p.func); },
          [](const RetI &p) {
            return fmt::format("return {}", operandName(p.ret));
          },
          [](const RetV &p) { return fmt::format("return"); },
        },
        stmt)
        .c_str());
  }

  F::String outputFunc(const Function &func) {
    F::String result =
      F::string(fmt::format("f_{} [{}]\n", func.name, func.arity).c_str());

    for (auto const &dec : func.dec) {
      result += INDENT + outputDecl(dec) + EOL;
    }
    result += EOL;

    for (auto const &stmt : func.code) {
      result += INDENT + outputStmt(stmt) + EOL;
    }
    result += F::string(fmt::format("end f_{}\n", func.name).c_str());
    return result;
  }

  std::string output_program(const Program &prog) {
    F::String result = EMPTY;
    for (auto const &dec : prog.global) {
      result += outputDecl(dec) + EOL;
    }
    result += EOL;

    for (auto const &func : prog.func) {
      result += outputFunc(func) + EOL;
    }
    return std::string{F::c_str(result)};
  }

  struct Test {
    Test() {
      auto test_prog = Program{
        0,
        {
          DeclarationS{VariableI{VCategory::var, 0}},
          DeclarationA{VariableI{VCategory::var, 0}, 40},
        },
        {Function{
          "main",
          0,
          {

            DeclarationS{VariableI{VCategory::var, 2}},
            DeclarationS{VariableI{VCategory::var, 3}},
            DeclarationS{VariableI{VCategory::temp, 0}},
            DeclarationS{VariableI{VCategory::temp, 1}},
            DeclarationS{VariableI{VCategory::temp, 2}},
          },
          {
            CallI{VariableI{VCategory::var, 0}, "getint"},
            RetI{0},
          }}}};
      auto output = output_program(test_prog);
      DEBUG_LOG("\n" + output);
    }
  };
} // namespace SysY::Eeyore
