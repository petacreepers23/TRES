
#include "kernel_functions.hpp"
#include "../drivers/pci.hpp"

int main() {
	//print((char*)"Hola",4);
	//int classID=0x01; // controlador de dispositivo de almacenamiento
	// search_device_class_in_bus(0, classID);

	/*const char* string = "hola hola";
	const char colour = 15;
	volatile char* mem_video = (volatile char*)0xB8000;

	while (*string != 0) {
		*mem_video++ = *string++;
		*mem_video++ = colour;
	}
	while(1);
	*/

	// int a = 0x12345678;
    // unsigned char *c = (unsigned char*)(&a);
    // if (*c == 0x78) {
    //    print((char*)"little-endian",13);
    // } else {
    //    print((char*)"big-endian",10);
    // }
	

	print_pci();
	//simple_print("ashola mundo\n\n123456789\n||\nbuenos\n123456789\n||\nbuenos");
	// simple_print("\nabc\n||\nfg");
	// // simple_print("123456789\n||\nbuenos");
	// uint64_t a = 25416584;
	// simple_print(a);
	// simple_print("||");
	// int b = 5;
	// simple_print(-999);
	// simple_print(0);
	// simple_print(a);
	//printNumeric(b);

	// print("otro print",10);
	// print("y otro mas",10);
	stop();
}