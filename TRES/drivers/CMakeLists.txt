
set(AHCI_DRIVER_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/ahci/ahci.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ahci/port.cpp
)

add_library(drivers
STATIC
    pci.cpp
    ${AHCI_DRIVER_SRCS}
)

target_include_directories(drivers
PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/lib
    ${CMAKE_SOURCE_DIR}/kernel
)