cmake_minimum_required(VERSION 3.24)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(CppTidbits CXX)

set(CMAKE_CXX_CLANG_FORMAT "clang-format;-style=Google")

add_executable(${PROJECT_NAME}
    "src/main.cpp"
    "src/beeper.h"
    "src/increment_overload.h"
    "src/pattern_match.h"
    "src/reverse_string.h"
    "src/remove_spaces.h"
    "src/implement_memmove.h")