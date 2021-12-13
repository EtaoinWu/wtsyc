#pragma once
#include "3rd-party/json/json.hpp"
#include "levelmap.hpp"
#include "semantics.hpp"
#include <optional>
#include <variant>

namespace SysY {
  namespace Pass {
    template <typename T> using rc_ptr = std::shared_ptr<T>;
    struct SymbolInfo {
      rc_ptr<AST::Node> ptr;
    };

    struct EvalPureResult {
      int data;
    };
    struct EvalOffsetResult {
      rc_ptr<AST::ConstDeclaration> data;
      int pos;
      int offset;
    };
    using eval_t = std::variant<EvalPureResult, EvalOffsetResult>;

    class Environment {
    public:
      using symbolinfo_t = SymbolInfo;
      using symbol_map_t = Utility::levelmap<std::string, symbolinfo_t>;
      rc_ptr<symbol_map_t> symbols = std::make_shared<symbol_map_t>();

      Environment clone() {
        return Environment{symbols->clone()};
      }
    };

    void verify(const AST::pointer<AST::CompUnit> &cu);
    void typecheck(AST::pointer<AST::Node> &x, Environment env = {});
  } // namespace Pass
} // namespace SysY