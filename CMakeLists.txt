cmake_minimum_required(VERSION 3.10)

project(UnionLib)

set(CMAKE_CXX_STANDARD 20)


file(GLOB_RECURSE UNIONLIB_SRC "src/*.cpp" "src/*.c")

add_library(UnionLib ${UNIONLIB_SRC})

target_include_directories(UnionLib PUBLIC include)
target_include_directories(UnionLib PRIVATE 
    include_private 
    include_private/common
    include_private/390N_T8
)
target_compile_options(UnionLib PRIVATE "$<$<C_COMPILER_ID:MSVC>:/utf-8>" "$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
