#pragma once

// Esto no tendría que estar aquí.
#include <stdint.h>
#include "pci_headers.hpp"
#include "../kernel/kernel_functions.hpp"





class pci
{
	public:
	void configure_pci_devices();
	


	private:
	void configure_bus(const uint32_t bus);
	int configure_slot(const uint32_t bus, const uint32_t slot);
	void set_PCI_register(int bus, int slot, int func, int reg, unsigned int value);
	uint32_t get_PCI_register(int bus, int slot, int func, int reg);
	int get_header_info(PCI_header *pci_header, int bus, int dev, int func);
	void make_pci_device(PCI_header *pci_header);

	// Si se lee de CONFIG_DAT se obtiene el contenido del registro PCI
	// especificado en CONFIG_DIR.
	// Si se escribe en CONFIG_DAT se modifica el contenido del registro PCI
	// especificado en CONFIG_DIR.
	const int CONFIG_DIR = 0xCF8;
	const int CONFIG_DAT = 0xCFC;
};

