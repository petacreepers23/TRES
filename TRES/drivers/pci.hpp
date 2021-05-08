#pragma once

// Esto no tendría que estar aquí.
typedef signed char __int8_t;
typedef unsigned char __uint8_t;
typedef signed short int __int16_t;
typedef unsigned short int __uint16_t;
typedef signed int __int32_t;
typedef unsigned int __uint32_t;

typedef __uint8_t uint8_t;
typedef __uint16_t uint16_t;
typedef __uint32_t uint32_t;

int ls_pci();
uint32_t get_PCI_register(int bus, int slot, int func, int reg);
int print_slot(int bus, int slot);
void print_bus(int bus);
int print_device(int bus, int slot, int func);
int print_regions(int bus, int slot, int func, int bridge);
void print_pci();