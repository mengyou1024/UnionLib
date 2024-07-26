cmake_minimum_required(VERSION 3.20)

project(UnionTest)
include(FetchContent)
enable_testing()
FetchContent_Declare(
    gtest
    URL https://codeload.github.com/google/googletest/tar.gz/refs/tags/v1.14.0
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(gtest)

FetchContent_Declare(
    MISC_UTILS
    GIT_REPOSITORY https://github.com/mengyou1024/misc_utils.git
    GIT_TAG master
)

FetchContent_MakeAvailable(MISC_UTILS)

FetchContent_Declare(
    LIBFTDI
    GIT_REPOSITORY https://github.com/mengyou1024/ftd2xx.git
    GIT_TAG master
)
FetchContent_MakeAvailable(LIBFTDI)

FetchContent_Declare(
    libprotobuf
    GIT_REPOSITORY https://github.com/mengyou1024/protobuf_prebuil_3.21.12.git
    GIT_TAG master
)
FetchContent_MakeAvailable(libprotobuf)

find_package(Qt5 5.15 REQUIRED COMPONENTS Core SerialPort Widgets)

set(CMAKE_CXX_STANDARD 20)
add_subdirectory(.. UniLib)
add_executable(unit_tests test_union.cpp)
target_link_libraries(unit_tests PRIVATE gtest_main UnionLib)
target_link_libraries(UnionLib PUBLIC misc_utils Qt5::Core Qt5::SerialPort Qt5::Widgets)
target_include_directories(unit_tests PRIVATE ../include_private)
include(GoogleTest)
gtest_discover_tests(unit_tests PROPERTIES LABELS "File" DISCOVERY_TIMEOUT 240)

add_custom_command(TARGET unit_tests POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different ${CMAKE_SOURCE_DIR}/test_file ${CMAKE_BINARY_DIR}/test_file
)
