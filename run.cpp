#include "run.hpp"
#include "3rd-party/fmt/core.h"
#include "3rd-party/mpark/patterns.hpp"
#include "array_align.hpp"
#include "error.hpp"
#include "levelmap.hpp"
#include "util.hpp"
#include <functional>
#include <unordered_set>

namespace SysY {
  namespace AST {
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
  } // namespace AST

  namespace Pass {
    using namespace mpark::patterns;

    void distinct_globals(const AST::pointer<AST::CompUnit> &cu) {
      std::unordered_set<std::string> global_names;
      for (const auto &x : cu->functions) {
        // if the name exists ...
        if (!global_names.insert(x->name).second) {
          throw Exception::VerificationError(
              fmt::format("Duplicate global identifier {}", x->name));
        }
      }
      for (const auto &x : cu->globals) {
        // if the name exists ...
        if (!global_names.insert(x->name).second) {
          throw Exception::VerificationError(
              fmt::format("Duplicate global identifier {}", x->name));
        }
      }
      if (global_names.count("main") == 0) {
        throw Exception::VerificationError("main() does not exist");
      }
    }

    eval_t clean_up(const eval_t &val, Environment env) {
      return std::visit(
          util::overloaded{
              [](const EvalPureResult &val) -> eval_t { return val; },
              [&env](const EvalOffsetResult &val) -> eval_t {
                if (val.pos == val.data->offset_list.offsets.size()) {
                  const auto &z = val.data->aligned_init;
                  auto it = std::lower_bound(
                      z.begin(), z.end(), val.offset,
                      [](const auto &x, const auto &y) { return x.offset < y });
                  if (it == z.end() || it->offset != val.offset) {
                    return EvalPureResult{0};
                  }
                  return k_eval(it->exp, env);
                } else {
                  return val;
                }
              }},
          val);
    }

    eval_t k_eval(AST::pointer<AST::Expression> exp, Environment env) {
      if (auto x = std::dynamic_pointer_cast<AST::LiteralExpression>(exp)) {
        return EvalPureResult{x->val};
      } else if (auto x = std::dynamic_pointer_cast<AST::Identifier>(exp)) {
        const auto &name = x->name;
        if (env.symbols->count(name)) {
          auto ptr = env.symbols->at(name).ptr;
          if (auto k = std::dynamic_pointer_cast<AST::ConstDeclaration>(ptr)) {
            return clean_up(EvalOffsetResult{k, 0, 0}, env);
          }
        }
        throw Exception::BadName(
            fmt::format("cannot find identifier {}", name));
      } else if (auto x =
                     std::dynamic_pointer_cast<AST::UnaryExpression>(exp)) {
        AST::literal_type ch =
            std::get<EvalPureResult>(k_eval(x->ch, env)).data;
        return EvalPureResult{AST::toOP(x->op)(ch)};
      } else if (auto x =
                     std::dynamic_pointer_cast<AST::BinaryExpression>(exp)) {
        AST::literal_type ch0 =
            std::get<EvalPureResult>(k_eval(x->ch0, env)).data;
        AST::literal_type ch1 =
            std::get<EvalPureResult>(k_eval(x->ch1, env)).data;
        return EvalPureResult{AST::toOP(x->op)(ch0, ch1)};
      } else if (auto x =
                     std::dynamic_pointer_cast<AST::OffsetExpression>(exp)) {
        AST::literal_type offset =
            std::get<EvalPureResult>(k_eval(x->offset, env)).data;
        auto arr = std::get<EvalOffsetResult>(k_eval(x->arr, env));
        return clean_up(
            EvalOffsetResult{
                arr.data, arr.pos + 1,
                arr.offset + arr.data->type.value().offsets[arr.pos] * offset},
            env);
      } else {
        throw Exception::BadAST(
            fmt::format("Bad const expression {}", exp->toJSON().dump()));
      }
    }

    void verify(const AST::pointer<AST::CompUnit> &cu) { distinct_globals(cu); }

    AST::container<AST::literal_type>
    calculate_dimensions(const AST::OffsetList &offset_list, Environment env) {
      AST::container<AST::literal_type> dimensions;
      for (const auto &offset : offset_list.offsets) {
        if (std::dynamic_pointer_cast<AST::NoneLiteral>(offset)) {
          dimensions.push_back(0);
        } else if (auto exp =
                       std::dynamic_pointer_cast<AST::Expression>(offset)) {
          dimensions.push_back(std::get<EvalPureResult>(k_eval(exp, env)).data);
        } else {
          throw Exception::BadAST(
              fmt::format("bad array dimension {}", offset->toJSON().dump()));
        }
      }
      return dimensions;
    }

    void typecheck(AST::pointer<AST::Node> x, Environment env) {
      if (const auto dec =
              std::dynamic_pointer_cast<AST::CompileTimeDeclaration>(x)) {
        dec->type = SemanticType(calculate_dimensions(dec->offset_list, env));

        // Set the proper initialization data
        if (dec->type.value().is_scalar()) {
          auto ptr =
              std::dynamic_pointer_cast<AST::Expression>(dec->init_value);
          if (ptr != nullptr) {
            dec->aligned_init = {ArrayAlign::Alignment{0, ptr}};
          }
        } else {
          auto ptr =
              std::dynamic_pointer_cast<AST::ArrayLiteral>(dec->init_value);
          if (ptr != nullptr) {
            dec->aligned_init = ArrayAlign::array_align(ptr, dec->type.value());
          }
        }

        if (auto kdec = std::dynamic_pointer_cast<AST::ConstDeclaration>(dec)) {
          // dec is a const
          if (kdec->aligned_init.empty()) {
            throw Exception::BadAST(
                fmt::format("bad const init {}", kdec->toJSON().dump()));
          }
          // memorize all initialization value
          for (auto &init_item : kdec->aligned_init) {
            init_item.exp = std::make_shared<AST::LiteralExpression>(
                std::get<EvalPureResult>(k_eval(init_item.exp, env)).data);
          }
        }
      } else if (const auto func = std::dynamic_pointer_cast<AST::Function>(x)) {
        for (const auto &param : func->params) {
          typecheck(param, env);
        }
        typecheck(func->code, env.clone());
      } else if (const auto func = std::dynamic_pointer_cast<AST::Block>(x)) {
        
      } else if (const auto cu = std::dynamic_pointer_cast<AST::CompUnit>(x)) {
        for (const auto &dec : cu->globals) {
          typecheck(dec, env);
          env.symbols->insert(dec->name, SymbolInfo{dec});
        }
        for (const auto &func : cu->functions) {
          typecheck(func, env);
        }
      }
    }
  } // namespace Pass
} // namespace SysY