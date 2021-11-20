#include <iostream>
#include <fstream>
#include <sstream>
#include "parser_wrapper.hpp"
#include "3rd-party/argparse.hpp"

int main(int argc, char **argv) {
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

  program.add_argument("input")
    .help("Input file name")
    .required();
  
  program.add_argument("-o", "--output")
    .help("Output file name")
    .required();

  try {
    program.parse_args(argc, argv);
  }
  catch (const std::runtime_error& err) {
    std::cerr << err.what() << std::endl;
    std::cerr << program;
    std::exit(1);
  }
  auto ifilename = program.get("input");
  auto ofilename = program.get("output");

  std::ifstream ifile(ifilename);
  std::stringstream ibuf;
  ibuf << ifile.rdbuf();
  auto res = SysY::parse(ibuf.str(), program.get<bool>("--trace"));
  std::cout << res->toString() << std::endl;

  return 0;
}
