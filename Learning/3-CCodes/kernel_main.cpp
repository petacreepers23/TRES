
#include "kernel_functions.h"


int main() {
	print((char*)"Hola",4);
	int classID=0x01; // controlador de dispositivo de almacenamiento
	search_device_class_in_bus(0, classID);
	stop();
	/*const char* string = "hola hola";
	const char colour = 15;
	volatile char* mem_video = (volatile char*)0xB8000;

	while (*string != 0) {
		*mem_video++ = *string++;
		*mem_video++ = colour;
	}
	while(1);
	*/
}