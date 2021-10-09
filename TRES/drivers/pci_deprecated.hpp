#pragma once

// Esto no tendría que estar aquí.
#include <stdint.h>
#include "pci_headers.hpp"
#include "../kernel/kernel_functions.hpp"

int ls_pci();
uint32_t get_PCI_register(int bus, int slot, int func, int reg);
// int print_slot(int bus, int slot);
// void print_bus(int bus);
// int print_device(int bus, int slot, int func);
// int print_regions(int bus, int slot, int func, int bridge);
// void print_pci();
enum Print_info : uint8_t { no_print = 0,
	                         print_basic_info = 1,
	                         print_all = 2 };


int get_pci_general_info(const uint32_t bus, const uint32_t slot, const uint32_t func, const Print_info print_info);
void configure_pci_devices(const Print_info print_info=print_all);
void configure_bus(const uint32_t bus, const Print_info print_info);
int configure_slot(const uint32_t bus, const uint32_t slot, const Print_info print_info);

inline bool device_exist(uint32_t bus, uint32_t slot, uint32_t func);





class prueba
{
private:
	/* data */
public:
	prueba(int a , int b);
	~prueba();
	int a ;
	int b;
	void print();

};