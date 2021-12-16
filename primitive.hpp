#pragma once
#include <functional>
#include <string>

namespace SysY {
  enum class PrimitiveType { INT, VOID };

  enum class UnaryOp { Plus, Minus, Not };

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

  std::string toString(PrimitiveType t);
  std::string toString(UnaryOp t);
  std::string toString(BinaryOp t);
  std::function<int(int)> toOP(UnaryOp t);
  std::function<int(int, int)> toOP(BinaryOp t);
} // namespace SysY

template <typename T,
          typename = decltype((void)(std::string(*)(T))(SysY::toString))>
inline std::ostream &operator<<(std::ostream &os, T t) {
  return os << SysY::toString(t);
}
