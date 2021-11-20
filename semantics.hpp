#pragma once

#include <string>
#include <vector>
#include <type_traits>
#include <optional>
#include "fmt/format.h"

namespace SysY {
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

    using literal_type = int;
    class Node {
    public:
      virtual std::string toString() = 0;
    };

    class Expression : public Node {};
    class LeftValueExpression : public Expression {};

    class Identifier : public LeftValueExpression {
    public:
      std::string name;
      Identifier(std::string_view name_) : name{name_} {}
      std::string toString() override {
        return fmt::format("Identifier {}", name);
      }
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
      std::string toString() override {
        return fmt::format("LiteralExpression ({})", val);
      }
    };

    class UnaryExpression : public Expression {
    public:
      UnaryOp op;
      pointer<Expression> ch;
      UnaryExpression(UnaryOp op_, pointer<Expression> ch_)
          : op{op_}, ch{std::move(ch_)} {}
      std::string toString() override {
        return fmt::format("UnaryExpression {} ({})", AST::toString(op),
                           ch->toString());
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
      std::string toString() override {
        return fmt::format("BinaryExpression {} ({}, {})", AST::toString(op),
                           ch0->toString(), ch1->toString());
      }
    };

    class OffsetExpression : public LeftValueExpression {
    public:
      pointer<LeftValueExpression> arr;
      pointer<Expression> offset;
      OffsetExpression(pointer<LeftValueExpression> arr_,
                       pointer<Expression> offset_)
          : arr{std::move(arr_)}, offset{std::move(offset_)} {}
      std::string toString() override {
        return fmt::format("OffsetExpression ({}, {})", arr->toString(),
                           offset->toString());
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
      std::string toString() override {
        return fmt::format("({})", fmt::join(params, ", "));
      }
    };

    class OffsetList : public Node {
    public:
      container<pointer<Expression>> offsets;
      OffsetList(container<pointer<Expression>> offsets_)
          : offsets{std::move(offsets_)} {}
      OffsetList() = default;
      OffsetList(const OffsetList &) = delete;
      OffsetList(OffsetList &&) = default;
      OffsetList &operator=(const OffsetList &) = delete;
      std::string toString() override {
        return fmt::format("[{}]", fmt::join(offsets, "]["));
      }
    };

    class CallExpression : public Expression {
    public:
      pointer<Identifier> func;
      FuncParams args;
      CallExpression(pointer<Identifier> func_, FuncParams args_)
          : func{std::move(func_)}, args{std::move(args_)} {}
      std::string toString() override {
        return fmt::format("CallExpression {} {}", func->toString(),
                           args.toString());
      }
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

    class Declaration : public Node {
    public:
      std::string name;
      std::optional<SemanticType> type;
      Declaration(Declaration &&) = default;
      Declaration(const std::string &name_) : name(name_), type(std::nullopt) {}
    };

    class CompileTimeDeclaration : public Declaration {
    public:
      using Declaration::Declaration;
      OffsetList offset_list;
      CompileTimeDeclaration(CompileTimeDeclaration &&) = default;
    };

    class ConstDeclaration : public CompileTimeDeclaration {
      using CompileTimeDeclaration::CompileTimeDeclaration;
    };

    class VariableDeclaration : public CompileTimeDeclaration {
      using CompileTimeDeclaration::CompileTimeDeclaration;
    };

    class CompUnit : public Node {
    public:
      container<pointer<ConstDeclaration>> global_consts;
      container<pointer<VariableDeclaration>> global_vars;
      std::string toString() override {
        return fmt::format("CompUnit\nwith const declaration: {}\nwith "
                           "variable declaration:{}\n",
                           fmt::join(global_consts, "\n"),
                           fmt::join(global_vars, "\n"));
      }
    };
  } // namespace AST

  using ASTNode = AST::Node;

  template <typename A, typename = void>
  struct has_toString_overload : std::false_type {};
  template <typename A>
  struct has_toString_overload<A, decltype((void)(std::string(*)(A))(toString))>
      : std::true_type {};

} // namespace SysY

template <typename T,
          typename = decltype((void)(std::string(*)(T))(SysY::AST::toString))>
inline std::ostream &operator<<(std::ostream &os, T t) {
  return os << SysY::AST::toString(t);
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
