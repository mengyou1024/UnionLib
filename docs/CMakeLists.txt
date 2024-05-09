cmake_minimum_required(VERSION 3.10)

project(docs)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in
    ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile
    @ONLY
)
