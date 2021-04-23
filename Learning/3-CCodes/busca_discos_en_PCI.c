// busca dispositivos de almacenamiento de un bus PCI e imprime
// la información de los mismos
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/io.h>


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

uint32_t get_PCI_register(int bus, int slot, int func, int reg) {
	uint32_t dir;
	dir = (uint32_t)((bus << 16) | (slot << 11) |
              (func << 8) | (reg & 0xfc) | ((uint32_t)0x80000000));
	printf("Direccion: %d",dir);

	// lectura del registro
        outl (dir, CONFIG_DIR);
        return inl(CONFIG_DAT);
}
void set_PCI_register(int bus, int slot, int func, int reg, uint32_t value) {
	uint32_t dir;
	dir = (uint32_t)((bus << 16) | (slot << 11) |
              (func << 8) | (reg & 0xfc) | ((uint32_t)0x80000000));

	// escritura en el registro
        outl (dir, CONFIG_DIR);
        outl(value, CONFIG_DAT);
}

// imprime la información de regiones de direcciones (MMIO o PIO)
// asignadas al dispositivo
void print_regions(int bus, int slot, int func) {
        uint32_t dat, datorig;
        int PCI_register;
        unsigned long dir_reg;
        int io, i, tam, num_reg, dir64;
        // bridges sólo tienen 2 regiones como máximo; resto hasta 6.
        num_reg = 6;
        PCI_register = 0x10;
        for (i=0; i<num_reg; i++, PCI_register+=4) {
                datorig = dat = get_PCI_register(bus, slot, func, PCI_register);

		// no debería ocurrir
                if (dat == 0xFFFFFFFF) return;

                dir64=0;
                // ¿IO o MMIO?
                if (dat & 0x00000001) { // IO
                        dir_reg=dat & 0xFFFFFFFC;
                        io=1;
                }
                else { // MMIO
                        dir64 =  (dat & 0x00000002)? 1 : 0; // ¿32 o 64 bits?
                        dir_reg=dat & 0xFFFFFFF0;
                        io=0;
                }
                if (dir_reg) { // tiene esa región asignada: obtener el tamaño
                        // escribe 0xFFFFFFFF para obtener el tamaño
                        set_PCI_register(bus, slot, func,
                                PCI_register,0xFFFFFFFF);
                        dat = get_PCI_register(bus, slot, func, PCI_register);
                        // restaura el valor original
                        set_PCI_register(bus, slot, func, PCI_register,datorig);
                        dat = io ? (dat & 0xFFFFFFFC) : (dat & 0xFFFFFFF0);
                        // calcula el tamaño de la región
                        tam=1;
                        while ((dat & 0x00000001) == 0){
                                dat>>=1;
                                tam<<=1;
                        }
                        if (dir64) { // obtiene la parte alta de la dirección
                                PCI_register+=4;
                                dat = get_PCI_register(bus, slot, func,
                                        PCI_register);
                                dir_reg |= ((uint64_t) dat) << 32;
                        }
                        printf("\tRegion %d (%s): dir %lx tam %d\n", i,
                                io ? "IO" : "Mem", dir_reg, tam);
                }
        }
}


// comprueba si es de la clase buscada; en caso afirmativo, imprime
// la información del dispositivo;
// si no lo es pero es un brigde, comprueba recursivamente los buses
// secundarios que se encuentra
void check_class(int bus, int slot, int func, int classID) {
        uint32_t dat;
	int PCI_register;
	int vend, prod, class, subclass;
	int bridge, bus_secund;
	
	// dirección de registro que contiene clase y subclase
	PCI_register = 0x08;
	dat = get_PCI_register(bus, slot, func, PCI_register);

	// no hay dispositivo en este "slot"; no debería ocurrir
	if (dat == 0xFFFFFFFF) return;

	// extrae clase y subclase
	class = (dat & 0xFF000000) >> 24;
	subclass = (dat & 0x00FF0000) >> 16;

	// es de la clase buscada
	if (class==classID) {
	        // dirección de registro que contiene vendedor y producto
        	PCI_register = 0;
        	dat = get_PCI_register(bus, slot, func, PCI_register);

		// no hay dispositivo en este "slot"; no debería ocurrir
        	if (dat == 0xFFFFFFFF) return;

        	// extrae vendedor y producto
        	vend = dat & 0x0000FFFF;
        	prod = dat>>16;

		printf("Dispositivo de la clase buscada (%x):\n\tVendedor %x\n\tProducto %x\n\tsubclase %x\n\tbus %x slot %x función %x\n",
			class, vend, prod, subclass, bus, slot, func);
		print_regions(bus, slot, func);
	} else {
		// no es de la clase buscada; comprobamos si es un PCI bridge
		// debe leer el registro PCI que contiene el tipo de cabecera
		PCI_register = 0xC;
		dat = get_PCI_register(bus, slot, func, PCI_register);

		// no hay dispositivo en este "slot"; no debería ocurrir
		if (dat == 0xFFFFFFFF) return;

		bridge = (dat>>16) & 0x7f;

		// navega por el bus asociado al PCI-PCI bridge encontrado
		if (bridge) {
			// obteniendo bus secundario asociado al bridge
			PCI_register = 0x18;
			dat = get_PCI_register(bus, slot, func, PCI_register);
			if (dat == 0xFFFFFFFF) return;
			bus_secund = (dat>>8) & 0xff;
			if (bus_secund)
				search_device_class_in_bus(bus_secund, classID);
		}
	}
}
// busca esa clase de dispositivo en el bus especificado;
// se llama indirectamente de forma recursiva cuando se encuentran
// nuevos buses
void search_device_class_in_bus(int bus, int classID) {
	int slot;
	int f;
	int numfun;
        uint32_t dat;
	int PCI_register;

	// Comprueba si hay dispositivo en ese slot;
	// Máximo de 32 "slots" por bus
	for (slot=0; slot<32; slot++) {
		// comprobando si hay dispositivo y si es multifunción
		// debe leer el registro PCI que contiene el tipo de cabecera
		PCI_register = 0xC;
		dat = get_PCI_register(bus, slot, 0, PCI_register);

		// no hay dispositivo; paso al siguiente slot
		if (dat == 0xFFFFFFFF) continue;

		// ¿es multifunción?
		// si es multifunción, puede haber hasta 8 funciones/slot
		numfun = ((dat>>16) & 0x80) ? 8 : 1;

		// por cada función, comprueba si es de esa clase
		for (f=0; f<numfun; f++)
			check_class(bus,slot,f,classID);
	}
}

int main(int argc, char *argv[]) {
	int classID=0x01; // controlador de dispositivo de almacenamiento

	// permiso para poder acceder a los dos puertos en modo usuario
	if (ioperm(CONFIG_DIR, 8, 1) < 0) {
                perror("ioperm");
                return 1;
        }
	// empieza la búsqueda en el bus 0
 	search_device_class_in_bus(0, classID);
	return 0;
}
