cmake_minimum_required(VERSION 3.5)
project(what_the_sysy_compiler LANGUAGES CXX VERSION "0.0.1")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compile_options(-Wall -Wno-register)

find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)

BISON_TARGET(parser
             parser.ypp
             ${CMAKE_CURRENT_BINARY_DIR}/parser.cpp)

FILE(GLOB CPP_SRC RELATIVE ${CMAKE_SOURCE_DIR} *.cpp)

FILE(GLOB FMT_CPP_SRC RELATIVE ${CMAKE_SOURCE_DIR} fmt/src/*.cc)

add_executable(wtsyc
    ${CPP_SRC}
    ${FMT_CPP_SRC}
    ${BISON_parser_OUTPUTS}
)

target_include_directories(wtsyc
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_BINARY_DIR})
