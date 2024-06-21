cmake_minimum_required(VERSION 3.10)

project(UnionLib)

set(CMAKE_CXX_STANDARD 20)

file(GLOB_RECURSE UNIONLIB_SRC "src/*.cpp" "src/*.c")

add_library(UnionLib ${UNIONLIB_SRC})

target_compile_options(UnionLib PUBLIC
    "$<$<C_COMPILER_ID:MSVC>:/utf-8>"
    "$<$<CXX_COMPILER_ID:MSVC>:/utf-8>"
    "$<$<C_COMPILER_ID:MSVC>:/W4>"
    "$<$<CXX_COMPILER_ID:MSVC>:/W4>"
)

target_include_directories(UnionLib PUBLIC include)
target_include_directories(UnionLib PRIVATE
    include_private
    include_private/common

    # A扫图像
    include_private/ASCAN
    include_private/ASCAN/390N_T8
    include_private/ASCAN/330
    include_private/ASCAN/390

    # TOFD_PE
    include_private/TOFD_PE
    include_private/TOFD_PE/TPE
    include_private/TOFD_PE/TOF

    # 9x0
    include_private/TOFD_MULTI/
    include_private/TOFD_MULTI/9x0

    # HDBridge
    include_private/Bridge
    include_private/Bridge/12CH_USB
)

add_library(_TOFDPort SHARED IMPORTED)

set_target_properties(
    _TOFDPort PROPERTIES
    IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/lib/Bridge/12CH_USB/TOFDPort.dll"
    IMPORTED_IMPLIB "${CMAKE_CURRENT_LIST_DIR}/lib/Bridge/12CH_USB/TOFDPort.lib"
)

target_link_libraries(UnionLib PRIVATE _TOFDPort)

if(Morose_FOUND)
    morose_copy(
        TARGET
        UnionLib
        FILES
        "lib/Bridge/12CH_USB/TOFDPort.dll"
        "lib/Bridge/12CH_USB/ftd2xx.dll"
    )
else(Morose_FOUND)
    execute_process(
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_LIST_DIR}/lib/Bridge/12CH_USB/TOFDPort.dll ${CMAKE_BINARY_DIR}/TOFDPort.dll
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_LIST_DIR}/lib/Bridge/12CH_USB/ftd2xx.dll ${CMAKE_BINARY_DIR}/ftd2xx.dll
    )
endif(Morose_FOUND)
