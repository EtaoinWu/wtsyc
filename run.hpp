#pragma once
#include "3rd-party/json/json.hpp"
#include "levelmap.hpp"
#include <optional>
#include <variant>

namespace SysY {
  namespace AST {
    template <typename T> using pointer = std::shared_ptr<T>;
    class Node;
    class CompUnit;
  }

  namespace Pass {
    template <typename T> using rc_ptr = std::shared_ptr<T>;
    struct SymbolInfo {
      enum Category {
        undetermined, variable, temporary, parameter, function, n_categories
      } category;
      static Category category_of(const AST::Node *node);
      int count;
      int length;
      rc_ptr<AST::Node> ptr;
    };

    class Environment {
    public:
      using symbolinfo_t = SymbolInfo;
      using symbol_map_t = Utility::levelmap<std::string, symbolinfo_t>;
      
      rc_ptr<symbol_map_t> symbols = std::make_shared<symbol_map_t>();
      int count[SymbolInfo::n_categories] = {};

      Environment clone() const {
        auto x = Environment{*this};
        x.symbols = x.symbols->clone();
        return x;
      }
    };

    void verify(const AST::pointer<AST::CompUnit> &cu);
    void typecheck(const AST::pointer<AST::Node> &x, Environment env = {});
  } // namespace Pass
} // namespace SysY