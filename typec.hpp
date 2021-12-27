#pragma once
#include "levelmap.hpp"
#include <fpp/fmap.h>
#include <fpp/fstring.h>
#include <json/json.hpp>
#include <optional>
#include <variant>

namespace SysY {
  class Lexer;

  namespace AST {
    template <typename T> using pointer = std::shared_ptr<T>;
    class Node;
    class CompUnit;
  } // namespace AST

  namespace Pass {
    template <typename T> using rc_ptr = std::shared_ptr<T>;
    struct LowLevelSymbolInfo {
      enum Category {
        undetermined,
        variable,
        temporary,
        parameter,
        function,
        n_categories
      } category;

      // For eeyore
      int count;

      // length == 0: scalar
      int length;
      rc_ptr<AST::Node> ptr;

      static Category category_of(const AST::Node *node);
    };

    class Environment;
    using environment_t = rc_ptr<Environment>;

    class Environment {
    public:
      using symbolinfo_t = LowLevelSymbolInfo;
      using symbol_map_t = Utility::levelmap<std::string, symbolinfo_t>;

      const Lexer *lexer;

      rc_ptr<symbol_map_t> symbols = std::make_shared<symbol_map_t>();
      rc_ptr<int> variable_count = std::make_shared<int>(0);
      rc_ptr<int> temp_count = std::make_shared<int>(0);
      rc_ptr<int> label_count = std::make_shared<int>(0);

      explicit Environment(const Lexer *lexer_) : lexer{lexer_} {}
      Environment(const Environment &) = default;

      environment_t clone() const {
        auto x = std::make_shared<Environment>(*this);
        x->symbols = x->symbols->clone();
        return x;
      }

      int get_count(LowLevelSymbolInfo::Category cat) {
        if (cat == LowLevelSymbolInfo::variable) {
          return (*variable_count)++;
        } else if (cat == LowLevelSymbolInfo::temporary) {
          return (*temp_count)++;
        } else {
          return 0;
        }
      }

      int new_label() { return (*label_count)++; }
    }; // namespace Pass

    void verify(const AST::pointer<AST::CompUnit> &cu);
    void typecheck(const AST::pointer<AST::Node> &x, environment_t env);
  } // namespace Pass
} // namespace SysY