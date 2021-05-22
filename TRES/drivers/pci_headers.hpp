#include <stdint.h>

// ################################################################################
// https://wiki.osdev.org/PCI#Configuration_Space for references.
struct PCI_common_part_header {
	uint16_t vendor_id;
	uint16_t device_id;
	uint16_t command;
	uint16_t status;
	uint8_t revision_id;
	uint8_t prog_if; // Programming Interface Byte  read-only register that specifies a register-level programming interface the device has, if it has any at all. 
	uint8_t subclass;
	uint8_t class_code;
	uint8_t cache_line_size;
	uint8_t latency_timer;
	uint8_t header_type;
	uint8_t bist;
};

struct PCI_header_type_0x00 {
	PCI_common_part_header generic;
	uint32_t bar[6];  // base address 0-5
	uint32_t cardbus_cis_pointer;
	uint16_t subsystem_vendor_id;
	uint16_t subsystem_id;
	uint32_t expansion_ROM_base_address;
	uint8_t capabilities_pointer;
	uint8_t reserved0;
	uint16_t reserved1;
	uint32_t reserved2;
	uint8_t interrupt_line;
	uint8_t interrupt_pin;
	uint8_t min_grant;
	uint8_t max_latency;
};

struct PCI_header_type_0x01 {  // PCI-to-PCI bridge

	PCI_common_part_header generic;
	uint32_t base_bar[2];
	uint8_t primary_bus_number;
	uint8_t secondary_bus_number;
	uint8_t subordinate_bus_number;
	uint8_t secondary_latency_timer;
	uint8_t io_base;
	uint8_t io_limit;
	uint16_t secondary_status;
	uint16_t memory_base;
	uint16_t memory_limit;
	uint16_t prefetchable_memory_base;
	uint16_t prefetchable_memory_limit;
	uint32_t prefetchable_memory_base_upper32;
	uint32_t prefetchable_memory_limit_upper32;
	uint16_t io_base_upper16;
	uint16_t io_limit_upper16;
	uint8_t capability_pointer;
	uint32_t reserved;
	uint32_t expansion_ROM_base_address;
	uint8_t interrupt_line; 
	uint8_t interrupt_pin;  
	uint16_t bridge_control;
};

struct pci_header_type_0x02{ // PCI-to-Cardbus bridge
	PCI_common_part_header generic;
	// TODO: rellenar
};
