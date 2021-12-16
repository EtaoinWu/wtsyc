cmake_minimum_required(VERSION 3.5)

project(libfpp LANGUAGES CXX VERSION "0.2.0")

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/gc)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(fpp
  fcompare.cpp
  flist.cpp
  fseq.cpp
  fshow.cpp
  fstring.cpp
  ftree.cpp
  fvector.cpp
)

target_link_libraries(fpp gc)

target_include_directories(fpp
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/gc/include
)
