#include "dashie.hpp"
#include "error.hpp"
#include "util.hpp"
#include <algorithm>
#include <fmt/core.h>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>

namespace SysY::Dashie {
#define INDENT "  "

  std::string join(const std::vector<std::string> &strings, const char *delim) {
    std::ostringstream oss;
    std::copy(
      strings.begin(), strings.end(),
      std::ostream_iterator<std::string>(oss, delim));
    return oss.str();
  }

  std::string to_tigger(RegType x) { return std::string{char(x)}; }
  std::string to_tigger(Reg x) {
    return fmt::format("{}{}", to_tigger(x.ns), x.id);
  }
  std::string to_tigger(const Unit &x) {
    std::string prefix;
    if (x.type != Unit::label) {
      prefix = INDENT;
    }
    switch (x.type) {
    case Unit::rbr:
      return prefix + fmt::format(
                        "{} = {} {} {}", to_tigger(x.dst), to_tigger(x.src1),
                        toString(x.bop), to_tigger(x.src2));
    case Unit::rbi:
      return prefix + fmt::format(
                        "{} = {} {} {}", to_tigger(x.dst), to_tigger(x.src1),
                        toString(x.bop), x.imm);
    case Unit::ur:
      return prefix + fmt::format(
                        "{} = {} {}", to_tigger(x.dst), toString(x.uop),
                        to_tigger(x.src1));
    case Unit::cr:
      return prefix +
             fmt::format("{} = {}", to_tigger(x.dst), to_tigger(x.src1));
    case Unit::ci:
      return prefix + fmt::format("{} = {}", to_tigger(x.dst), x.imm);
    case Unit::ar:
      return prefix +
             fmt::format(
               "{} = {}[{}]", to_tigger(x.dst), to_tigger(x.src), x.incr);
    case Unit::aw:
      return prefix +
             fmt::format(
               "{}[{}] = {}", to_tigger(x.dst), x.incr, to_tigger(x.src));
    case Unit::jc:
      return prefix + fmt::format(
                        "if {} {} {} goto l{}", to_tigger(x.src1),
                        toString(x.bop), to_tigger(x.src2), x.label_id);
    case Unit::jmp:
      return prefix + fmt::format("goto l{}", x.label_id);
    case Unit::label:
      return prefix + fmt::format("l{}:", x.label_id);
    case Unit::call:
      return prefix + fmt::format("call f_{}", x.name.raw());
    case Unit::ret:
      return prefix + "return";
    case Unit::wstk:
      return prefix +
             fmt::format("store {} {}", to_tigger(x.src), x.stack_incr / 4);
    case Unit::rstk:
      return prefix +
             fmt::format("load {} {}", x.stack_incr / 4, to_tigger(x.dst));
    case Unit::rglo:
      return prefix + fmt::format("load v{} {}", x.global_id, to_tigger(x.dst));
    case Unit::ptrstk:
      return prefix +
             fmt::format("loadaddr {} {}", x.stack_incr / 4, to_tigger(x.dst));
    case Unit::ptrglo:
      return prefix +
             fmt::format("loadaddr v{} {}", x.global_id, to_tigger(x.dst));
    default:
      C_ERROR("bad Unit::Type");
    }
  }

  std::string to_tigger(const Global &x) {
    if (x.length == 0) {
      return fmt::format("v{} = 0", x.id);
    } else {
      return fmt::format("v{} = malloc {}", x.id, x.length);
    }
  }

  std::string to_tigger(const Function &f) {
    return fmt::format("f_{} [{}] [{}]\n", f.name.raw(), f.arity, f.frame / 4) +
           join(
             map_to_vector<std::string>(
               f.code, static_cast<std::string (*)(const Unit &)>(to_tigger)),
             "\n") +
           fmt::format("end f_{}\n", f.name.raw());
  }

  std::string to_tigger(const Program &prog) {
    return join(
             map_to_vector<std::string>(
               prog.globals,
               static_cast<std::string (*)(const Global &)>(to_tigger)),
             "\n") +
           "\n" +
           join(
             map_to_vector<std::string>(
               prog.funcs,
               static_cast<std::string (*)(const Function &)>(to_tigger)),
             "\n");
  }
#undef INDENT
} // namespace SysY::Dashie