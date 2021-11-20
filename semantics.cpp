#include "semantics.hpp"

namespace SysY {
  namespace AST {
    void SemanticType::calculate_offsets() {
      offsets = dimensions;
      for (auto i = offsets.end() - 1; i >= offsets.begin(); i--) {
        *i *= *(i + 1);
      }
      for (auto i = offsets.begin(); i < offsets.end() - 1; i++) {
        *i = *(i + 1);
      }
      if (!offsets.empty()) {
        offsets.back() = 1;
      }
    }
  } // namespace AST
} // namespace SysY