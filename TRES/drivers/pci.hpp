#pragma once

// Esto no tendría que estar aquí.
#include <stdint.h>
int ls_pci();
uint32_t get_PCI_register(int bus, int slot, int func, int reg);
int print_slot(int bus, int slot);
void print_bus(int bus);
int print_device(int bus, int slot, int func);
int print_regions(int bus, int slot, int func, int bridge);
void print_pci();