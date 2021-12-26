#include "parser_wrapper.hpp"
#include "semantics.hpp"
#include <argparse.hpp>
#include <fstream>
#include <gc/gc_cpp.h>
#include <iostream>
#include <sstream>

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
  auto ofilename = program.get("output");

  std::ifstream ifile(ifilename);
  std::stringstream ibuf;
  ibuf << ifile.rdbuf();
  auto res = parse(ibuf.str(), program.get<bool>("--trace"));
  std::cout << res->toJSON().dump(2) << std::endl;

  // passes
  try {
    Pass::verify(res);
    Pass::typecheck(res, std::make_shared<Pass::Environment>());
  } catch (const std::runtime_error &e) {
    std::cerr << typeid(e).name() << ' ' << e.what() << '\n';
    return -1;
  }

  return 0;
}
