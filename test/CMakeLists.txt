cmake_minimum_required(VERSION 3.10)

project(test)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(.. UniLib)

add_executable(test main.cpp)

target_link_libraries(test PUBLIC UnionLib)
target_compile_options(test PRIVATE "$<$<C_COMPILER_ID:MSVC>:/utf-8>" "$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
