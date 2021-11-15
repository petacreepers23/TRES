

#include "pci.hpp"

#include "ahci/ahci.hpp"



void pci::configure_pci_devices() {
	for (uint32_t bus = 0; bus < 256; bus++) {
		configure_bus(bus);
	}
}

void pci::configure_bus(const uint32_t bus) {
	for (uint32_t d = 0; d < 32; d++) {
		configure_slot(bus, d);
	}
}

int pci::configure_slot(const uint32_t bus, const uint32_t slot) {
	int numfun;
	uint32_t dat;
	int PCI_register;
	const int multifuncion = 0x80;
	// comprobando si es multifunción
	// debe leer el registro PCI que contiene el tipo de cabecera
	PCI_register = 0xC;
	dat = get_PCI_register(bus, slot, 0, PCI_register);
	if (dat == 0xFFFFFFFF){
		return 1;
	}

	// ¿es multifunción?
	numfun = ((dat >> 16) & multifuncion) ? 8 : 1;

	for (int function = 0; function < numfun; function++) {
		//simple_print("new pci header");
		PCI_header * pci_header = new PCI_header();
		if(get_header_info(pci_header,bus,slot,function)==-1){
			continue;
		}
		pci_header->print_generic();
		
		// CONFIGURAMOS EL DRIVERS
		make_pci_device(pci_header);

		//get_pci_general_info(bus, slot, function, print_info);
	}
	return 0;
}

/**
 * Sets  the value in a PCI register. 
*/
void pci::set_PCI_register(int bus, int slot, int func, int reg, unsigned int value) {
	uint32_t enable_bit = 0x80000000;
	uint32_t dir = (unsigned int)((bus << 16) | (slot << 11) |
	                     (func << 8) | (reg & 0xfc) | enable_bit);
	outl(dir, CONFIG_DIR);
	outl(value, CONFIG_DAT);
}

/*
|    31    |(30-24) |(23-16)|(15-11)  |(10-8)     |(7-2)      |(1-0)|

|Enable Bit|Reserved|BusNum |DeviceNum|FunctionNum|RegisterNum| 0 0 |
*/
uint32_t pci::get_PCI_register(int bus, int slot, int func, int reg) {
	uint32_t dat;
	uint32_t enable_bit = 0x80000000;
	uint32_t dir = (uint32_t)((bus << 16) | (slot << 11) |
	                          (func << 8) | (reg & 0xfc) | enable_bit);
	outl(dir, CONFIG_DIR);  // lectura del registro
	ins(CONFIG_DAT, &dat, 2);
	simple_print("");// TODO: find real sol
	return dat;
}

int pci::get_header_info(PCI_header *pci_header, int bus, int dev, int func)
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


/*
Recibe una estructura generica, con la info necesaria.
Detecta el dispositivo, crea la estructura de turno y llama a  la clase del driver.
Que lo configura.
*/
void pci::make_pci_device(PCI_header *pci_header) {
	//char  device_types [255][][];
	//uint8_t class_code, uint8_t subclass, uint8_t prog_if,
	// TODO: esto tendrían que ser punteros a funciones, pero es una cutre prueba.
	//simple_print("aquiss: ");
	//simple_print(pci_header->class_code);
	if (pci_header->class_code == 1) {
		if (pci_header->subclass == 6) {
			PCI_header_type_0x00 *ahci_header = (PCI_header_type_0x00*)pci_header;
			tres::AHCI* ahci_configuration = new tres::AHCI(ahci_header);
			ahci_configuration->init();
		}
	}
}

