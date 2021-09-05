#pragma once
#include <stdint.h>
void print(const char *,unsigned int);
void stop();

//void set_PCI_register(int bus, int slot, int func, int reg, unsigned int value);
void search_device_class_in_bus(int bus, int classID);
void printNumeric(unsigned int num);
void simple_print(const char* msg);
void console_clear();
void scroll();
char* itoa(int num, int base);
char* itoa(uint32_t num, int base);
//void simple_print(uint32_t num);
void simple_print(int num, int base=10);
void simple_print(uint32_t num, int base=10);
void WriteCharacter(unsigned char c, unsigned char forecolour, unsigned char backcolour, int x, int y);

// class print
// {
// private:
// 	/* data */
// public:
// 	print(/* args */);
// 	~print();
// 	void printNumeric(unsigned int num);
// 	static void simple_print(const char* msg);
// 	void simple_print(int num, int base=10);
// 	void simple_print(uint32_t num, int base=10);
// 	static void operator<<(const char* msg){
// 		simple_print(msg);
// 	}
// };

// print::print(/* args */)
// {
// }

// print::~print()
// {
// }



template <typename T>
void ins(short port, T *dest, unsigned int count) {
	asm volatile("rep ins%z2"
	             : "+D"(dest), "+c"(count), "=m"(*dest)
	             : "d"(port)
	             : "memory");
}

template <typename T>
void outl(uint32_t dir, T port) {
	asm volatile("outl %0,%w1"
	             :
	             : "a"(dir), "Nd"(port));
}