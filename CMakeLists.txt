cmake_minimum_required(VERSION 3.12)

project(sudoku)

add_subdirectory(raylib)
include_directories(raylib/src)

add_executable(sudoku main.cpp sudoku.cpp)

set_property(TARGET sudoku PROPERTY CXX_STANDARD 20)

target_link_libraries(sudoku raylib)
