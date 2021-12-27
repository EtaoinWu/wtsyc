#include "semantics.hpp"
#include "dynamic_ast.hpp"
#include <algorithm>
#include <fmt/format.h>

namespace SysY {
  const SemanticType SemanticType::scalar = SemanticType{{}};

  SemanticType::SemanticType(const std::vector<literal_type> &dimensions_)
      : dimensions(dimensions_) {
    calculate_offsets();
  }

  SemanticType SemanticType::drop_first() const {
    auto result = *this;
    result.dimensions.erase(result.dimensions.begin());
    result.offsets.erase(result.offsets.begin());
    return result;
  }

  void SemanticType::calculate_offsets() {
    offsets.resize(dimensions.size());
    int prod = 1;
    for (int i = offsets.size() - 1; i >= 0; --i) {
      offsets[i] = prod;
      prod *= dimensions[i];
    }
  }

  namespace AST {
    std::string OffsetList::toString() const {
      return fmt::format("[{}]", fmt::join(offsets, "]["));
    }

    json OffsetList::toJSON() const { return AST::toJSON(offsets); }

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

    std::string Symbol::toString() const {
      return fmt::format("Symbol {}", name);
    }

    std::string LiteralExpression::toString() const {
      return fmt::format("LiteralExpression ({})", val);
    }

    json LiteralExpression::toJSON() const { return val; }

    std::string UnaryExpression::toString() const {
      return fmt::format("UnaryExpression {} ({})", SysY::toString(op),
                         ch->toString());
    }

    json UnaryExpression::toJSON() const {
      return {
          {"type", "unary"}, {"op", SysY::toString(op)}, {"ch", ch->toJSON()}};
    }

    std::string BinaryExpression::toString() const {
      return fmt::format("BinaryExpression {} ({}, {})", SysY::toString(op),
                         ch0->toString(), ch1->toString());
    }

    json BinaryExpression::toJSON() const {
      return {{"type", "binary"},
              {"op", SysY::toString(op)},
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

    std::string CallParams::toString() const {
      return fmt::format("({})", fmt::join(params, ", "));
    }

    json CallParams::toJSON() const { return AST::toJSON(params); }

    std::string CallExpression::toString() const {
      return fmt::format("CallExpression {} {}", func->toString(),
                         args.toString());
    }

    json CallExpression::toJSON() const {
      return {
          {"type", "call"}, {"func", func->toJSON()}, {"args", args.toJSON()}};
    }

    std::string EmptyStmt::toString() const { return "nop"; }

    json EmptyStmt::toJSON() const { return "nop"; }

    std::string BreakStmt::toString() const { return "break"; }

    json BreakStmt::toJSON() const { return "break"; }

    std::string ContinueStmt::toString() const { return "continue"; }

    json ContinueStmt::toJSON() const { return "continue"; }

    std::string Block::toString() const {
      return fmt::format("{{{}}}", fmt::join(code, "; "));
    }

    json Block::toJSON() const { return AST::toJSON(code); }

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
      return {
          {"stmt", "return"},
          {"return", rhs.has_value() ? rhs.value()->toJSON() : json(nullptr)}};
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
              {"else", else_clause.has_value() ? else_clause.value()->toJSON()
                                               : json(nullptr)}};
    }

    std::string WhileStmt::toString() const {
      return fmt::format("While {} Do {}", cond->toString(), body->toString());
    }

    json WhileStmt::toJSON() const {
      return {
          {"stmt", "while"}, {"while", cond->toJSON()}, {"do", body->toJSON()}};
    }

    std::string PrimitiveFunction::toString() const {
      return fmt::format("PrimitiveFunction {} {} ({})", SysY::toString(ret),
                         name, fmt::join(params, ", "));
    }

    json PrimitiveFunction::toJSON() const {
      return {{"return", SysY::toString(ret)},
              {"name", name},
              {"params", AST::toJSON(params)}};
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
                         fmt::join(globals, "\n"), fmt::join(functions, "\n"));
    }

    json CompUnit::toJSON() const {
      return {{"type", "comp unit"},
              {"globals", AST::toJSON(globals)},
              {"functions", AST::toJSON(functions)}};
    }

    std::string ArrayLiteral::toString() const {
      return fmt::format("ArrayLiteral [{}]", fmt::join(data, ", "));
    }

    json ArrayLiteral::toJSON() const { return AST::toJSON(data); }
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
