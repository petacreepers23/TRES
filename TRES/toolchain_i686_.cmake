set(CMAKE_SYSTEM_NAME "Generic")
SET(CMAKE_SYSTEM_VERSION 1)



message("CMAKE_SYSTEM_NAME: ${CMAKE_SYSTEM_NAME}")
set(CMAKE_SYSTEM_PROCESSOR i686)
set(CMAKE_ASM_NASM_COMPILER nasm)
set(CMAKE_ASM_NASM_OBJECT_FORMAT bin)
# nasm do not have linker, and we are in 16 bits. Because we really do not need a linker and just want the proper name, a mv is not ok, but ok.
set(CMAKE_ASM_NASM_LINK_EXECUTABLE "mv <CMAKE_ASM_NASM_LINK_FLAGS> <LINK_FLAGS> <OBJECTS>   <TARGET>")
set(CMAKE_CXX_COMPILER i686-elf-g++)

# Flag to the compiler. TODO: replace
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -std=c++17 -Wall -ffreestanding  -fno-exceptions -fno-rtti" )

set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")