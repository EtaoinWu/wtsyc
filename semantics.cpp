#include "semantics.hpp"

#include "fmt/format.h"

namespace SysY {
  namespace AST {
    std::string OffsetList::toString() const {
      return fmt::format("[{}]", fmt::join(offsets, "]["));
    }

    json OffsetList::toJSON() const {
      return AST::toJSON(offsets);
    }

    void SemanticType::calculate_offsets() {
      offsets = dimensions;
      for (auto i = offsets.end() - 1; i >= offsets.begin(); i--) {
        *i *= *(i + 1);
      }
      for (auto i = offsets.begin(); i < offsets.end() - 1; i++) {
        *i = *(i + 1);
      }
      if (!offsets.empty()) {
        offsets.back() = 1;
      }
    }

    std::string ConstDeclaration::toString() const {
      return fmt::format("ConstDeclaration {} {} = {}", name,
                         offset_list.toString(), init_value->toString());
    }

    json ConstDeclaration::toJSON() const {
      return {{"type", "const_dec"},
              {"name", name},
              {"type", offset_list.toJSON()},
              {"init", init_value->toJSON()}};
    }

    std::string VariableDeclaration::toString() const {
      return fmt::format("VariableDeclaration {} {} = {}", name,
                         offset_list.toString(), init_value->toString());
    }

    json VariableDeclaration::toJSON() const {
      return {{"type", "var_dec"},
              {"name", name},
              {"type", offset_list.toJSON()},
              {"init", init_value->toJSON()}};
    }

    std::string ParamDeclaration::toString() const {
      return fmt::format("ParamDeclaration {} {}", name,
                         offset_list.toString());
    }

    json ParamDeclaration::toJSON() const {
      return {{"name", name}, {"type", offset_list.toJSON()}};
    }

    std::string Identifier::toString() const {
      return fmt::format("Identifier {}", name);
    }

    std::string Expressions::toString(UnaryOp t) {
      switch (t) {
      case UnaryOp::Plus:
        return "+";
      case UnaryOp::Minus:
        return "-";
      case UnaryOp::Not:
        return "!";
      default:
        return "";
      }
    }

    std::string Expressions::toString(BinaryOp t) {
      switch (t) {
      case BinaryOp::Plus:
        return "+";
      case BinaryOp::Minus:
        return "-";
      case BinaryOp::Mult:
        return "*";
      case BinaryOp::Div:
        return "/";
      case BinaryOp::Mod:
        return "%";
      case BinaryOp::LT:
        return "<";
      case BinaryOp::GT:
        return ">";
      case BinaryOp::LE:
        return "<=";
      case BinaryOp::GE:
        return ">=";
      case BinaryOp::NE:
        return "!=";
      case BinaryOp::EQ:
        return "==";
      case BinaryOp::And:
        return "&&";
      case BinaryOp::Or:
        return "||";
      default:
        return "";
      }
    }

    std::string LiteralExpression::toString() const {
      return fmt::format("LiteralExpression ({})", val);
    }

    json LiteralExpression::toJSON() const {
      return val;
    }

    std::string UnaryExpression::toString() const {
      return fmt::format("UnaryExpression {} ({})",
                         AST::Expressions::toString(op), ch->toString());
    }

    json UnaryExpression::toJSON() const {
      return {{"type", "unary"},
              {"op", AST::Expressions::toString(op)},
              {"ch", ch->toJSON()}};
    }

    std::string BinaryExpression::toString() const {
      return fmt::format("BinaryExpression {} ({}, {})",
                         AST::Expressions::toString(op), ch0->toString(),
                         ch1->toString());
    }

    json BinaryExpression::toJSON() const {
      return {{"type", "binary"},
              {"op", AST::Expressions::toString(op)},
              {"ch0", ch0->toJSON()},
              {"ch1", ch1->toJSON()}};
    }

    std::string OffsetExpression::toString() const {
      return fmt::format("OffsetExpression ({}, {})", arr->toString(),
                         offset->toString());
    }

    json OffsetExpression::toJSON() const {
      return {{"type", "offset"},
              {"arr", arr->toJSON()},
              {"offset", offset->toJSON()}};
    }

    std::string FuncParams::toString() const {
      return fmt::format("({})", fmt::join(params, ", "));
    }

    json FuncParams::toJSON() const {
      return AST::toJSON(params);
    }

    std::string CallExpression::toString() const {
      return fmt::format("CallExpression {} {}", func->toString(),
                         args.toString());
    }

    json CallExpression::toJSON() const {
      return {{"type", "call"},
              {"func", func->toJSON()},
              {"args", args.toJSON()}};
    }

    std::string BreakStmt::toString() const {
      return "break";
    }

    json BreakStmt::toJSON() const {
      return "break";
    }

    std::string ContinueStmt::toString() const {
      return "continue";
    }

    json ContinueStmt::toJSON() const {
      return "continue";
    }

    std::string Block::toString() const {
      return fmt::format("{{{}}}", fmt::join(code, "; "));
    }

    json Block::toJSON() const {
      return AST::toJSON(code);
    }

    std::string AssignmentStmt::toString() const {
      return fmt::format("Assignment {} = {}", lhs->toString(),
                         rhs->toString());
    }

    json AssignmentStmt::toJSON() const {
      return {{"stmt", "assignment"},
              {"lhs", lhs->toJSON()},
              {"rhs", rhs->toJSON()}};
    }

    std::string ExpressionStmt::toString() const {
      return fmt::format("ExprStmt {}", rhs->toString());
    }

    json ExpressionStmt::toJSON() const {
      return {{"stmt", "expr"}, {"rhs", rhs->toJSON()}};
    }

    std::string ReturnStmt::toString() const {
      if (rhs.has_value()) {
        return fmt::format("Return {}", rhs.value()->toString());
      } else {
        return "Return;";
      }
    }

    json ReturnStmt::toJSON() const {
      return {{"stmt", "return"},
              {"return",
               rhs.has_value() ? rhs.value()->toJSON() : json(nullptr)}};
    }

    std::string IfStmt::toString() const {
      return else_clause.has_value()
               ? fmt::format("If {} Then {} Else {}", cond->toString(),
                             then_clause->toString(),
                             else_clause.value()->toString())
               : fmt::format("If {} Then {}", cond->toString(),
                             then_clause->toString());
    }

    json IfStmt::toJSON() const {
      return {{"stmt", "if"},
              {"if", cond->toJSON()},
              {"then", then_clause->toJSON()},
              {"else", else_clause.has_value()
                         ? else_clause.value()->toJSON()
                         : json(nullptr)}};
    }

    std::string WhileStmt::toString() const {
      return fmt::format("While {} Do {}", cond->toString(),
                         body->toString());
    }

    json WhileStmt::toJSON() const {
      return {{"stmt", "while"},
              {"while", cond->toJSON()},
              {"do", body->toJSON()}};
    }

    std::string Function::toString() const {
      return fmt::format("Function {} {} ({}) {{{}}}", SysY::toString(ret),
                         name, fmt::join(params, ", "), code->toString());
    }

    json Function::toJSON() const {
      return {{"return", SysY::toString(ret)},
              {"name", name},
              {"params", AST::toJSON(params)},
              {"code", code->toJSON()}};
    }

    std::string CompUnit::toString() const {
      return fmt::format("CompUnit\nwith declaration: {}\nwith functions: {}",
                         fmt::join(globals, "\n"),
                         fmt::join(functions, "\n"));
    }

    json CompUnit::toJSON() const {
      return {{"type", "comp unit"},
              {"globals", AST::toJSON(globals)},
              {"functions", AST::toJSON(functions)}};
    }

    std::string ArrayLiteral::toString() const {
      return fmt::format("ArrayLiteral [{}]", fmt::join(data, ", "));
    }

    json ArrayLiteral::toJSON() const {
      return AST::toJSON(data);
    }
  } // namespace AST
} // namespace SysY


template <typename T>
struct fmt::formatter<SysY::AST::pointer<T>> : formatter<string_view> {
  // parse is inherited from formatter<string_view>.
  template <typename FormatContext>
  auto format(const SysY::AST::pointer<T> &c, FormatContext &ctx) {
    return formatter<string_view>::format(c->toString(), ctx);
  }
};
