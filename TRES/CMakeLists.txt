SET(CMAKE_TOOLCHAIN_FILE "toolchain_i686_.cmake")
cmake_minimum_required(VERSION 3.12)

project(TRES 
    VERSION 1.0
    DESCRIPTION "The TRES operating system"
    LANGUAGES CXX ASM_NASM
)

add_subdirectory(boot)
add_subdirectory(lib)
add_subdirectory(drivers)
add_subdirectory(kernel)

