cmake_minimum_required(VERSION 3.31)
project(shit_fsm LANGUAGES CXX)

set(CMAKE_CXX_COMPILER "/usr/bin/clang++")
set(CMAKE_CXX_COMPILER_LINKER "/usr/bin/lld")

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_executable(shit_fsm main.cpp)
target_compile_options(shit_fsm PRIVATE -shared-libgcc) #shared libgcc
target_link_libraries(shit_fsm PRIVATE /usr/lib/libstdc++.so /usr/lib/libc.so) #shared libc,libstdc++
