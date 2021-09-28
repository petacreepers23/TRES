add_library(lib
STATIC
    basic_allocator.cpp
)
#set_target_properties(lib PROPERTIES OUTPUT_NAME "")
target_include_directories(lib
PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)