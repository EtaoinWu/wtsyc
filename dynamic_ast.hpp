#pragma once
#include "semantics.hpp"
#include "typec.hpp"

namespace SysY {
  namespace AST {
    class Symbol : public Identifier {
    public:
      Pass::LowLevelSymbolInfo info;
      Symbol(std::string name_, Pass::LowLevelSymbolInfo info_)
          : Identifier(name_), info{info_} {}
      std::string toString() const override;
      json toJSON() const override { return name; }
    };
  } // namespace AST
} // namespace SysY