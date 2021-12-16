#pragma once
#include "3rd-party/fpp/fmap.h"
#include "3rd-party/fpp/fstring.h"
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
    struct LowLevelSymbolInfo {
      enum Category {
        undetermined, variable, temporary, parameter, function, n_categories
      } category;
      static Category category_of(const AST::Node *node);
      int count;
      int length;
      rc_ptr<AST::Node> ptr;
    };

    class Environment;
    using environment_t = rc_ptr<Environment>;

    class Environment {
    public:
      using symbolinfo_t = LowLevelSymbolInfo;
      using symbol_map_t = Utility::levelmap<std::string, symbolinfo_t>;
      
      rc_ptr<symbol_map_t> symbols = std::make_shared<symbol_map_t>();
      rc_ptr<int> variable_count = std::make_shared<int>(0);

      environment_t clone() const {
        auto x = std::make_shared<Environment>(*this);
        x->symbols = x->symbols->clone();
        return x;
      }

      environment_t strong_clone() const {
        auto x = clone();
        x->variable_count = std::make_shared<int>(0);
        return x;
      }

      int get_count(LowLevelSymbolInfo::Category cat) {
        if (cat == LowLevelSymbolInfo::variable) {
          return (*variable_count)++;
        } else {
          return 0;
        }
      }
    };

    void verify(const AST::pointer<AST::CompUnit> &cu);
    void typecheck(const AST::pointer<AST::Node> &x, environment_t env);
  } // namespace Pass
} // namespace SysY