#pragma once

#include <string>
#include <vector>
#include <type_traits>
#include <optional>
#include "fmt/format.h"
#include "json/json.hpp"

namespace SysY {
  using nlohmann::json;
  enum class PrimitiveType { INT, VOID };

  inline std::string toString(PrimitiveType t) {
    switch (t) {
    case PrimitiveType::INT:
      return "int";
    case PrimitiveType::VOID:
      return "void";
    default:
      return "";
    }
  }

  namespace AST {
    template <typename T> using pointer = std::unique_ptr<T>;
    template <typename T> using container = std::vector<T>;

    template <typename T> json toJSON(const container<T> &c) {
      std::vector<json> c_json;
      std::transform(c.begin(), c.end(), std::back_inserter(c_json),
                     [](const T &exp) { return exp->toJSON(); });
      return c_json;
    }

    using literal_type = int;
    class Node {
    public:
      virtual std::string toString() const = 0;
      virtual json toJSON() const = 0;
    };

    class BlockItem : public Node {};
    class PotentialLiteral : public Node {};
    class NoneLiteral : public PotentialLiteral {
    public:
      std::string toString() const override { return "N/A"; }
      json toJSON() const override { return nullptr; }
    };

    namespace Expressions {
      class Expression : public PotentialLiteral {};
      class LeftValueExpression : public Expression {};

      class Identifier : public LeftValueExpression {
      public:
        std::string name;
        Identifier(std::string_view name_) : name{name_} {}
        std::string toString() const override {
          return fmt::format("Identifier {}", name);
        }
        json toJSON() const override { return name; }
      };

      enum class UnaryOp { Plus, Minus, Not };

      inline std::string toString(UnaryOp t) {
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

      enum class BinaryOp {
        Plus,
        Minus,
        Mult,
        Div,
        Mod,
        LT,
        GT,
        LE,
        GE,
        NE,
        EQ,
        And,
        Or
      };

      inline std::string toString(BinaryOp t) {
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

      class LiteralExpression : public Expression {
      public:
        literal_type val;
        LiteralExpression(literal_type val_) : val{val_} {}
        std::string toString() const override {
          return fmt::format("LiteralExpression ({})", val);
        }
        json toJSON() const override { return val; }
      };

      class UnaryExpression : public Expression {
      public:
        UnaryOp op;
        pointer<Expression> ch;
        UnaryExpression(UnaryOp op_, pointer<Expression> ch_)
            : op{op_}, ch{std::move(ch_)} {}
        std::string toString() const override {
          return fmt::format("UnaryExpression {} ({})",
                             AST::Expressions::toString(op), ch->toString());
        }
        json toJSON() const override {
          return {{"type", "unary"},
                  {"op", AST::Expressions::toString(op)},
                  {"ch", ch->toJSON()}};
        }
      };

      class BinaryExpression : public Expression {
      public:
        BinaryOp op;
        pointer<Expression> ch0;
        pointer<Expression> ch1;
        BinaryExpression(BinaryOp op_, pointer<Expression> ch0_,
                         pointer<Expression> ch1_)
            : op{op_}, ch0{std::move(ch0_)}, ch1{std::move(ch1_)} {}
        std::string toString() const override {
          return fmt::format("BinaryExpression {} ({}, {})",
                             AST::Expressions::toString(op), ch0->toString(),
                             ch1->toString());
        }
        json toJSON() const override {
          return {{"type", "binary"},
                  {"op", AST::Expressions::toString(op)},
                  {"ch0", ch0->toJSON()},
                  {"ch1", ch1->toJSON()}};
        }
      };

      class OffsetExpression : public LeftValueExpression {
      public:
        pointer<LeftValueExpression> arr;
        pointer<Expression> offset;
        OffsetExpression(pointer<LeftValueExpression> arr_,
                         pointer<Expression> offset_)
            : arr{std::move(arr_)}, offset{std::move(offset_)} {}
        std::string toString() const override {
          return fmt::format("OffsetExpression ({}, {})", arr->toString(),
                             offset->toString());
        }
        json toJSON() const override {
          return {{"type", "offset"},
                  {"arr", arr->toJSON()},
                  {"offset", offset->toJSON()}};
        }
      };

      class FuncParams : public Node {
      public:
        container<pointer<Expression>> params;
        FuncParams(container<pointer<Expression>> params_)
            : params{std::move(params_)} {}
        FuncParams() = default;
        FuncParams(const FuncParams &) = delete;
        FuncParams(FuncParams &&) = default;
        FuncParams &operator=(const FuncParams &) = delete;
        std::string toString() const override {
          return fmt::format("({})", fmt::join(params, ", "));
        }
        json toJSON() const override { return AST::toJSON(params); }
      };

      class CallExpression : public Expression {
      public:
        pointer<Identifier> func;
        FuncParams args;
        CallExpression(pointer<Identifier> func_, FuncParams args_)
            : func{std::move(func_)}, args{std::move(args_)} {}
        std::string toString() const override {
          return fmt::format("CallExpression {} {}", func->toString(),
                             args.toString());
        }
        json toJSON() const override {
          return {{"type", "call"},
                  {"func", func->toJSON()},
                  {"args", args.toJSON()}};
        }
      };
    } // namespace Expressions
    using namespace Expressions;

    namespace Declarations {
      class OffsetList : public Node {
      public:
        container<pointer<PotentialLiteral>> offsets;
        OffsetList(container<pointer<PotentialLiteral>> offsets_)
            : offsets{std::move(offsets_)} {}
        OffsetList() = default;
        OffsetList(const OffsetList &) = delete;
        OffsetList(OffsetList &&) = default;
        OffsetList &operator=(const OffsetList &) = delete;
        std::string toString() const override {
          return fmt::format("[{}]", fmt::join(offsets, "]["));
        }
        json toJSON() const override { return AST::toJSON(offsets); }
      };

      class SemanticType {
      public:
        std::vector<size_t> dimensions;
        std::vector<size_t> offsets;
        SemanticType(const std::vector<size_t> &dimensions_)
            : dimensions(dimensions_) {
          calculate_offsets();
        }
        void calculate_offsets();
        bool is_scalar() const { return dimensions.size() == 0; }
        bool is_array() const { return !is_scalar(); }
      };

      class Declaration : public BlockItem {
      public:
        std::string name;
        std::optional<SemanticType> type;
        pointer<PotentialLiteral> init_value;
        Declaration(Declaration &&) = default;
        Declaration(const std::string &name_,
                    pointer<PotentialLiteral> init_value_)
            : name(name_), type(std::nullopt),
              init_value(std::move(init_value_)) {}
      };

      class CompileTimeDeclaration : public Declaration {
      public:
        using Declaration::Declaration;
        OffsetList offset_list;
        CompileTimeDeclaration(const std::string &name_,
                               OffsetList offset_list_,
                               pointer<PotentialLiteral> init_value_)
            : Declaration(name_, std::move(init_value_)),
              offset_list(std::move(offset_list_)) {}
        CompileTimeDeclaration(CompileTimeDeclaration &&) = default;
      };

      class ConstDeclaration : public CompileTimeDeclaration {
      public:
        using CompileTimeDeclaration::CompileTimeDeclaration;
        std::string toString() const override {
          return fmt::format("ConstDeclaration {} {} = {}", name,
                             offset_list.toString(), init_value->toString());
        }
        json toJSON() const override {
          return {{"type", "const_dec"},
                  {"name", name},
                  {"type", offset_list.toJSON()},
                  {"init", init_value->toJSON()}};
        }
      };

      class VariableDeclaration : public CompileTimeDeclaration {
      public:
        using CompileTimeDeclaration::CompileTimeDeclaration;
        std::string toString() const override {
          return fmt::format("VariableDeclaration {} {} = {}", name,
                             offset_list.toString(), init_value->toString());
        }
        json toJSON() const override {
          return {{"type", "var_dec"},
                  {"name", name},
                  {"type", offset_list.toJSON()},
                  {"init", init_value->toJSON()}};
        }
      };

      class ParamDeclaration : public CompileTimeDeclaration {
      public:
        using CompileTimeDeclaration::CompileTimeDeclaration;
        std::string toString() const override {
          return fmt::format("ParamDeclaration {} {}", name,
                             offset_list.toString());
        }
        json toJSON() const override {
          return {{"name", name}, {"type", offset_list.toJSON()}};
        }
      };
    } // namespace Declarations
    using namespace Declarations;

    namespace Statements {
      class Statement : public BlockItem {};

      class BreakStmt : public Statement {
      public:
        BreakStmt() = default;
        std::string toString() const override { return "break"; }
        json toJSON() const override { return "break"; }
      };
      class ContinueStmt : public Statement {
      public:
        ContinueStmt() = default;
        std::string toString() const override { return "continue"; }
        json toJSON() const override { return "continue"; }
      };

      class Block : public Statement {
      public:
        container<pointer<BlockItem>> code;
        Block() = default;
        Block(Block &&) = default;
        std::string toString() const override {
          return fmt::format("{{{}}}", fmt::join(code, "; "));
        }
        json toJSON() const override { return AST::toJSON(code); }
      };

      class AssignmentStmt : public Statement {
      public:
        pointer<LeftValueExpression> lhs;
        pointer<Expression> rhs;

        AssignmentStmt(pointer<LeftValueExpression> lhs_,
                       pointer<Expression> rhs_)
            : lhs{std::move(lhs_)}, rhs{std::move(rhs_)} {}
        std::string toString() const override {
          return fmt::format("Assignment {} = {}", lhs->toString(),
                             rhs->toString());
        }
        json toJSON() const override {
          return {{"stmt", "assignment"},
                  {"lhs", lhs->toJSON()},
                  {"rhs", rhs->toJSON()}};
        }
      };

      class ExpressionStmt : public Statement {
      public:
        pointer<Expression> rhs;

        ExpressionStmt(pointer<Expression> rhs_) : rhs{std::move(rhs_)} {}
        std::string toString() const override {
          return fmt::format("ExprStmt {}", rhs->toString());
        }
        json toJSON() const override {
          return {{"stmt", "expr"}, {"rhs", rhs->toJSON()}};
        }
      };

      class ReturnStmt : public Statement {
      public:
        std::optional<pointer<Expression>> rhs;

        ReturnStmt() : rhs{std::nullopt} {}
        ReturnStmt(pointer<Expression> rhs_) : rhs{std::move(rhs_)} {}
        std::string toString() const override {
          if (rhs.has_value()) {
            return fmt::format("Return {}", rhs.value()->toString());
          } else {
            return "Return;";
          }
        }
        json toJSON() const override {
          return {{"stmt", "return"},
                  {"return",
                   rhs.has_value() ? rhs.value()->toJSON() : json(nullptr)}};
        }
      };

      class IfStmt : public Statement {
      public:
        pointer<Expression> cond;
        pointer<Statement> then_clause;
        std::optional<pointer<Statement>> else_clause;

        IfStmt(pointer<Expression> cond_, pointer<Statement> then_clause_,
               pointer<Statement> else_clause_)
            : cond{move(cond_)}, then_clause{move(then_clause_)},
              else_clause{move(else_clause_)} {}

        IfStmt(pointer<Expression> cond_, pointer<Statement> then_clause_)
            : cond{move(cond_)}, then_clause{move(then_clause_)},
              else_clause{std::nullopt} {}

        std::string toString() const override {
          return else_clause.has_value()
                     ? fmt::format("If {} Then {} Else {}", cond->toString(),
                                   then_clause->toString(),
                                   else_clause.value()->toString())
                     : fmt::format("If {} Then {}", cond->toString(),
                                   then_clause->toString());
        }
        json toJSON() const override {
          return {{"stmt", "if"},
                  {"if", cond->toJSON()},
                  {"then", then_clause->toJSON()},
                  {"else", else_clause.has_value()
                               ? else_clause.value()->toJSON()
                               : json(nullptr)}};
        }
      };

      class WhileStmt : public Statement {
      public:
        pointer<Expression> cond;
        pointer<Statement> body;

        WhileStmt(pointer<Expression> cond_, pointer<Statement> body_)
            : cond{move(cond_)}, body{move(body_)} {}

        std::string toString() const override {
          return fmt::format("While {} Do {}", cond->toString(),
                             body->toString());
        }
        json toJSON() const override {
          return {{"stmt", "while"},
                  {"while", cond->toJSON()},
                  {"do", body->toJSON()}};
        }
      };
    } // namespace Statements
    using namespace Statements;

    class Function : public Node {
    public:
      PrimitiveType ret;
      std::string name;
      container<pointer<ParamDeclaration>> params;
      pointer<Block> code;
      Function(PrimitiveType ret_, const std::string &name_,
               decltype(params) params_, decltype(code) code_)
          : ret{ret_}, name{name_}, params{std::move(params_)}, code{move(
                                                                    code_)} {}
      Function(PrimitiveType ret_, const std::string &name_,
               decltype(code) code_)
          : ret{ret_}, name{name_}, params{}, code{move(code_)} {}
      std::string toString() const override {
        return fmt::format("Function {} {} ({}) {{{}}}", SysY::toString(ret),
                           name, fmt::join(params, ", "), code->toString());
      }
      json toJSON() const override {
        return {{"return", SysY::toString(ret)},
                {"name", name},
                {"params", AST::toJSON(params)},
                {"code", code->toJSON()}};
      }
    };

    class CompUnit : public Node {
    public:
      container<pointer<CompileTimeDeclaration>> globals;
      container<pointer<Function>> functions;
      std::string toString() const override {
        return fmt::format("CompUnit\nwith declaration: {}\nwith functions: {}",
                           fmt::join(globals, "\n"),
                           fmt::join(functions, "\n"));
      }
      json toJSON() const override {
        return {{"type", "comp unit"},
                {"globals", AST::toJSON(globals)},
                {"functions", AST::toJSON(functions)}};
      }
    };

    class ArrayLiteral : public PotentialLiteral {
    public:
      container<pointer<PotentialLiteral>> data;
      ArrayLiteral() = default;
      ArrayLiteral(ArrayLiteral &&) = default;
      std::string toString() const override {
        return fmt::format("ArrayLiteral [{}]", fmt::join(data, ", "));
      }
      json toJSON() const override { return AST::toJSON(data); }
    };
  } // namespace AST

  using ASTNode = AST::Node;

  template <typename A, typename = void>
  struct has_toString_overload : std::false_type {};
  template <typename A>
  struct has_toString_overload<A, decltype((void)(std::string(*)(A))(toString))>
      : std::true_type {};

} // namespace SysY

template <typename T, typename = decltype((void)(std::string(*)(T))(
                          SysY::AST::Expressions::toString))>
inline std::ostream &operator<<(std::ostream &os, T t) {
  return os << SysY::AST::Expressions::toString(t);
}

inline std::ostream &operator<<(std::ostream &os, SysY::PrimitiveType t) {
  return os << SysY::toString(t);
}

template <typename T>
struct fmt::formatter<SysY::AST::pointer<T>> : formatter<string_view> {
  // parse is inherited from formatter<string_view>.
  template <typename FormatContext>
  auto format(const SysY::AST::pointer<T> &c, FormatContext &ctx) {
    return formatter<string_view>::format(c->toString(), ctx);
  }
};
