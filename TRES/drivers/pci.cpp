

#include "pci.hpp"

#include "../kernel/kernel_functions.hpp"

// Si se lee de CONFIG_DAT se obtiene el contenido del registro PCI
// especificado en CONFIG_DIR.
// Si se escribe en CONFIG_DAT se modifica el contenido del registro PCI
// especificado en CONFIG_DIR.
#define CONFIG_DIR 0xCF8
#define CONFIG_DAT 0xCFC





void set_PCI_register(int bus, int slot, int func, int reg, unsigned int value) {
	unsigned int dir;
	dir = (unsigned int)((bus << 16) | (slot << 11) |
	                     (func << 8) | (reg & 0xfc) | ((unsigned int)0x80000000));

	// __asm__ __volatile__("outl %0,%w1"
	//                      :
	//                      : "a"(dir), "Nd"(CONFIG_DIR));
	// __asm__ __volatile__("outl %0,%w1"
	//                      :
	//                      : "a"(value), "Nd"(CONFIG_DAT));
	outl(dir,CONFIG_DIR);
	outl(value,CONFIG_DAT);
}

/*
|    31    |(30-24) |(23-16)|(15-11)  |(10-8)     |(7-2)      |(1-0)|

|Enable Bit|Reserved|BusNum |DeviceNum|FunctionNum|RegisterNum| 0 0 |
*/
uint32_t get_PCI_register(int bus, int slot, int func, int reg) {
	uint32_t dat;
	uint32_t enable_bit = 0x80000000;
	uint32_t dir = (uint32_t)((bus << 16) | (slot << 11) |
	                          (func << 8) | (reg & 0xfc) | enable_bit);

	// lectura del registro
	outl(dir,CONFIG_DIR);
	ins(CONFIG_DAT, &dat, 2);
	return dat;
}

int ls_pci() {
	unsigned int dir, dat, reg, clase, subclass;
	reg = 0x8;  // registro que contiene clase y subclase
	for (uint32_t bus = 0; bus < 256; bus++) {
		for (uint32_t device = 0; device < 32; device++) {
			// printDevice(bus, device)
			for (uint32_t f = 0; f < 7; f++) {
				dat = get_PCI_register(bus, device, f, 0x8);

				if (dat == 0xFFFFFFFF)
					continue;

				clase = dat >> 24;
				// extrae la subclase
				subclass = (dat >> 16) & 0xff;
				//unsigned int algo = 0x08000000;
				// if (clase == algo)
				// 	continue;

				//printf("Bus %device Dev %device Func %device: Clase %x Subclase %x\n",bus, device, f, clase, subclass);
				simple_print("Bus");
				simple_print(bus);

				simple_print("Dev");
				simple_print(device);

				simple_print("Fun");
				simple_print(f);

				simple_print("Cla");
				simple_print(clase);

				simple_print("Sub");
				simple_print(subclass);
				simple_print("--------------");
			}
		}
	}
	return 0;
}

// Obtiene la información de las regiones asignadas al dispositivo
// recibido como parámetro (bus|slot|func), que puede ser de tipo bridge
// (hasta 2 regiones) o no (hasta 6 regiones).
// Para obtener la dirección de inicio de la región asociada a un BAR,
// basta con leerlo.
// Una región puede estar incluida en el espacio de puertos (PIO)
// o en el de memoria (MMIO) según lo especifique el bit de menor peso
// de la dirección.
// En el caso de MMIO, la región puede tener asignadas direcciones
// de 32 o 64 bits según lo indique el segundo bit de menor peso.
// Para conocer el tamaño asignado a la región, hay que escribir en
// el BAR una palabra con todos los bits a 1, leer el registro obteniendo
// el tamaño y volver a escribir en el BAR el valor original (la dirección).
// Si el tamaño de una región, que debe ser potencia de 2, es X (2^k), la
// lectura del registro devuelve una palabra con los k bits de menor peso
// a 0 y los de mayor peso restantes a 1. Para obtener el tamaño, hay que
// intercambiar ceros y unos y sumar 1.
int print_regions(int bus, int slot, int func, int bridge) {
	uint32_t dat, datorig, tam;
	int PCI_register;
	unsigned long dir_reg;
	int io, i, num_reg, dir64 = 0;

	// bridges sólo tienen 2 regiones como máximo; resto hasta 6.
	num_reg = bridge ? 2 : 6;

	// registro que contiene el BAR0
	PCI_register = 0x10;

	// Cada iteración recupera la información de un BAR
	for (i = 0; i < num_reg; i++, PCI_register += 4) {
		// lee el BAR correspondiente a esta iteración
		// obtendrá la dirección inicial de la región
		datorig = dat = get_PCI_register(bus, slot, func, PCI_register);

		// me has engañado: no existe ese dispositivo
		if (dat == 0xFFFFFFFF)
			return 1;

		// los bits de menor peso de la dirección guardan info de:
		//    - si es una dirección de E/S tipo puerto (PIO)
		//      o tipo memoria (MMIO) en el bit de menor peso
		//    - en caso de ser de memoria, si es de 32 o 64 bits,
		//      en el segundo bit de menor peso

		// ¿PIO o MMIO? -> indicado por bit de menor peso
		if (dat & 0x00000001) {  // PIO
			// limpiamos los 2 bits de menos peso de la dirección
			dir_reg = dat & 0xFFFFFFFC;
			io = 1;  // es de PIO
		} else {     // MMIO
			// el segundo bit de menor peso indica nº bits de dir
			dir64 = (dat & 0x00000002) ? 1 : 0;  // ¿32 o 64 bits?

			// limpiamos los 4 bits de menos peso de la dirección
			dir_reg = dat & 0xFFFFFFF0;
			io = 0;  // es de MMIO
		}
		// valor 0 en la dirección indica que este BAR no está en uso
		if (dir_reg) {
			// tiene una región asignada: obtiene el tamaño
			// escribe 0xFFFFFFFF en el BAR
			set_PCI_register(bus, slot, func,
			                 PCI_register, 0xFFFFFFFF);

			// lee un valor a partir del cual se calcula el tamaño
			dat = get_PCI_register(bus, slot, func, PCI_register);

			// restaura el valor original (la dirección)
			set_PCI_register(bus, slot, func, PCI_register, datorig);

			// limpiamos como antes bits de menor peso de lo leído
			dat = io ? (dat & 0xFFFFFFFC) : (dat & 0xFFFFFFF0);

			// para calcular el tamaño:
			// intercambiamos 0s y 1s y sumamos 1
			tam = (~dat) + 1;

			// si es de 64 bits la parte alta de la dirección
			// está en el siguiente BAR
			// if (dir64) {
			// 	// obtiene la parte alta de la dirección
			// 	PCI_register+=4;
			// 	dat = get_PCI_register(bus, slot, func,
			// 		PCI_register);
			// 	// mezcla la parte baja y alta de la dirección
			// 	dir_reg |= ((uint64_t) dat) << 32;
			// }
			//printf("\tRegion %d (%s): dir %lx tam %d\n", i, io ? "IO" : "Mem", dir_reg, tam);
			simple_print("Region ");
			simple_print(i, 10);
			simple_print(io ? "(IO) " : "(Mem)");
			simple_print(": dir");
			simple_print(dir_reg, 10);
			simple_print(" tam ");
			simple_print(tam, 10);
			simple_print("\n");
		}
	}
	return 0;
}

// imprime la siguiente información de un dispositivo:
//	cod. vendedor, cod. producto, clase y subclase del dispositivo
int print_device(int bus, int slot, int func) {
	uint32_t dat;
	int PCI_register;
	int vend, prod, clase, subclass;
	int bridge, bus_secund;

	// dirección de registro que contiene vendedor y producto
	PCI_register = 0;
	dat = get_PCI_register(bus, slot, func, PCI_register);

	// no hay dispositivo en este "slot"
	if (dat == 0xFFFFFFFF)
		return 1;

	// extrae vendedor y producto
	vend = dat & 0x0000FFFF;
	prod = dat >> 16;

	// dirección de registro que contiene la clase y subclase
	PCI_register = 0x8;
	dat = get_PCI_register(bus, slot, func, PCI_register);

	// no hay dispositivo en este "slot"
	// ya comprobado: NO DEBERÍA OCURRIR
	if (dat == 0xFFFFFFFF)
		return 1;

	// extrae la clase
	clase = dat >> 24;
	// extrae la subclase
	subclass = (dat >> 16) & 0xff;

	//printf("Bus %d Slot %d Func %d: ID Vendedor %x ID Producto %x Clase %x Subclase %x\n", bus, slot, func, vend, prod, clase, subclass);
	simple_print("Bus ");
	simple_print(bus);
	simple_print(" Slot ");
	simple_print(slot);
	simple_print(" Func ");
	simple_print(func);
	simple_print(" ID Vendedor: ");
	simple_print(vend);
	simple_print(" ID Producto ");
	simple_print(prod);
	simple_print(" Clase ");
	simple_print(clase);
	simple_print(" Subclase ");
	simple_print(subclass);
	simple_print("\n");

	// comprobando si existen PCI bridges
	// debe leer el registro PCI que contiene el tipo de cabecera
	PCI_register = 0xC;
	dat = get_PCI_register(bus, slot, func, PCI_register);

	if (dat == 0xFFFFFFFF)
		return 1;

	bridge = (dat >> 16) & 0x7f;

	// imprime las regiones (MMIO o PIO) asociadas al dispositivo
	if (print_regions(bus, slot, func, bridge) < 0)
		return 1;

	// navega por el bus asociado al PCI-PCI bridge encontrado
	if (bridge) {
		// obteniendo bus secundario asociado al bridge
		PCI_register = 0x18;
		dat = get_PCI_register(bus, slot, func, PCI_register);
		if (dat == 0xFFFFFFFF)
			return 1;
		bus_secund = (dat >> 8) & 0xff;
		if (bus_secund)
			print_bus(bus_secund);
	}
	return 0;
}

int print_slot(int bus, int slot) {
	int f;
	int numfun;
	uint32_t dat;
	int PCI_register;
	// comprobando si es multifunción
	// debe leer el registro PCI que contiene el tipo de cabecera
	PCI_register = 0xC;
	dat = get_PCI_register(bus, slot, 0, PCI_register);

	if (dat == 0xFFFFFFFF)
		return 1;

	// ¿es multifunción?
	numfun = ((dat >> 16) & 0x80) ? 8 : 1;

	for (f = 0; f < numfun; f++)
		print_device(bus, slot, f);
	return 0;
}

void print_bus(int bus) {
	int d;
	// Máximo de 32 "slots" por bus
	for (d = 0; d < 32; d++)
		print_slot(bus, d);
}

void print_pci() {
	for (uint32_t bus = 0; bus < 256; bus++) {
		print_bus(bus);
	}
}



// TODO: create a struct for ahci, has a lot of details, probably a lot of them are really of pci, but, probably 

// ########################################################################






int get_general_info(uint32_t bus, uint32_t slot, uint32_t func){
	for (int offset = 0; offset < 4; offset+=4)
	{
		get_PCI_register(bus, slot, func, offset);
	}
	
}
// imprime la siguiente información de un dispositivo:
//	cod. vendedor, cod. producto, clase y subclase del dispositivo
int configure_device(uint32_t bus, uint32_t slot, uint32_t func, Print_info print_info) {
	uint32_t dat;
	int PCI_register;
	int vend, prod, clase, subclass;
	int bridge, bus_secund;
	PCI_common_part_header general_header;

	// dirección de registro que contiene vendedor y producto
	PCI_register = 0;
	dat = get_PCI_register(bus, slot, func, PCI_register);

	// no hay dispositivo en este "slot"
	if (dat == 0xFFFFFFFF)
		return 1;

	// extrae vendedor y producto
	vend = dat & 0x0000FFFF;
	prod = dat >> 16;

	general_header.vendor_id = vend;
	general_header.device_id = prod;
	// dirección de registro que contiene la clase y subclase
	PCI_register = 0x8;
	dat = get_PCI_register(bus, slot, func, PCI_register);

	// no hay dispositivo en este "slot"
	// ya comprobado: NO DEBERÍA OCURRIR
	if (dat == 0xFFFFFFFF)
		return 1;

	// extrae la clase
	clase = dat >> 24;
	// extrae la subclase
	subclass = (dat >> 16) & 0xff;

	//printf("Bus %d Slot %d Func %d: ID Vendedor %x ID Producto %x Clase %x Subclase %x\n", bus, slot, func, vend, prod, clase, subclass);
	simple_print("Bus ");
	simple_print(bus);
	simple_print(" Slot ");
	simple_print(slot);
	simple_print(" Func ");
	simple_print(func);
	simple_print(" ID Vendedor: ");
	simple_print(vend);
	simple_print(" ID Producto ");
	simple_print(prod);
	simple_print(" Clase ");
	simple_print(clase);
	simple_print(" Subclase ");
	simple_print(subclass);
	simple_print("\n");

	// comprobando si existen PCI bridges
	// debe leer el registro PCI que contiene el tipo de cabecera
	PCI_register = 0xC;
	dat = get_PCI_register(bus, slot, func, PCI_register);

	if (dat == 0xFFFFFFFF)
		return 1;

	bridge = (dat >> 16) & 0x7f; // bridge is the bit 31.

	// imprime las regiones (MMIO o PIO) asociadas al dispositivo
	if (print_regions(bus, slot, func, bridge) < 0)
		return 1;

	// navega por el bus asociado al PCI-PCI bridge encontrado
	if (bridge) {
		// obteniendo bus secundario asociado al bridge
		PCI_register = 0x18;
		dat = get_PCI_register(bus, slot, func, PCI_register);
		if (dat == 0xFFFFFFFF)
			return 1;
		bus_secund = (dat >> 8) & 0xff;
		if (bus_secund)
			print_bus(bus_secund);
	}
	return 0;
}

int configure_slot(uint32_t bus, uint32_t slot, Print_info print_info) {
	int numfun;
	uint32_t dat;
	int PCI_register;
	// comprobando si es multifunción
	// debe leer el registro PCI que contiene el tipo de cabecera
	PCI_register = 0xC;
	dat = get_PCI_register(bus, slot, 0, PCI_register);

	if (dat == 0xFFFFFFFF)
		return 1;

	// ¿es multifunción?
	numfun = ((dat >> 16) & 0x80) ? 8 : 1;

	for (int function = 0; function < numfun; function++)
		configure_device(bus, slot, function, print_info);
	return 0;
}

void configure_bus(uint32_t bus, Print_info print_info) {
	for (uint32_t d = 0; d < 32; d++) {
		configure_slot(bus, d, print_info);
	}
}

void configure_pci_devices(Print_info print_info) {
	for (uint32_t bus = 0; bus < 256; bus++) {
		configure_bus(bus, print_info);
	}
}
