#pragma once
void print(const char *,unsigned int);
void stop();

void set_PCI_register(int bus, int slot, int func, int reg, unsigned int value);
void search_device_class_in_bus(int bus, int classID);
void printNumeric(unsigned int num);
void simple_print(const char* msg);
void simple_print(const char* msg,char a);