cmake_minimum_required(VERSION 3.22)

project(
    ACompilerProj
    VERSION 0.1
    LANGUAGES CXX)


set(A_COMPILER_SRCS
        src/main.cpp
        src/file_utils.cpp
        src/lexer.cpp
        src/cfg.cpp)

set(CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(dummy_compiler ${A_COMPILER_SRCS})
set_target_properties(dummy_compiler PROPERTIES CXX_EXTENSIONS OFF)
