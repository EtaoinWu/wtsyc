#include "typec.hpp"
#include "semantics.hpp"
#include "3rd-party/fmt/core.h"
#include "3rd-party/mpark/patterns.hpp"
#include "array_align.hpp"
#include "error.hpp"
#include "levelmap.hpp"
#include "util.hpp"
#include "primitive.hpp"
#include <functional>
#include <unordered_set>

namespace SysY {

  namespace Pass {
    using namespace mpark::patterns;
    LowLevelSymbolInfo::Category LowLevelSymbolInfo::category_of(const AST::Node *node) {
      if (dynamic_cast<const AST::Function *>(node) != nullptr) {
        return function;
      } else if (dynamic_cast<const AST::ParamDeclaration *>(node) != nullptr) {
        return parameter;
      } else if (dynamic_cast<const AST::CompileTimeDeclaration *>(node) !=
                 nullptr) {
        return variable;
      } else if (dynamic_cast<const AST::Expression *>(node) != nullptr) {
        return temporary;
      } else {
        throw Exception::BadAST(
            fmt::format("bad {}: {}", __FUNCTION__, node->toJSON().dump()));
      }
    }

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

    struct EvalPureResult {
      int data;
    };
    struct EvalOffsetResult {
      rc_ptr<AST::ConstDeclaration> data;
      int pos;
      int offset;
    };
    using eval_t = std::variant<EvalPureResult, EvalOffsetResult>;

    eval_t k_eval(AST::pointer<AST::Expression> exp, environment_t env);

    // Helper function for k_eval
    eval_t clean_up(const eval_t &val, environment_t env) {
      return std::visit(
          util::overloaded{
              [](const EvalPureResult &val) -> eval_t { return val; },
              [&env](const EvalOffsetResult &val) -> eval_t {
                if ((unsigned) val.pos == val.data->offset_list.offsets.size()) {
                  const auto &z = val.data->aligned_init;
                  auto it = std::lower_bound(
                      z.begin(), z.end(), val.offset,
                      [](const auto &x, const auto &y) { return x.offset < y; });
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

    // Const evaluate an expression w/ environment
    eval_t k_eval(AST::pointer<AST::Expression> exp, environment_t env) {
      if (auto x = std::dynamic_pointer_cast<AST::LiteralExpression>(exp)) {
        return EvalPureResult{x->val};
      } else if (auto x = std::dynamic_pointer_cast<AST::Identifier>(exp)) {
        const auto &name = x->name;
        if (env->symbols->count(name)) {
          auto ptr = env->symbols->at(name).ptr;
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
        return EvalPureResult{toOP(x->op)(ch)};
      } else if (auto x =
                     std::dynamic_pointer_cast<AST::BinaryExpression>(exp)) {
        AST::literal_type ch0 =
            std::get<EvalPureResult>(k_eval(x->ch0, env)).data;
        AST::literal_type ch1 =
            std::get<EvalPureResult>(k_eval(x->ch1, env)).data;
        return EvalPureResult{toOP(x->op)(ch0, ch1)};
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
    calculate_dimensions(const AST::OffsetList &offset_list, environment_t env) {
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

    void typecheck(const AST::pointer<AST::Node> &x, environment_t env) {
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
      } else if (const auto func =
                     std::dynamic_pointer_cast<AST::Function>(x)) {
        for (const auto &param : func->params) {
          typecheck(param, env);
        }
        typecheck(func->code, env->clone());
      } else if (const auto blk = std::dynamic_pointer_cast<AST::Block>(x)) {
        blk->env = env;
        for (const auto &item : blk->code) {
          if (auto dec = std::dynamic_pointer_cast<AST::CompileTimeDeclaration>(item)) {
            typecheck(dec, env);
          } else if (auto blk_ = std::dynamic_pointer_cast<AST::Block>(item)) {
            typecheck(blk_, env->clone());
          } else if (auto stmt = std::dynamic_pointer_cast<AST::Statement>(item)) {
            // non-block statement
            typecheck(stmt, env);
          }
        }
      } else if (const auto stmt_if = std::dynamic_pointer_cast<AST::IfStmt>(x)) {
        typecheck(stmt_if->then_clause, env);
        if (stmt_if->else_clause.has_value()) {
          typecheck(stmt_if->else_clause.value(), env);
        }
      } else if (const auto stmt_while = std::dynamic_pointer_cast<AST::WhileStmt>(x)) {
        typecheck(stmt_while->body, env);
      } else if (const auto cu = std::dynamic_pointer_cast<AST::CompUnit>(x)) {
        cu->env = env;
        for (const auto &dec : cu->globals) {
          typecheck(dec, env);
          auto cat = LowLevelSymbolInfo::category_of(dec.get());
          env->symbols->insert(
              dec->name, LowLevelSymbolInfo{cat, env->get_count(cat), dec->length(), dec});
        }
        for (const auto &func : cu->functions) {
          typecheck(func, env->clone());
          auto cat = LowLevelSymbolInfo::category_of(func.get());
          env->symbols->insert(
              func->name, LowLevelSymbolInfo{cat, env->get_count(cat), -1, func});
        }
      }
    }
  } // namespace Pass
} // namespace SysY