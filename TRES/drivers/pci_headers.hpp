#include <stdint.h>

#include "../kernel/kernel_functions.hpp"

/*
Existen 3 tipos de cabeceras PCI. Todas comparten una parte común, los primeros 4 bytes
PCI header tipo 0x00 -> Dispositivos "normales" una ahci, un sata, una tarjeta de red etc..
PCI header tipo 0x01 -> PCI-to-PCI bridge
PCI header tipo 0x02 -> PCI-to-Cardbus bridge

+ info
https://wiki.osdev.org/PCI#Configuration_Space for references.
*/

struct PCI_common_part_header {
	// offset 0
	uint16_t vendor_id;
	uint16_t device_id;

	// offset 4
	uint16_t command;
	uint16_t status;

	// offset 8
	uint8_t revision_id;
	uint8_t prog_if;  // Programming Interface Byte  read-only register that specifies a register-level programming interface the device has, if it has any at all.
	uint8_t subclass;
	uint8_t class_code;

	// offset C
	uint8_t cache_line_size;
	uint8_t latency_timer;
	uint8_t header_type;
	uint8_t bist;

	void print_generic() {
		simple_print("vendor id: ");
		simple_print(vendor_id);
		simple_print(" Clase: ");
		simple_print(class_code);
		simple_print(" Subclase: ");
		simple_print(subclass);
		simple_print("\n");
	}
};

/*
Esta estructura es generica, aqui metemos los datos. Luego, cuando sepamos la 
estructura concreta hacemos un casting, y como el header siempre ocupa lo mismo,
pues ya esta.
*/
struct PCI_header {

	// offset 0
	uint16_t vendor_id;
	uint16_t device_id;

	// offset 4
	uint16_t command;
	uint16_t status;

	// offset 8
	uint8_t revision_id;
	uint8_t prog_if;  // Programming Interface Byte  read-only register that specifies a register-level programming interface the device has, if it has any at all.
	uint8_t subclass;
	uint8_t class_code;

	// offset C
	uint8_t cache_line_size;
	uint8_t latency_timer;
	uint8_t header_type;
	uint8_t bist;

	uint32_t specific[12];
	void print_generic() {
		simple_print("vendor id: ");
		simple_print(vendor_id);
		simple_print(" Clase: ");
		simple_print(class_code);
		simple_print(" Subclase: ");
		simple_print(subclass);
		simple_print("\n");
	}
};

struct PCI_header_type_0x00 {
	// offset 0
	uint16_t vendor_id;
	uint16_t device_id;

	// offset 4
	uint16_t command;
	uint16_t status;

	// offset 8
	uint8_t revision_id;
	uint8_t prog_if;  // Programming Interface Byte  read-only register that specifies a register-level programming interface the device has, if it has any at all.
	uint8_t subclass;
	uint8_t class_code;

	// offset C
	uint8_t cache_line_size;
	uint8_t latency_timer;
	uint8_t header_type;
	uint8_t bist;

	// offset 10h-24h
	uint32_t bar[6];  // base address 0-5

	// offset 28h
	uint32_t cardbus_cis_pointer;

	// offset 2C
	uint16_t subsystem_vendor_id;
	uint16_t subsystem_id;

	//offset 30h
	uint32_t expansion_ROM_base_address;

	// offset 34h
	uint8_t capabilities_pointer;
	uint8_t reserved0;
	uint16_t reserved1;

	// offset 38h
	uint32_t reserved2;

	// offset 3C
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

struct pci_header_type_0x02 {  // PCI-to-Cardbus bridge
	PCI_common_part_header generic;
	// TODO: rellenar
};
