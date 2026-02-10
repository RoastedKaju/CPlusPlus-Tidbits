cmake_minimum_required(VERSION 3.24)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(CppTidbits CXX)

add_executable(${PROJECT_NAME}
    "src/main.cpp"
    "src/beeper.h"
    "src/increment_overload.h"
    "src/pattern_match.h"
    "src/reverse_string.h"
    "src/remove_spaces.h"
    "src/implement_memmove.h"
    "src/find_unique.h"
    "src/detect_arbitrage.h"
    "src/max_drawdown.h"
    "src/singleton.h"
    "src/crtp_polymorphism.h")