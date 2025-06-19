cmake_minimum_required(VERSION 4.0)
project(SudokuDLX VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 26)

include_directories(
    include
)

add_executable(${PROJECT_NAME} src/main.cpp)
