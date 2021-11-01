cmake_minimum_required(VERSION 3.5)
project(what_the_sysy_compiler LANGUAGES CXX VERSION "0.0.1")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compile_options(-Wall -Wno-deprecated-register)

find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)

BISON_TARGET(sysy_parser
             sysy_parser.yy
             ${CMAKE_CURRENT_BINARY_DIR}/sysy_parser.parse.cpp)
FLEX_TARGET(sysy_lexer
            sysy_lexer.l
            ${CMAKE_CURRENT_BINARY_DIR}/sysy_lexer.lex.cpp)
ADD_FLEX_BISON_DEPENDENCY(sysy_lexer sysy_parser)

FILE(GLOB_RECURSE CPP_SRC RELATIVE ${CMAKE_SOURCE_DIR} *.cpp)

add_executable(wtsyc
    ${CPP_SRC}
    ${BISON_sysy_parser_OUTPUTS}
    ${FLEX_sysy_lexer_OUTPUTS}
)

target_include_directories(wtsyc
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${FLEX_INCLUDE_DIRS}
        ${CMAKE_CURRENT_BINARY_DIR})
