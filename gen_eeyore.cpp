#include "gen_eeyore.hpp"
#include "error.hpp"
#include "lexer.hpp"
#include "util.hpp"
#include <fmt/core.h>
#include <iterator>
#include <unordered_set>
#include <utility>

namespace SysY::Pass {
  const std::string sysy_macro_prefix = "_sysy_";

  using f_code = std::vector<Eeyore::Statement>;
  using f_symtab = std::vector<LowLevelSymbolInfo>;
  using f_dec = std::vector<Eeyore::DeclarationX>;
  using expr_type = std::tuple<f_code, Eeyore::VariableI, SemanticType>;
  using stmt_result = std::tuple<f_code, f_symtab>;
  using dec_result = std::tuple<f_code, LowLevelSymbolInfo>;

  template <typename T>
  std::vector<T> &merge_into(std::vector<T> &a, const std::vector<T> &b) {
    a.insert(a.end(), b.begin(), b.end());
    return a;
  }

  template <typename T>
  std::vector<T> &merge_into(std::vector<T> &a, const T &b) {
    a.push_back(b);
    return a;
  }

  struct do_in_order {
    template <typename T> do_in_order(std::initializer_list<T> &&) {}
  };

  namespace details {
    template <typename V> void concat_helper(V &l, const V &r) {
      l.insert(l.end(), r.begin(), r.end());
    }
    template <class V> void concat_helper(V &l, V &&r) {
      l.insert(
        l.end(), std::make_move_iterator(r.begin()),
        std::make_move_iterator(r.end()));
    }
  } // namespace details

  template <typename T, typename... A>
  std::vector<T> concat(std::vector<T> v1, A &&... vr) {
    std::size_t s = v1.size();
    do_in_order{s += vr.size()...};
    v1.reserve(s);
    do_in_order{(details::concat_helper(v1, std::forward<A>(vr)), 0)...};
    return std::move(v1); // rvo blocked
  }

  Eeyore::VCategory category_cast(LowLevelSymbolInfo::Category cat) {
    switch (cat) {
    case LowLevelSymbolInfo::temporary:
      return Eeyore::VCategory::temp;
    case LowLevelSymbolInfo::parameter:
      return Eeyore::VCategory::param;
    case LowLevelSymbolInfo::variable:
      return Eeyore::VCategory::var;
    default:
      throw Exception::BadAST(fmt::format(
        "{}: bad category {} in category_cast", __func__, to_underlying(cat)));
    }
  }

  Eeyore::VariableI symbol_ref(const LowLevelSymbolInfo &info) {
    return Eeyore::VariableI{category_cast(info.category), info.count};
  }

  Eeyore::VariableI new_temp(environment_t env) {
    return Eeyore::VariableI{
      Eeyore::VCategory::temp, env->get_count(LowLevelSymbolInfo::temporary)};
  }

  Eeyore::VariableI copy_temp(Eeyore::VariableI var, environment_t env) {
    if (var.cat == Eeyore::VCategory::temp) {
      return var;
    } else {
      return new_temp(env);
    }
  }

  Eeyore::DeclarationX declaration_cast(const LowLevelSymbolInfo &info) {
    if (info.length > 0) {
      return Eeyore::DeclarationA{symbol_ref(info), info.length * 4};
    } else {
      return Eeyore::DeclarationS{symbol_ref(info)};
    }
  }

  dec_result
  generate_dec(rc_ptr<AST::CompileTimeDeclaration> dec, environment_t env);
  expr_type generate_expr(rc_ptr<AST::Expression> exp, environment_t env);
  stmt_result generate_stmt(
    rc_ptr<AST::Statement> stmt, environment_t env,
    std::optional<Eeyore::Label> cont_pos = std::nullopt,
    std::optional<Eeyore::Label> break_pos = std::nullopt);

  dec_result
  generate_dec(rc_ptr<AST::CompileTimeDeclaration> dec, environment_t env) {
    auto info = env->symbols->at(dec->name);
    f_code initializer_code;
    auto ref = symbol_ref(info);
    if (info.length > 0) {
      for (const auto &res : dec->aligned_init) {
        auto [code, var, _] = generate_expr(res.exp, env);
        code.emplace_back(Eeyore::ExprAW{ref, res.offset * 4, var});
        merge_into(initializer_code, code);
      }
    } else {
      for (const auto &res : dec->aligned_init) {
        auto [code, var, _] = generate_expr(res.exp, env);
        code.emplace_back(Eeyore::ExprC{ref, var});
        merge_into(initializer_code, code);
      }
    }
    return std::make_tuple(initializer_code, info);
  }

  expr_type generate_expr(rc_ptr<AST::Expression> exp, environment_t env) {
    if (auto id = std::dynamic_pointer_cast<AST::Identifier>(exp)) {
      auto info = env->symbols->at(id->name);
      auto dec =
        std::dynamic_pointer_cast<AST::CompileTimeDeclaration>(info.ptr);
      return expr_type{{}, symbol_ref(info), dec->type.value()};

    } else if (
      auto lit = std::dynamic_pointer_cast<AST::LiteralExpression>(exp)) {
      auto temp = new_temp(env);
      auto code = f_code{Eeyore::ExprC{temp, lit->val}};
      return expr_type{code, temp, SemanticType::scalar};

    } else if (
      auto unary = std::dynamic_pointer_cast<AST::UnaryExpression>(exp)) {
      if (unary->op == UnaryOp::Plus) {
        return generate_expr(unary->ch, env);
      }
      auto [code, result_ch, _] = generate_expr(unary->ch, env);
      assert(_.is_scalar());
      auto temp = new_temp(env);
      code.push_back(Eeyore::ExprU{temp, unary->op, result_ch});
      return expr_type{code, temp, SemanticType::scalar};

    } else if (
      auto binary = std::dynamic_pointer_cast<AST::BinaryExpression>(exp)) {
      auto [code_ch0, result_ch0, _0] = generate_expr(binary->ch0, env);
      auto [code_ch1, result_ch1, _1] = generate_expr(binary->ch1, env);
      assert(_0.is_scalar());
      assert(_1.is_scalar());
      auto temp = new_temp(env);
      if (binary->op == BinaryOp::And) {
        auto mid = Eeyore::LabelE{env->new_label()};
        auto end = Eeyore::LabelE{env->new_label()};
        return expr_type{
          concat(
            code_ch0,
            f_code{
              Eeyore::JumpC{mid, result_ch0},
            },
            code_ch1,
            f_code{
              Eeyore::ExprC{temp, result_ch1},
              Eeyore::JumpU{end},
              mid,
              Eeyore::ExprC{temp, 0},
              end,
            }),
          temp, SemanticType::scalar};
      } else if (binary->op == BinaryOp::Or) {
        auto mid = Eeyore::LabelE{env->new_label()};
        auto end = Eeyore::LabelE{env->new_label()};
        return expr_type{
          concat(
            code_ch0,
            f_code{
              Eeyore::JumpC{mid, result_ch0},
              Eeyore::ExprC{temp, 1},
              Eeyore::JumpU{end},
              mid,
            },
            code_ch1,
            f_code{
              Eeyore::ExprC{temp, result_ch1},
              end,
            }),
          temp, SemanticType::scalar};
      } else {
        auto code = concat(code_ch0, code_ch1);
        code.push_back(Eeyore::ExprB{temp, binary->op, result_ch0, result_ch1});
        return expr_type{code, temp, SemanticType::scalar};
      }

    } else if (
      auto offset = std::dynamic_pointer_cast<AST::OffsetExpression>(exp)) {
      auto [code_arr, result_arr, arr_type] = generate_expr(offset->arr, env);
      auto [code_off, result_off, _] = generate_expr(offset->offset, env);
      assert(_.is_scalar());
      assert(arr_type.is_array());
      auto type = arr_type.drop_first();
      auto disp = new_temp(env);
      auto temp = new_temp(env);
      f_code snippet;
      if (type.is_array()) {
        // calculate a pointer
        snippet = f_code{
          Eeyore::ExprB{
            disp, BinaryOp::Mult, result_off, 4 * arr_type.offsets.front()},
          Eeyore::ExprB{temp, BinaryOp::Plus, result_arr, disp},
        };
      } else {
        // calculate an indirect access
        snippet = f_code{
          Eeyore::ExprB{
            disp, BinaryOp::Mult, result_off, 4 * arr_type.offsets.front()},
          Eeyore::ExprAR{temp, result_arr, disp},
        };
      }
      auto code = concat(code_arr, code_off, snippet);
      return expr_type{code, temp, type};

    } else if (
      auto call = std::dynamic_pointer_cast<AST::CallExpression>(exp)) {
      auto code = f_code{};

      std::vector<Eeyore::VariableI> temp_params;
      for (auto param : call->args.params) {
        auto [code_p, result_p, _] = generate_expr(param, env);
        code.insert(code.end(), code_p.begin(), code_p.end());
        if (result_p.cat != Eeyore::VCategory::temp) {
          auto temp_p = new_temp(env);
          code.push_back(Eeyore::ExprC{temp_p, result_p});
          temp_params.push_back(temp_p);
        } else {
          temp_params.push_back(result_p);
        }
      }
      for (auto temp_param : temp_params) {
        code.push_back(Eeyore::Param{temp_param});
      }
      auto function = std::dynamic_pointer_cast<AST::Callable>(
        env->symbols->at(call->func->name).ptr);
      assert(function != nullptr);
      if (
        auto prim =
          std::dynamic_pointer_cast<AST::PrimitiveFunction>(function)) {
        // Primitive "macro expansion"
        int lineno = env->lexer->toPosition(call->range.begin).line;
        code.push_back(Eeyore::Param{lineno});
        code.push_back(Eeyore::CallV{sysy_macro_prefix + call->func->name});
        return expr_type{
          code,
          Eeyore::VariableI{Eeyore::VCategory::temp, -1},
          SemanticType::scalar,
        };
      } else if (function->ret == PrimitiveType::INT) {
        auto ret = new_temp(env);
        code.push_back(Eeyore::CallI{ret, call->func->name});
        return expr_type{code, ret, SemanticType::scalar};
      } else {
        code.push_back(Eeyore::CallV{call->func->name});
        return expr_type{
          code,
          Eeyore::VariableI{Eeyore::VCategory::temp, -1},
          SemanticType::scalar,
        };
      }

    } else {
      throw Exception::BadAST(
        fmt::format("control reached the end of {}()", __func__));
    }
  }

  stmt_result generate_stmt(
    rc_ptr<AST::Statement> stmt, environment_t env,
    std::optional<Eeyore::Label> cont_pos,
    std::optional<Eeyore::Label> break_pos) {
    if (auto brk = std::dynamic_pointer_cast<AST::BreakStmt>(stmt)) {
      return stmt_result{f_code{Eeyore::JumpU{break_pos.value()}}, {}};

    } else if (auto cont = std::dynamic_pointer_cast<AST::ContinueStmt>(stmt)) {
      return stmt_result{f_code{Eeyore::JumpU{cont_pos.value()}}, {}};

    } else if (auto blk = std::dynamic_pointer_cast<AST::Block>(stmt)) {
      env = blk->env;
      auto code = f_code{};
      auto symtab = f_symtab{};
      for (auto item : blk->code) {
        if (
          auto dec =
            std::dynamic_pointer_cast<AST::CompileTimeDeclaration>(item)) {
          auto [dec_code, dec_sym] = generate_dec(dec, env);
          code.insert(code.end(), dec_code.begin(), dec_code.end());
          symtab.push_back(dec_sym);
        } else if (
          auto stmt = std::dynamic_pointer_cast<AST::Statement>(item)) {
          auto [code_, symtab_] = generate_stmt(stmt, env, cont_pos, break_pos);
          code.insert(code.end(), code_.begin(), code_.end());
          symtab.insert(symtab.end(), symtab_.begin(), symtab_.end());
        }
      }
      return stmt_result{code, symtab};

    } else if (
      auto assign = std::dynamic_pointer_cast<AST::AssignmentStmt>(stmt)) {
      auto [rhs_code, rhs_temp, _r] = generate_expr(assign->rhs, env);
      f_code code = rhs_code;
      auto lval = assign->lhs;
      f_code snippet;
      if (auto var_lval = std::dynamic_pointer_cast<AST::Identifier>(lval)) {
        // scalar assign
        auto [_code, var, _] = generate_expr(var_lval, env);
        assert(_.is_scalar());
        snippet = {Eeyore::ExprC{var, rhs_temp}};
      } else if (
        auto indirect_lval =
          std::dynamic_pointer_cast<AST::OffsetExpression>(lval)) {
        // array subscript assign
        auto [code_arr, result_arr, arr_type] =
          generate_expr(indirect_lval->arr, env);
        auto [code_off, result_off, _] =
          generate_expr(indirect_lval->offset, env);
        assert(_.is_scalar());
        assert(arr_type.is_array());
        assert(arr_type.drop_first().is_scalar());

        auto disp = new_temp(env);
        merge_into(code, code_arr);
        merge_into(code, code_off);
        snippet = {
          Eeyore::ExprB{
            disp, BinaryOp::Mult, result_off, 4 * arr_type.offsets.front()},
          Eeyore::ExprAW{result_arr, disp, rhs_temp},
        };
      }
      merge_into(code, snippet);
      return stmt_result{code, {}};
    } else if (
      auto exps = std::dynamic_pointer_cast<AST::ExpressionStmt>(stmt)) {
      auto [code, result, _] = generate_expr(exps->rhs, env);
      return stmt_result{code, {}};

    } else if (auto ret = std::dynamic_pointer_cast<AST::ReturnStmt>(stmt)) {
      if (ret->rhs.has_value()) {
        auto exp = ret->rhs.value();
        auto [code, result, _] = generate_expr(exp, env);
        return stmt_result{
          concat(
            code,
            f_code{
              Eeyore::RetI{result},
            }),
          {},
        };
      } else {
        return stmt_result{
          {
            Eeyore::RetV{},
          },
          {},
        };
      }

    } else if (auto ifs = std::dynamic_pointer_cast<AST::IfStmt>(stmt)) {
      auto [cond_code, cond, _] = generate_expr(ifs->cond, env);
      auto [then_code, then_dec] =
        generate_stmt(ifs->then_clause, env, cont_pos, break_pos);
      if (ifs->else_clause.has_value()) {
        auto [else_code, else_dec] =
          generate_stmt(ifs->else_clause.value(), env, cont_pos, break_pos);
        auto else_label = Eeyore::LabelE{env->new_label()};
        auto end_label = Eeyore::LabelE{env->new_label()};
        return stmt_result{
          concat(
            cond_code,
            f_code{
              Eeyore::JumpC{else_label, cond},
            },
            then_code,
            f_code{
              Eeyore::JumpU{end_label},
              else_label,
            },
            else_code,
            f_code{
              end_label,
            }),
          concat(then_dec, else_dec),
        };
      } else {
        auto end_label = Eeyore::LabelE{env->new_label()};
        return stmt_result{
          concat(
            cond_code,
            f_code{
              Eeyore::JumpC{end_label, cond},
            },
            then_code,
            f_code{
              end_label,
            }),
          then_dec,
        };
      }

    } else if (auto loop = std::dynamic_pointer_cast<AST::WhileStmt>(stmt)) {
      auto [cond_code, cond, _] = generate_expr(loop->cond, env);
      auto start_label = Eeyore::LabelE{env->new_label()};
      auto end_label = Eeyore::LabelE{env->new_label()};
      auto [body_code, body_dec] =
        generate_stmt(loop->body, env, start_label, end_label);

      return stmt_result{
        concat(
          f_code{
            start_label,
          },
          cond_code,
          f_code{
            Eeyore::JumpC{end_label, cond},
          },
          body_code,
          f_code{
            Eeyore::JumpU{start_label},
            end_label,
          }),
        body_dec};
    } else {
      throw Exception::BadAST(
        fmt::format("control reached the end of {}()", __func__));
    }
  }

  Eeyore::Function
  pretty_function(std::string_view name, int arity, f_dec dec, f_code code) {
    using namespace Eeyore;
    std::unordered_set<int> uniq;
    f_code vcode;
    for (auto const &stmt : code) {
      vcode.push_back(stmt);

      auto dst = std::visit(
        overloaded{
          [](ExprU exp) -> std::optional<SysY::Eeyore::VariableX> {
            return exp.dst;
          },
          [](ExprB exp) -> std::optional<SysY::Eeyore::VariableX> {
            return exp.dst;
          },
          [](ExprC exp) -> std::optional<SysY::Eeyore::VariableX> {
            return exp.dst;
          },
          [](ExprAR exp) -> std::optional<SysY::Eeyore::VariableX> {
            return exp.dst;
          },
          [](CallI exp) -> std::optional<SysY::Eeyore::VariableX> {
            return exp.ret;
          },
          [](auto exp) -> std::optional<SysY::Eeyore::VariableX> {
            return std::nullopt;
          }},
        stmt);
      if (dst.has_value()) {
        auto var = std::get<Eeyore::VariableI>(dst.value());
        if (var.cat == VCategory::temp) {
          if (uniq.insert(var.id).second) {
            dec.emplace_back(DeclarationS{var});
          }
        }
      }
    }
    return Function{std::string(name), arity, dec, vcode};
  }

  Eeyore::Program generate_eeyore(rc_ptr<AST::CompUnit> cu) {
    using namespace Eeyore;
    Program result;

    // Generate the global declarations with initializations
    f_code initializer_code;
    for (auto dec : cu->globals) {
      auto [code, sym] = generate_dec(dec, cu->env);
      result.global.emplace_back(declaration_cast(sym));
      initializer_code.insert(initializer_code.end(), code.begin(), code.end());
    }

    initializer_code.push_back(RetV{});

    result.func.emplace_back(
      pretty_function("__initializer__SysY", 0, {}, initializer_code));

    for (auto f : cu->functions) {
      auto [code, symtab] = generate_stmt(f->code, f->env);
      f_dec dec;
      dec.reserve(symtab.size());
      std::transform(
        symtab.begin(), symtab.end(), std::back_inserter(dec),
        declaration_cast);

      if (f->name == "main") {
        f_code prologue = {CallV{"__initializer__SysY"}};
        code = concat(prologue, code);
      }

      if (f->ret == PrimitiveType::VOID) {
        code.push_back(RetI{0});
      }

      result.func.emplace_back(
        pretty_function(f->name, f->params.size(), dec, code));
    }

    return result;
  }
} // namespace SysY::Pass