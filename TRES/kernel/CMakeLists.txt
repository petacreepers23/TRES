

# TODO: kernel_functions should be in lib , or other place but here?..

add_library(kernel_functions
STATIC
kernel_functions.cpp)

target_include_directories(kernel_functions
PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/lib
)


add_executable(kernel
    kernel_main.cpp 
)
set(LINKER_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/linkerscript.ld")
set(LINKER_FLAGS "SHELL:-T${LINKER_SCRIPT} -ffreestanding -nostdlib -lgcc")
target_link_options(kernel PRIVATE ${LINKER_FLAGS})

#target_link_options(bootloader PUBLIC "LINKER:--gc-sections,-Tbootloader.ld,-Map=bootloader.map") 
#target_link_options(kernel PRIVATE "SHELLLINKER:-T  -ffreestanding -nostdlib -lgcc ")
#add_link_options(LINKER:-T ${CMAKE_CURRENT_BINARY_DIR}/linker.ld -nostdlib)
target_include_directories(kernel
PRIVATE
${CMAKE_CURRENT_SOURCE_DIR}
${CMAKE_SOURCE_DIR}/lib
${CMAKE_SOURCE_DIR}/drivers
)


target_link_libraries(kernel
PRIVATE
lib
kernel_functions
drivers
)
set_target_properties(kernel PROPERTIES OUTPUT_NAME "KERNEL.BIN")

#add_link_options(LINKER:-T ${CMAKE_CURRENT_SOURCE_DIR}/linkerscript.ld -nostdlib -lgcc)