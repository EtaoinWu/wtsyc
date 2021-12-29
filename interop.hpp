#pragma once
#include "eeyore.hpp"
#include "error.hpp"
#include "pinkie.hpp"
#include "dashie.hpp"

namespace SysY {
  namespace Pinkie {
    struct FromEeyore {
      Operand::Namespace operator()(Eeyore::VCategory cat) {
        switch (cat) {
        case Eeyore::VCategory::param:
          return Operand::Namespace::param;
        case Eeyore::VCategory::temp:
          return Operand::Namespace::temp;
        case Eeyore::VCategory::var:
          return Operand::Namespace::var;
        default:
          C_ERROR("bad VCategory");
        }
      }
      Operand operator()(Eeyore::VariableI var) {
        return Operand{
          var.id,
          (*this)(var.cat),
        };
      }
      Operand operator()(Eeyore::VariableX var) {
        return std::visit(*this, var);
      }
      Operand operator()(Eeyore::Literal var) { return literal(var); }
      Operand operator()(Eeyore::Operand var) { return std::visit(*this, var); }
      Unit operator()(const Eeyore::ExprU &stmt) {
        return Unit{
          .type = Unit::unary,
          .uop = stmt.op,
          .dst = (*this)(stmt.dst),
          .src = (*this)(stmt.src),
        };
      }
      Unit operator()(const Eeyore::ExprB &stmt) {
        return Unit{
          .type = Unit::binary,
          .bop = stmt.op,
          .dst = (*this)(stmt.dst),
          .src1 = (*this)(stmt.src1),
          .src2 = (*this)(stmt.src2),
        };
      }
      Unit operator()(const Eeyore::ExprC &stmt) {
        return Unit{
          .type = Unit::copy,
          .dst = (*this)(stmt.dst),
          .src = (*this)(stmt.src),
        };
      }
      Unit operator()(const Eeyore::ExprAW &stmt) {
        return Unit{
          .type = Unit::aw,
          .dst = (*this)(stmt.dst_b),
          .src = (*this)(stmt.src),
          .incr = (*this)(stmt.dst_i),
        };
      }
      Unit operator()(const Eeyore::ExprAR &stmt) {
        return Unit{
          .type = Unit::ar,
          .dst = (*this)(stmt.dst),
          .src = (*this)(stmt.src_b),
          .incr = (*this)(stmt.src_i),
        };
      }
      Unit operator()(const Eeyore::ExprX &stmt) {
        return std::visit(*this, stmt);
      }
      Unit operator()(const Eeyore::LabelE &stmt) {
        return Unit{
          .type = Unit::label,
          .id = stmt.id,
        };
      }
      Unit operator()(const Eeyore::JumpU &stmt) {
        return Unit{
          .type = Unit::jmp,
          .id = stmt.tgt.id,
        };
      }
      Unit operator()(const Eeyore::JumpC &stmt) {
        return Unit{
          .type = Unit::jc,
          .bop = BinaryOp::EQ,
          .id = stmt.tgt.id,
          .src1 = (*this)(stmt.cnd),
          .src2 = (*this)(0),
        };
      }
      Unit operator()(const Eeyore::Param &stmt) {
        return Unit{
          .type = Unit::param,
          .val = (*this)(stmt.param),
        };
      }
      Unit operator()(const Eeyore::CallI &stmt) {
        return Unit{
          .type = Unit::calli,
          .name = gc_copy_str(stmt.func),
          .dst = (*this)(stmt.ret),
        };
      }
      Unit operator()(const Eeyore::CallV &stmt) {
        return Unit{
          .type = Unit::callv,
          .name = gc_copy_str(stmt.func.c_str()),
        };
      }
      Unit operator()(const Eeyore::RetI &stmt) {
        return Unit{
          .type = Unit::reti,
          .val = (*this)(stmt.ret),
        };
      }
      Unit operator()(const Eeyore::RetV &stmt) {
        return Unit{
          .type = Unit::retv,
        };
      }
      Unit operator()(const Eeyore::Statement &stmt) {
        return std::visit(*this, stmt);
      }
      Symb operator()(const Eeyore::DeclarationS &dec) {
        return Symb{
          .name = (*this)(dec.name),
          .len = 0,
        };
      }
      Symb operator()(const Eeyore::DeclarationA &dec) {
        return Symb{
          .name = (*this)(dec.name),
          .len = dec.len,
        };
      }
      Symb operator()(const Eeyore::DeclarationX &dec) {
        return std::visit(*this, dec);
      }
      Code operator()(const Eeyore::container<Eeyore::Statement> &from) {
        return map_to_vector<Unit>(from, *this);
      }
      Decl operator()(const Eeyore::container<Eeyore::DeclarationX> &from) {
        return map_to_vector<Symb>(from, *this);
      }
      std::vector<Func>
      operator()(const Eeyore::container<Eeyore::Function> &from) {
        return map_to_vector<Func>(from, *this);
      }
      Func operator()(const Eeyore::Function &func) {
        return Func{
          .name = gc_copy_str(func.name),
          .arity = func.arity,
          .decl = (*this)(func.dec),
          .code = (*this)(func.code),
        };
      }
      Prog operator()(const Eeyore::Program &prog) {
        return Prog{
          .label_count = prog.label_count,
          .globals = (*this)(prog.global),
          .funcs = (*this)(prog.func),
        };
      }
    };
    extern FromEeyore from_eeyore;

    struct ToEeyore {
      Eeyore::VCategory operator()(Operand::Namespace ns) {
        switch (ns) {
        case Operand::Namespace::param:
          return Eeyore::VCategory::param;
        case Operand::Namespace::temp:
          return Eeyore::VCategory::temp;
        case Operand::Namespace::var:
          return Eeyore::VCategory::var;
        default:
          C_ERROR("bad Operand::Namespace");
        }
      }
      Eeyore::VariableI to_var(Operand var) {
        return Eeyore::VariableI{
          (*this)(var.ns),
          var.id,
        };
      }
      Eeyore::Operand operator()(Operand var) {
        switch (var.ns) {
        case Operand::Namespace::literal:
          return Eeyore::Literal{var.id};
        default:
          return to_var(var);
        }
      }

      Eeyore::DeclarationX operator()(Symb symb) {
        if (symb.len == 0) {
          return Eeyore::DeclarationS{
            to_var(symb.name),
          };
        } else {
          return Eeyore::DeclarationA{
            to_var(symb.name),
            symb.len,
          };
        }
      }
      Eeyore::Statement operator()(Unit stmt) {
        switch (stmt.type) {
        case Unit::unary:
          return Eeyore::ExprU{
            to_var(stmt.dst),
            stmt.uop,
            (*this)(stmt.src),
          };
        case Unit::binary:
          return Eeyore::ExprB{
            to_var(stmt.dst),
            stmt.bop,
            (*this)(stmt.src1),
            (*this)(stmt.src2),
          };
        case Unit::copy:
          return Eeyore::ExprC{
            to_var(stmt.dst),
            (*this)(stmt.src),
          };
        case Unit::aw:
          return Eeyore::ExprAW{
            to_var(stmt.dst),
            (*this)(stmt.incr),
            (*this)(stmt.src),
          };
        case Unit::ar:
          return Eeyore::ExprAR{
            to_var(stmt.dst),
            to_var(stmt.src),
            (*this)(stmt.incr),
          };
        case Unit::label:
          return Eeyore::Label{
            stmt.id,
          };
        case Unit::jmp:
          return Eeyore::JumpU{
            Eeyore::Label{stmt.id},
          };
        case Unit::jc:
          return Eeyore::JumpC{
            Eeyore::Label{stmt.id},
            to_var(stmt.cond),
          };
        case Unit::param:
          return Eeyore::Param{(*this)(stmt.val)};
        case Unit::calli:
          return Eeyore::CallI{
            to_var(stmt.dst),
            stmt.name.raw(),
          };
        case Unit::callv:
          return Eeyore::CallV{
            stmt.name.raw(),
          };
        case Unit::reti:
          return Eeyore::RetI{
            (*this)(stmt.val),
          };
        case Unit::retv:
          return Eeyore::RetV{};
        default:
          C_ERROR("bad Unit::Type");
        }
      }
      Eeyore::container<Eeyore::Statement> operator()(Code from) {
        return map_to_vector<Eeyore::Statement>(from, *this);
      }
      Eeyore::container<Eeyore::DeclarationX> operator()(Decl from) {
        return map_to_vector<Eeyore::DeclarationX>(from, *this);
      }
      Eeyore::Function operator()(Func from) {
        return Eeyore::Function{
          from.name.raw(),
          from.arity,
          (*this)(from.decl),
          (*this)(from.code),
        };
      }
      Eeyore::container<Eeyore::Function> operator()(std::vector<Func> from) {
        return map_to_vector<Eeyore::Function>(from, *this);
      }
      Eeyore::Program operator()(Prog from) {
        return Eeyore::Program{
          from.label_count,
          (*this)(from.globals),
          (*this)(from.funcs),
        };
      }
    };
    extern ToEeyore to_eeyore;
  } // namespace Pinkie
  namespace Dashie {
    Program compile(const Pinkie::Prog &src);
  }
} // namespace SysY