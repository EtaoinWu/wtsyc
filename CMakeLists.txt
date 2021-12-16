cmake_minimum_required(VERSION 3.5)
project(what_the_sysy_compiler LANGUAGES CXX VERSION "0.0.1")

set (CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake")
include(cotire)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(${CMAKE_SOURCE_DIR}/3rd-party/fpp)

add_compile_options(-Wall -Wno-register)

# Make ninja happy with color outputs
option (FORCE_COLORED_OUTPUT "Always produce ANSI-colored output (GNU/Clang only)." FALSE)
if (${FORCE_COLORED_OUTPUT})
    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
       add_compile_options (-fdiagnostics-color=always)
    elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
       add_compile_options (-fcolor-diagnostics)
    endif ()
endif ()

find_package(BISON REQUIRED)

BISON_TARGET(parser
             parser.ypp
             ${CMAKE_CURRENT_BINARY_DIR}/parser.cpp)

FILE(GLOB CPP_SRC RELATIVE ${CMAKE_SOURCE_DIR} *.cpp)

FILE(GLOB FMT_CPP_SRC RELATIVE ${CMAKE_SOURCE_DIR} 3rd-party/fmt/src/*.cc)

FILE(GLOB FMT_HDR RELATIVE ${CMAKE_SOURCE_DIR} 3rd-party/fmt/*.h)

find_library(LIBSYSY sysy 3rd-party/sysy-runtime)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/lexer.cpp
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/lexer.rl
    COMMAND ragel ${CMAKE_CURRENT_SOURCE_DIR}/lexer.rl -o ${CMAKE_CURRENT_SOURCE_DIR}/lexer.cpp || echo "ragel not found"
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(wtsyc
    ${CPP_SRC}
    ${FMT_CPP_SRC}
    ${BISON_parser_OUTPUTS}
    ${CMAKE_CURRENT_SOURCE_DIR}/lexer.cpp
)

target_include_directories(wtsyc
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/3rd-party
        ${CMAKE_CURRENT_SOURCE_DIR}/3rd-party/fpp
        ${CMAKE_CURRENT_SOURCE_DIR}/3rd-party/fpp/gc/include
)

target_link_libraries(wtsyc fpp ${LIBSYSY})

set_target_properties(wtsyc PROPERTIES
        COTIRE_PREFIX_HEADER_INCLUDE_PATH "${CMAKE_SOURCE_DIR}/3rd-party/json")

cotire(wtsyc)
