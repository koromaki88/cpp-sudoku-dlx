cmake_minimum_required(VERSION 4.0)
project(SudokuDLX VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # KEEP NEAR THE TOP OR compile_commands.json WONT GENERATE

include_directories(
    include
)

add_executable(${PROJECT_NAME} src/main.cpp)
