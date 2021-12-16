#include "primitive.hpp"
#include "error.hpp"

namespace SysY {
  std::string toString(PrimitiveType t) {
    switch (t) {
    case PrimitiveType::INT:
      return "int";
    case PrimitiveType::VOID:
      return "void";
    default:
      return "";
    }
  }

  std::string toString(UnaryOp t) {
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

  std::string toString(BinaryOp t) {
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

  std::function<int(int)> toOP(UnaryOp t) {
    switch (t) {
    case UnaryOp::Plus:
      return [](int x) -> int { return +x; };
    case UnaryOp::Minus:
      return [](int x) -> int { return -x; };
    case UnaryOp::Not:
      return [](int x) -> int { return !x; };
    default:
      throw Exception::BadOP("");
    }
  }
  std::function<int(int, int)> toOP(BinaryOp t) {
    switch (t) {
    case BinaryOp::Plus:
      return [](int x, int y) -> int { return x + y; };
    case BinaryOp::Minus:
      return [](int x, int y) -> int { return x - y; };
    case BinaryOp::Mult:
      return [](int x, int y) -> int { return x * y; };
    case BinaryOp::Div:
      return [](int x, int y) -> int { return x / y; };
    case BinaryOp::Mod:
      return [](int x, int y) -> int { return x % y; };
    case BinaryOp::LT:
      return [](int x, int y) -> int { return x < y; };
    case BinaryOp::GT:
      return [](int x, int y) -> int { return x > y; };
    case BinaryOp::LE:
      return [](int x, int y) -> int { return x <= y; };
    case BinaryOp::GE:
      return [](int x, int y) -> int { return x >= y; };
    case BinaryOp::NE:
      return [](int x, int y) -> int { return x != y; };
    case BinaryOp::EQ:
      return [](int x, int y) -> int { return x == y; };
    case BinaryOp::And:
      return [](int x, int y) -> int { return x && y; };
    case BinaryOp::Or:
      return [](int x, int y) -> int { return x || y; };
    default:
      throw Exception::BadOP("");
    }
  }
} // namespace SysY