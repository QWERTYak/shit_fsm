cmake_minimum_required(VERSION 3.31)
project(shit_fsm LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_executable(shit_fsm main.cpp)
target_link_libraries(shit_fsm PRIVATE /usr/lib/libstdc++.so)
