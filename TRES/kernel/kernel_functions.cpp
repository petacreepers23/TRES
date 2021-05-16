
#include "kernel_functions.hpp"
static int dir_memoria_video = 0xB8000;

const static uint16_t _width = 80;
const static uint16_t _length = 0;
static uint16_t _current_width = 0;
static uint16_t _current_length = 0;
static int index = 0; // TODO: fix please, this is a ñapa.


void print(const char* msg, unsigned int size) {
	volatile char* mem_video = (volatile char*)dir_memoria_video;

	for (unsigned int i = 0; i < size; ++i) {
		mem_video[i * 2] = msg[i];
		mem_video[i * 2 + 1] = 15;
	}
	mem_video[size * 2] = ' ';
	mem_video[size * 2 + 1] = (char)15;
	dir_memoria_video += size * 2 + 2;
}


void simple_print(const char* msg, int a, int b) {
	volatile char* mem_video = (volatile char*)dir_memoria_video;
	while (*msg != 0) {
		*mem_video++ = *msg++;
		*mem_video++ = 15;
		index+=2;
		dir_memoria_video += 2;
		
		index=index%160;
	}
}

void simple_print(const char* msg) {
	volatile char* mem_video = (volatile char*)dir_memoria_video;
	//simple_print(index,10);
	int aux = 1;
	while (*msg != 0) {
		aux = 1;
		if (*msg == '\n') {
			mem_video+=(160-index);
			dir_memoria_video+=(160-index);
			index=2;
			mem_video+=2;
			dir_memoria_video+=2;
			msg++;

		} else {
			
			*mem_video++ = *msg++;
			*mem_video++ = 15;
			index+=2;
			dir_memoria_video += 2;
		}
		aux = 0;
		index=index%160;
	}
	//if(aux==0){	index-=2;}

	//dir_memoria_video-=2;
	//simple_print(index);
}
void simple_print(char msg) {
	volatile char* mem_video = (volatile char*)dir_memoria_video;
	*mem_video++ = msg;
	*mem_video = 15;
	dir_memoria_video += 2;
	index +=2;
}

// debería funcionar con los uints perfectamente
void simple_print(int num, int base) {
	static char buf[32] = {0};
	int i = 30;
	int sign;

	if ((sign = num) < 0) {
		num = -num;
	}

	do {
		buf[i] = "0123456789abcdef"[num % base];
		i--;
		num /= base;
	} while (num != 0 && i != 0);
	if (sign < 0) {  // Si num negativo metemos el -
		buf[i] = '-';
		i--;
	}
	simple_print(&buf[i + 1]);
}


/*Nums del 0-15*/
char auxNumeric(unsigned char num) {
	if (num > 15) {
		return '-';
	}
	if (num > 9) {
		return (num - 10) + 'A';
	} else {
		return num + '0';
	}
}
/*Nums de 32 bits*/
void printNumeric(unsigned int num) {
	char buffer[10];
	buffer[0] = '0';
	buffer[1] = 'x';

	union U {
		int num;
		unsigned char bytes[4];
	} u;
	u.num = num;

	for (int i = 0; i < 4; ++i) {
		buffer[i * 2 + 2] = auxNumeric(015 & u.bytes[i]);
		buffer[i * 2 + 3] = auxNumeric((240 & u.bytes[i]) >> 4);
	}

	print(buffer, 10);
}

void stop() {
	while (1)
		;
}

// Cosas de la PCI

// Se recomienda consultar:
// http://wiki.osdev.org/PCI

// puertos para la configuración de PCI:
//	CONFIG_DIR 0xCF8
//	CONFIG_DAT 0xCFC

// en CONFIG_DIR se especifica la dir. a leer/escribir: bus|slot|func|registro
// con el siguiente formato

/*
|    31    |(30-24) |(23-16)|(15-11)  |(10-8)     |(7-2)      |(1-0)|

|Enable Bit|Reserved|BusNum |DeviceNum|FunctionNum|RegisterNum| 0 0 |
*/
#define CONFIG_DIR 0xCF8

// Si se lee de CONFIG_DAT se obtiene el contenido del registro PCI
// especificado en CONFIG_DIR.
// Si se escribe en CONFIG_DAT se modifica el contenido del registro PCI
// especificado en CONFIG_DIR.
#define CONFIG_DAT 0xCFC

void search_device_class_in_bus(int bus, int classID);
/*
extern "C" int inl();
asm(R"(
.globl inl
    .type inl, @function
    inl:
    .cfi_startproc
	mov $0xCFC,%dx
    in %dx
    ret
    .cfi_endproc
)");*/
template <typename T>
void ins(short port, T* dest, unsigned int count) {
	asm volatile("rep ins%z2"
	             : "+D"(dest), "+c"(count), "=m"(*dest)
	             : "d"(port)
	             : "memory");
}


void set_PCI_register(int bus, int slot, int func, int reg, unsigned int value) {
	unsigned int dir;
	dir = (unsigned int)((bus << 16) | (slot << 11) |
	                     (func << 8) | (reg & 0xfc) | ((unsigned int)0x80000000));

	// escritura en el registro
	//outl(dir, CONFIG_DIR);

	__asm__ __volatile__("outl %0,%w1"
	                     :
	                     : "a"(dir), "Nd"(CONFIG_DIR));
	__asm__ __volatile__("outl %0,%w1"
	                     :
	                     : "a"(value), "Nd"(CONFIG_DAT));
	return;
}

// imprime la información de regiones de direcciones (MMIO o PIO)
// asignadas al dispositivo
// void print_regions(int bus, int slot, int func) {
// 	unsigned int dat, datorig;
// 	int PCI_register;
// 	unsigned long dir_reg;
// 	int io, i, tam, num_reg, dir64;
// 	// bridges sólo tienen 2 regiones como máximo; resto hasta 6.
// 	num_reg = 6;
// 	PCI_register = 0x10;
// 	for (i = 0; i < num_reg; i++, PCI_register += 4) {
// 		datorig = dat = get_PCI_register(bus, slot, func, PCI_register);

// busca esa clase de dispositivo en el bus especificado;
// se llama indirectamente de forma recursiva cuando se encuentran
// nuevos buses
void search_device_class_in_bus(int bus, int classID) {
	int slot;
	int f;
	int numfun;
	unsigned int dat;
	int PCI_register;

	// Comprueba si hay dispositivo en ese slot;
	// Máximo de 32 "slots" por bus
	for (slot = 0; slot < 32; slot++) {
		// comprobando si hay dispositivo y si es multifunción
		// debe leer el registro PCI que contiene el tipo de cabecera
		PCI_register = 0xC;
		//dat = get_PCI_register(bus, slot, 0, PCI_register);
		print("Devuelve:", 10);
		printNumeric(dat);

		// no hay dispositivo; paso al siguiente slot
		if (dat == 0xFFFFFFFF) continue;

		// ¿es multifunción?
		// si es multifunción, puede haber hasta 8 funciones/slot
		numfun = ((dat >> 16) & 0x80) ? 8 : 1;

		// por cada función, comprueba si es de esa clase
		// for (f = 0; f < numfun; f++)
		// 	check_class(bus, slot, f, classID);
	}
}
