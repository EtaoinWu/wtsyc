#include "array_align.hpp"
#include "3rd-party/fmt/core.h"
#include "error.hpp"

namespace SysY::ArrayAlign {
  int round_up(int x, int base) {
    int div = x / base;
    int round_down = div * base;
    if (round_down == x) {
      return x;
    } else {
      return round_down + base;
    }
  }

  Result _array_align(
    const AST::pointer<AST::ArrayLiteral> &exp, const SemanticType &type,
    int j) {
    int current = 0;
    Result alignment = {};
    for (const auto &item : exp->data) {
      if (auto arr = std::dynamic_pointer_cast<AST::ArrayLiteral>(item)) {
        current = round_up(current, type.offsets[j]);
        auto rec = _array_align(arr, type, j + 1);
        for (auto &align : rec) {
          align.offset += current;
        }
        std::move(rec.begin(), rec.end(), std::back_inserter(alignment));
        current += type.offsets[j];
      } else if (auto exp = std::dynamic_pointer_cast<AST::Expression>(item)) {
        alignment.push_back({current, exp});
        current++;
      } else {
        throw Exception::BadAST(
          fmt::format("bad array alignment {}", item->toJSON().dump()));
      }
    }
    return alignment;
  }

  // Perform the array initialzation process.
  Result array_align(
    const AST::pointer<AST::ArrayLiteral> &exp, const SemanticType &type) {
    return _array_align(exp, type, 0);
  }

  // Perform the array initialzation process, and zero-expand them.
  Result array_align_expand(
    const AST::pointer<AST::ArrayLiteral> &exp, const SemanticType &type) {
    auto result = _array_align(exp, type, 0);
    auto size = type.length();
    auto expanded = Result{};
    expanded.reserve(size);
    auto it = result.begin();
    auto zero = std::make_shared<AST::LiteralExpression>(0);
    for (int i = 0; i < size; i++) {
      if (it != result.end() && it->offset == i) {
        expanded.push_back(*it);
        it++;
      } else {
        expanded.push_back(Alignment{i, zero});
      }
    }
    while (it != result.end()) {
      expanded.push_back(*it);
    }
    return expanded;
  }
} // namespace SysY::ArrayAlign