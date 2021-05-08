
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
	
	// ls_pci();
	print_pci();

	// print("otro print",10);
	// print("y otro mas",10);
	stop();
}