#include "typec.hpp"
#include "array_align.hpp"
#include "dynamic_ast.hpp"
#include "error.hpp"
#include "levelmap.hpp"
#include "primitive.hpp"
#include "semantics.hpp"
#include "util.hpp"
#include <fmt/core.h>
#include <functional>
#include <mpark/patterns.hpp>
#include <unordered_set>

namespace SysY {

  namespace Pass {
    using namespace mpark::patterns;

    struct EvalPureResult {
      int data;
    };
    struct EvalOffsetResult {
      rc_ptr<AST::ConstDeclaration> data;
      int pos;
      int offset;
    };
    using eval_t = std::variant<EvalPureResult, EvalOffsetResult>;

    LowLevelSymbolInfo::Category
    LowLevelSymbolInfo::category_of(const AST::Node *node) {
      if (dynamic_cast<const AST::Callable *>(node) != nullptr) {
        return function;
      } else if (dynamic_cast<const AST::ParamDeclaration *>(node) != nullptr) {
        return parameter;
      } else if (
        dynamic_cast<const AST::CompileTimeDeclaration *>(node) != nullptr) {
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

    eval_t k_eval(AST::pointer<AST::Expression> exp, environment_t env);

    // Helper function for k_eval
    eval_t clean_up(const eval_t &val, environment_t env) {
      return std::visit(
        overloaded{
          [](const EvalPureResult &val) -> eval_t { return val; },
          [&env](const EvalOffsetResult &val) -> eval_t {
            if ((unsigned)val.pos == val.data->offset_list.offsets.size()) {
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
      } else if (
        auto x = std::dynamic_pointer_cast<AST::UnaryExpression>(exp)) {
        AST::literal_type ch =
          std::get<EvalPureResult>(k_eval(x->ch, env)).data;
        return EvalPureResult{toOP(x->op)(ch)};
      } else if (
        auto x = std::dynamic_pointer_cast<AST::BinaryExpression>(exp)) {
        if (x->op == BinaryOp::And) {
          AST::literal_type ch0 =
            std::get<EvalPureResult>(k_eval(x->ch0, env)).data;
          return EvalPureResult{
            ch0 && std::get<EvalPureResult>(k_eval(x->ch1, env)).data};
        } else if (x->op == BinaryOp::Or) {
          AST::literal_type ch0 =
            std::get<EvalPureResult>(k_eval(x->ch0, env)).data;
          return EvalPureResult{
            ch0 || std::get<EvalPureResult>(k_eval(x->ch1, env)).data};
        } else {
          AST::literal_type ch0 =
            std::get<EvalPureResult>(k_eval(x->ch0, env)).data;
          AST::literal_type ch1 =
            std::get<EvalPureResult>(k_eval(x->ch1, env)).data;
          return EvalPureResult{toOP(x->op)(ch0, ch1)};
        }
      } else if (
        auto x = std::dynamic_pointer_cast<AST::OffsetExpression>(exp)) {
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

    AST::container<AST::literal_type> calculate_dimensions(
      const AST::OffsetList &offset_list, environment_t env) {
      AST::container<AST::literal_type> dimensions;
      for (const auto &offset : offset_list.offsets) {
        if (std::dynamic_pointer_cast<AST::NoneLiteral>(offset)) {
          dimensions.push_back(0);
        } else if (
          auto exp = std::dynamic_pointer_cast<AST::Expression>(offset)) {
          dimensions.push_back(std::get<EvalPureResult>(k_eval(exp, env)).data);
        } else {
          throw Exception::BadAST(
            fmt::format("bad array dimension {}", offset->toJSON().dump()));
        }
      }
      return dimensions;
    }

    AST::pointer<AST::Expression>
    disambiguate(const AST::pointer<AST::Expression> &exp, environment_t env) {
      if (auto x = std::dynamic_pointer_cast<AST::Identifier>(exp)) {
        auto z =
          std::make_shared<AST::Symbol>(x->name, env->symbols->at(x->name));
        z->range = x->range;
        return z;
      } else if (
        auto x = std::dynamic_pointer_cast<AST::UnaryExpression>(exp)) {
        x->ch = disambiguate(x->ch, env);
        return x;
      } else if (
        auto x = std::dynamic_pointer_cast<AST::BinaryExpression>(exp)) {
        x->ch0 = disambiguate(x->ch0, env);
        x->ch1 = disambiguate(x->ch1, env);
        return x;
      } else if (
        auto x = std::dynamic_pointer_cast<AST::OffsetExpression>(exp)) {
        x->arr = std::dynamic_pointer_cast<AST::LeftValueExpression>(
          disambiguate(x->arr, env));
        assert(x->arr != nullptr);
        x->offset = disambiguate(x->offset, env);
        return x;
      } else if (auto x = std::dynamic_pointer_cast<AST::CallExpression>(exp)) {
        x->func = std::dynamic_pointer_cast<AST::Identifier>(
          disambiguate(x->func, env));
        assert(x->func != nullptr);
        for (auto &param : x->args.params) {
          param = disambiguate(param, env);
        }
        return x;
      } else {
        return exp;
      }
    }

    void typecheck(const AST::pointer<AST::Node> &x, environment_t env) {
      if (
        const auto dec =
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
            dec->aligned_init =
              ArrayAlign::array_align_expand(ptr, dec->type.value());
          }
        }

        for (auto &alignment : dec->aligned_init) {
          alignment.exp = disambiguate(alignment.exp, env);
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

      } else if (
        const auto func = std::dynamic_pointer_cast<AST::Function>(x)) {
        env = env->clone();
        int param_cnt = 0;
        for (const auto &param : func->params) {
          typecheck(param, env);

          auto cat = LowLevelSymbolInfo::category_of(param.get());
          env->symbols->insert(
            param->name,
            LowLevelSymbolInfo{cat, param_cnt++, param->length(), param});
        }
        func->env = env;
        typecheck(func->code, env);

      } else if (const auto blk = std::dynamic_pointer_cast<AST::Block>(x)) {
        env = env->clone();
        blk->env = env;
        for (const auto &item : blk->code) {
          typecheck(item, env);
          if (
            auto dec =
              std::dynamic_pointer_cast<AST::CompileTimeDeclaration>(item)) {
            auto cat = LowLevelSymbolInfo::category_of(dec.get());
            env->symbols->insert(
              dec->name, LowLevelSymbolInfo{
                           cat,
                           env->get_count(cat),
                           dec->length(),
                           dec,
                         });
          }
        }

      } else if (
        const auto assign = std::dynamic_pointer_cast<AST::AssignmentStmt>(x)) {
        assign->lhs = std::dynamic_pointer_cast<AST::LeftValueExpression>(
          disambiguate(assign->lhs, env));
        assert(assign->lhs);
        assign->rhs = disambiguate(assign->rhs, env);

      } else if (
        const auto expr_stmt =
          std::dynamic_pointer_cast<AST::ExpressionStmt>(x)) {
        expr_stmt->rhs = disambiguate(expr_stmt->rhs, env);

      } else if (
        const auto return_stmt =
          std::dynamic_pointer_cast<AST::ReturnStmt>(x)) {
        if (return_stmt->rhs.has_value()) {
          return_stmt->rhs = disambiguate(return_stmt->rhs.value(), env);
        }

      } else if (
        const auto stmt_if = std::dynamic_pointer_cast<AST::IfStmt>(x)) {
        stmt_if->cond = disambiguate(stmt_if->cond, env);
        typecheck(stmt_if->then_clause, env);
        if (stmt_if->else_clause.has_value()) {
          typecheck(stmt_if->else_clause.value(), env);
        }

      } else if (
        const auto stmt_while = std::dynamic_pointer_cast<AST::WhileStmt>(x)) {
        stmt_while->cond = disambiguate(stmt_while->cond, env);
        typecheck(stmt_while->body, env);

      } else if (const auto cu = std::dynamic_pointer_cast<AST::CompUnit>(x)) {
        cu->env = env;
        for (const auto &dec : cu->globals) {
          typecheck(dec, env);
          auto cat = LowLevelSymbolInfo::category_of(dec.get());
          env->symbols->insert(
            dec->name, LowLevelSymbolInfo{
                         cat,
                         env->get_count(cat),
                         dec->length(),
                         dec,
                       });
        }
        for (const auto &func : cu->functions) {
          auto cat = LowLevelSymbolInfo::category_of(func.get());
          env->symbols->insert(
            func->name, LowLevelSymbolInfo{
                          cat,
                          env->get_count(cat),
                          -1,
                          func,
                        });
          typecheck(func, env);
        }
      }
    }
  } // namespace Pass
} // namespace SysY