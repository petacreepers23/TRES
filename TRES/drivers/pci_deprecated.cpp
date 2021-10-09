

#include "pci_deprecated.hpp"

#include "ahci/ahci.hpp"

// Si se lee de CONFIG_DAT se obtiene el contenido del registro PCI
// especificado en CONFIG_DIR.
// Si se escribe en CONFIG_DAT se modifica el contenido del registro PCI
// especificado en CONFIG_DIR.
#define CONFIG_DIR 0xCF8
#define CONFIG_DAT 0xCFC

prueba::prueba(int a , int b)
{
	this->a=a;
	this->b=b;
}

prueba::~prueba()
{
}
void prueba::print(){
	simple_print(a); 
	simple_print(b);
}

/**
 * Sets  the value in a PCI register. 
*/
void set_PCI_register(int bus, int slot, int func, int reg, unsigned int value) {
	unsigned int dir;
	dir = (unsigned int)((bus << 16) | (slot << 11) |
	                     (func << 8) | (reg & 0xfc) | ((unsigned int)0x80000000));
	outl(dir, CONFIG_DIR);
	outl(value, CONFIG_DAT);
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
	outl(dir, CONFIG_DIR);  // lectura del registro
	ins(CONFIG_DAT, &dat, 2);
	return dat;
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
			if (dir64) {
				// obtiene la parte alta de la dirección
				PCI_register += 4;
				dat = get_PCI_register(bus, slot, func,
				                       PCI_register);
				// mezcla la parte baja y alta de la dirección
				dir_reg |= ((uint64_t)dat) << 32;
			}
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

// TODO: create a struct for ahci, has a lot of details, probably a lot of them are really of pci, but, probably

// ########################################################################

const char *class_mass_storage_controller_names[10] = {
    "SCSI bus controller",
    "IDE controller",
    "floppy disk controller",
    "IPI bus controller",
    "RAID controller",
    "ATA Controller",
    "Serial ATA",
    "Serial Attached SCSI",
    "Non-Volatile Memory Controller",
    "Other"};
const char *class_bridge_device_names[13] = {
    "host bridge",
    "ISA bridge",
    "EISA bridge",
    "MCA bridge",
    "PCI-to-PCI bridge",
    "Subtractive decode PCI-to-PCI bridge",
    "PCMCIA bridge",
    "NuBus bridge",
    "CardBus bridge",
    "RACEway bridge",
    "PCI-to-PCI Bridge",
    "InfiniBand-to-PCI Host Bridge",
    "Other"};

/*
Recibe una estructura generica, con la info necesaria.
Detecta el dispositivo, crea la estructura de turno y llama a  la clase del driver.
Que lo configura.
*/
void configure_pci_device(PCI_header *pci_header) {
	//char  device_types [255][][];
	//uint8_t class_code, uint8_t subclass, uint8_t prog_if,
	// TODO: esto tendrían que ser punteros a funciones, pero es una cutre prueba.
	//simple_print("aquiss: ");
	//simple_print(pci_header->class_code);
	if (pci_header->class_code == 1) {
		if (pci_header->subclass == 6) {
			//PCI_header_type_0x00 *ahci_header = new PCI_header_type_0x00();
			PCI_header_type_0x00 *ahci_header = (PCI_header_type_0x00*)pci_header;
			tres::AHCI* ahci_configuration = new tres::AHCI(ahci_header);
			ahci_configuration->init();
		}
	}
}







static int get_header_info(PCI_header *pci_header, int bus, int dev, int func)
{
	uint32_t *pci_register = (uint32_t*)pci_header;
	*pci_register++ = get_PCI_register(bus, dev, func, 0);
	if(pci_header->vendor_id == 0xFFFF) {
		return -1;
	}

	for(int offset=4; offset<=0x3C; offset+=4) {
		*pci_register++ = get_PCI_register(bus, dev, func, offset);
	}
	return 0;
}



int configure_slot(const uint32_t bus, const uint32_t slot, const Print_info print_info) {
	int numfun;
	uint32_t dat;
	int PCI_register;
	const int multifuncion = 0x80;
	// comprobando si es multifunción
	// debe leer el registro PCI que contiene el tipo de cabecera
	PCI_register = 0xC;
	dat = get_PCI_register(bus, slot, 0, PCI_register);
	if (dat == 0xFFFFFFFF)
		return 1;

	// ¿es multifunción?
	numfun = ((dat >> 16) & multifuncion) ? 8 : 1;

	for (int function = 0; function < numfun; function++) {
		//PCI_common_part_header *general_header = new PCI_common_part_header();
		PCI_header * pci_header = new PCI_header();
		if(get_header_info(pci_header,bus,slot,function)==-1){
			continue;
		}
		pci_header->print_generic();
		
		// CONFIGURAMOS EL DRIVERS
		configure_pci_device(pci_header);

		//get_pci_general_info(bus, slot, function, print_info);
	}
	return 0;
}

/*
	given a bus, loops through all the slots. and configure them.

*/
void configure_bus(const uint32_t bus, const Print_info print_info) {
	for (uint32_t d = 0; d < 32; d++) {
		configure_slot(bus, d, print_info);
	}
}

void configure_pci_devices(const Print_info print_info) {
	for (uint32_t bus = 0; bus < 256; bus++) {
		configure_bus(bus, print_info);
	}
}
// configure_pci_devices->configure_bus->configure_slot->configure_device



// DEPRECATED o NEEDS REVISION.
// NO ELIMINAR, que hay q revisar lo de los bridges
// imprime la siguiente información de un dispositivo:
//	cod. vendedor, cod. producto, clase y subclase del dispositivo
int get_pci_general_info(const uint32_t bus, const uint32_t slot, const uint32_t func, const Print_info print_info) {
	uint32_t dat;
	int PCI_register;
	//int vend, prod, clase, subclass;
	int bridge, bus_secund;
	PCI_header *general_header = new PCI_header();

	// dirección de registro que contiene vendedor y producto
	PCI_register = 0;
	dat = get_PCI_register(bus, slot, func, PCI_register);
	// no hay dispositivo en este "slot"
	if (dat == 0xFFFFFFFF)
		return false;
	// extrae vendedor y producto
	general_header->vendor_id = (dat & 0x0000FFFF);
	general_header->device_id = (dat >> 16);

	// extrae status y command
	PCI_register = 0x4;
	dat = get_PCI_register(bus, slot, func, PCI_register);
	general_header->command = dat & 0x0000FFFF;
	general_header->status = dat >> 16;

	// extrae la clase , subclase, prog if y revision id
	PCI_register = 0x8;  // dirección de registro que contiene la clase y subclase
	dat = get_PCI_register(bus, slot, func, PCI_register);

	// ya comprobado: NO DEBERÍA OCURRIR
	// if (dat == 0xFFFFFFFF) // no hay dispositivo en este "slot"
	// 	return 1;
	general_header->class_code = dat >> 24;
	general_header->subclass = (dat >> 16) & 0xff;
	general_header->prog_if = (dat & 0xF0) >> 4;
	general_header->revision_id = (dat & 0xF);

	//printf("Bus %d Slot %d Func %d: ID Vendedor %x ID Producto %x Clase %x Subclase %x\n", bus, slot, func, vend, prod, clase, subclass);
	// simple_print("Bus ");
	// simple_print(bus);
	// simple_print(" Slot ");
	// simple_print(slot);
	// simple_print(" Func ");
	// simple_print(func);
	// simple_print(" ID Vendedor: ");
	// simple_print(vend);
	// simple_print(" ID Producto ");
	// simple_print(prod);
	// simple_print(" Clase ");
	// simple_print(clase);
	// simple_print(" Subclase ");
	// simple_print(subclass);
	// simple_print("\n");
	general_header->print_generic();

	// comprobando si existen PCI bridges
	// debe leer el registro PCI que contiene el tipo de cabecera
	PCI_register = 0xC;
	dat = get_PCI_register(bus, slot, func, PCI_register);
	if (dat == 0xFFFFFFFF)
		return 1;
	bridge = (dat >> 16) & 0x7f;  // bridge is the BIST except the 31 bit.

	// imprime las regiones (MMIO o PIO) asociadas al dispositivo
	// if (print_regions(bus, slot, func, bridge) < 0)
	// 	return 1;

	// navega por el bus asociado al PCI-PCI bridge encontrado
	if (bridge) {
		// obteniendo bus secundario asociado al bridge
		PCI_register = 0x18;
		dat = get_PCI_register(bus, slot, func, PCI_register);
		if (dat == 0xFFFFFFFF)
			return 1;
		bus_secund = (dat >> 8) & 0xff;
		if (bus_secund)
			configure_bus(bus_secund, print_info);
	}

	// CONFIGURAMOS EL DRIVERS
	//configure_pci_device(general_header);
	return 0;
}