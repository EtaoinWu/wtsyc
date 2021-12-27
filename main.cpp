#include "error.hpp"
#include "gen_eeyore.hpp"
#include "parser_wrapper.hpp"
#include "semantics.hpp"
#include <argparse.hpp>
#include <fstream>
#include <gc/gc_cpp.h>
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

  argparse::ArgumentParser program("wtsyc");

  program.add_argument("-T", "--trace")
    .help("Bison trace")
    .default_value(false)
    .implicit_value(true);

  program.add_argument("-e", "--output-eeyore")
    .help("Output Eeyore code")
    .default_value(false)
    .implicit_value(true);
  program.add_argument("-t", "--output-tigger")
    .help("Output Tigger code")
    .default_value(false)
    .implicit_value(true);

  program.add_argument("input").help("Input file name").required();

  program.add_argument("-o", "--output").help("Output file name").required();

  try {
    program.parse_args(argc, argv);
  } catch (const std::runtime_error &err) {
    std::cerr << err.what() << std::endl;
    std::cerr << program;
    std::exit(1);
  }
  auto ifilename = program.get("input");
  auto ofilename = program.get("-o");

  std::ifstream ifile(ifilename);
  std::stringstream ibuf;
  ibuf << ifile.rdbuf();
  std::string source = ibuf.str();
  Driver driver(source);
  auto res = parse(driver, program.get<bool>("--trace"));

  // passes
  try {
    Pass::verify(res);
    Pass::typecheck(res, new_primitive_env(&driver.lexer));
    if (program.get<bool>("--output-eeyore")) {
      auto eeyore = Pass::generate_eeyore(res);
      auto program = output_program(eeyore);
      std::ofstream ofs(ofilename);
      ofs << program;
    }
  } catch (const std::runtime_error &e) {
    std::cerr << typeid(e).name() << ' ' << e.what() << '\n';
    return -1;
  }

  return 0;
}
