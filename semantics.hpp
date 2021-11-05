#pragma once 

#include <string>

namespace SysY
{
  enum class Type {
    INT, VOID
  };

  inline std::string toString(Type t) {
    switch (t) {
    case Type::INT:
      return "int";
    case Type::VOID:
      return "void";
    default:
      return "";
    }
  }

  class ASTNode {
  public:
    int value;
    ASTNode(int value_):value(value_) {}
    virtual std::string showRaw() {
      return "";
    }
  };
} // namespace SysY

inline std::ostream &operator<<(std::ostream &os, SysY::Type t)  {
  return os << SysY::toString(t);
}
