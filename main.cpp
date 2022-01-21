// clang-format off
#include <argparse.hpp>
// clang-format on
#include "error.hpp"
#include "gen_eeyore.hpp"
#include "glimmy.hpp"
#include "interop.hpp"
#include "optimize.hpp"
#include "parser_wrapper.hpp"
#include "riscv_output.hpp"
#include "semantics.hpp"
#include <fpp/fvector.h>
#include <fstream>
#include <gc/gc.h>
#include <iostream>
#include <sstream>

namespace SysY {
  Pass::environment_t new_primitive_env(const Lexer *lexer) {
    std::string primitive_declaration = R"(
      int getint() {}
      int getch() {}
      int getarray(int a[]) {}
      void putint(int a) {}
      void putch(int a) {}
      void putarray(int a, int b[]) {}
      void _sysy_starttime(int a) {}
      void _sysy_stoptime(int a) {}
    )";

    Driver driver(primitive_declaration);
    auto primitive_unit = parse(driver, false);
    auto env = std::make_shared<Pass::Environment>(&driver.lexer);
    Pass::typecheck(primitive_unit, env);
    env->lexer = lexer;

    for (auto prim : {
           std::make_shared<AST::PrimitiveFunction>(
             PrimitiveType::VOID, "starttime"),
           std::make_shared<AST::PrimitiveFunction>(
             PrimitiveType::VOID, "stoptime"),
         }) {
      auto cat = Pass::LowLevelSymbolInfo::category_of(prim.get());
      env->symbols->insert(
        prim->name, Pass::LowLevelSymbolInfo{
                      cat,
                      env->get_count(cat),
                      -1,
                      prim,
                    });
    }
    return env;
  }
} // namespace SysY

int main(int argc, char **argv) {
  GC_INIT();

  using namespace SysY;

  argparse::ArgumentParser argparser("wtsyc");

  argparser.add_argument("-S", "--source")
    .help("Compile from source")
    .default_value(true)
    .implicit_value(true);

  argparser.add_argument("-T", "--trace")
    .help("Bison trace")
    .default_value(false)
    .implicit_value(true);

  argparser.add_argument("-e", "--output-eeyore")
    .help("Output Eeyore code")
    .default_value(false)
    .implicit_value(true);
  argparser.add_argument("-t", "--output-tigger")
    .help("Output Tigger code")
    .default_value(false)
    .implicit_value(true);

  argparser.add_argument("input").help("Input file name").required();

  argparser.add_argument("-o", "--output").help("Output file name").required();

  try {
    argparser.parse_args(argc, argv);
  } catch (const std::runtime_error &err) {
    std::cerr << err.what() << std::endl;
    std::cerr << argparser;
    std::exit(1);
  }
  auto ifilename = argparser.get("input");
  auto ofilename = argparser.get("-o");
  auto trace = argparser.get<bool>("--trace");

  std::ifstream ifile(ifilename);
  std::stringstream ibuf;
  ibuf << ifile.rdbuf();
  std::string source = ibuf.str();
  Driver driver(source);
  auto res = parse(driver, trace);

  if (trace) {
    DEBUG_LOG("\n" + res->toJSON().dump());
  }

  // passes
  try {
    Pass::verify(res);
    Pass::typecheck(res, new_primitive_env(&driver.lexer));
    auto eeyore = Pass::generate_eeyore(res);
    if (trace) {
      DEBUG_LOG("\n" + output_program(eeyore));
    }
    auto pinkie = Pinkie::from_eeyore(eeyore);
    pinkie = Pinkie::Pass::optimize(pinkie);
    if (argparser.get<bool>("--output-eeyore")) {
      auto eeyore2 = Pinkie::to_eeyore(pinkie);
      auto program = output_program(eeyore2);
      std::ofstream ofs(ofilename);
      ofs << program;
    } else {
      auto dashie = Dashie::compile(pinkie);
      dashie = Dashie::Pass::peephole(dashie);
      if (argparser.get<bool>("--output-tigger")) {
        auto tigger = Dashie::to_tigger(dashie);
        std::ofstream ofs(ofilename);
        ofs << tigger;
      } else {
        auto riscv = Glimmy::to_riscv(dashie);
        std::ofstream ofs(ofilename);
        ofs << riscv;
      }
    }
  } catch (const std::runtime_error &e) {
    std::cerr << typeid(e).name() << ' ' << e.what() << '\n';
    return -1;
  }

  return 0;
}
