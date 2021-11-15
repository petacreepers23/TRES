#include <stdint.h>


#include "ahci.hpp"
#include "constants.hpp"


namespace tres {
/*
	The Advance Host Controller Interface (AHCI) is a controller designed
	to be a data movement engine between system memory and SATA devices.
	*/

/*  Frame Information Structure, the structure used to transport the info
	There are different types of FIS, depending on the host-device relation. */



	void ahci_version::set_version(uint32_t version){
		major =  (uint16_t) (version >> 16);
		minor =  (uint16_t) version;
	}
	void ahci_version::print(){
		simple_print(major);
		simple_print(".");
		simple_print(minor);
	}
/** METODOS DE LA CLASE AHCI.*/

/*
	Detect attached sata devices.
		1 a que puerto esta atacheao.
		2 que tipo de dispositivo.
	*/
	const char *type_of_attached_device[5] =
	{
		"NULL",
		"SATA",
		"SEMB",
		"PORT_MULTIPLIER",
		"SATAPI"
	};

bool check_status_ok(HBA_Port* port) {
	uint32_t sataStatus = port->ssts;

	uint8_t det = sataStatus & 0x0F;
	uint8_t ipm = (sataStatus >> 8) & 0x0F;
	simple_print(" det: ");
	simple_print(det);
	simple_print(" ipm: ");
	simple_print(ipm);
	if (det != 0x3 || ipm != 1) {
		simple_print(" false\n");
		return false;
	}
	
	//uint8_t spd = (sataStatus >> 4) & 0x0F;
	return true;
}

ATTACHED_DEVICE_TYPE get_port_attached_device(HBA_Port* port) {
	enum SATA_SIGNATURES {
		SIG_SATA  = 0x00000101,   // SATA drive
		SIG_ATAPI = 0xEB140101,  // SATAPI drive
		SIG_SEMB  = 0xC33C0101,   // Enclosure management bridge
		SIG_PM    = 0x96690101,     // Port multiplier
	};
	switch (port->sig) {
	case SIG_SATA:
		//simple_print(" SATA ");
		return ATTACHED_DEVICE_TYPE::SATA;
	case SIG_SEMB:
		//simple_print("SEMB, enclusre manag.. bridge");
		return ATTACHED_DEVICE_TYPE::SEMB;
	case SIG_PM:
		//simple_print(" portMultiplier ");
		return ATTACHED_DEVICE_TYPE::PORT_MULTIPLIER;
	case SIG_ATAPI:
		//simple_print(" atapi ");
		return ATTACHED_DEVICE_TYPE::SATAPI;
	default:
		//simple_print(" unknown ");
		return ATTACHED_DEVICE_TYPE::NULL;
	}
}


AHCI::AHCI(PCI_header_type_0x00* pci_header) {
	this->pci_header = pci_header;
	// en bar[5] se encuentra la direccion correspondiente al registro
	// HBA memory.
	simple_print("Configando la AHCI...\n");
	//HBA_Memory *abar = new HBA_Memory();
	abar = (HBA_Memory*)pci_header->bar[5];
	
}


/*La especificacion de la AHCI, nos indica que en el 
port implemented register o HBA_MEM.pi. Es un reg de 32 bits, donde 
cada bit si esta a 1, nos indica que esta unido a un dispositivo.

Vamos a escanear los 32 puertos que ofrece la HBA, el dispositivo físco que
implementa la especificacion AHCI.

Existen 4 posibles tipos de SATA. El registro port signature register
contiene la firma del dispositivo.
*/

void AHCI::scan_ports() {
	/**cada bit representa un port. 
		 * si 1 -> tiene dispositivo atacheado.
		 * si 0 -> no lo tiene.
		 * */
	const uint32_t pi = abar->pi;       // copia del reg pi(ports implemented)
	const uint32_t num_max_ports = 32;  // abar->cap.np+1;// CAP.NP, Number of ports. bits del 4-0.
	simple_print("\nPI: ");
	simple_print(pi);
	simple_print("Scan ports \n");
	// loop 32 ports and check bit.

	portCount = 0;
	for (uint32_t i = 0; i < num_max_ports; i++) {
		if (pi & (1 << i)) {
			simple_print("nPort:  ");
			simple_print(i);
			// check type.
			if (check_status_ok(&abar->port[i]) == false) {
				continue;
			}
			simple_print("-> Port attached: ");
			ATTACHED_DEVICE_TYPE attached_device_type = get_port_attached_device(&abar->port[i]);
			simple_print(type_of_attached_device[attached_device_type]);
			// dir = 100h+i*80h
			// configure ports
			if(attached_device_type==ATTACHED_DEVICE_TYPE::SATA){
				simple_print(" new port");
				ports[portCount] = new Port();
				ports[portCount]->portType = attached_device_type;
				ports[portCount]->hba_port = &abar->port[i];
				ports[portCount]->portNumber = portCount;
				portCount++;
			}
			simple_print(" \n");

			

		}
	}
}



void AHCI::configure_ports(){

	for (uint32_t i = 0; i < portCount; i++)
	{
		//Port* port = ports[i];
		//simple_print(ports[i]->hba_port->sig);
		simple_print(" \n");
		simple_print("configure port: ");
		simple_print(type_of_attached_device[ports[i]->portType]);
		simple_print("\n");
		ports[i]->internal_memory_allocation();
		simple_print("SATA inicializado.\n");
		ports[i]->hba_port->cmd|= 0x10;
		ports[i]->hba_port->cmd|= 0x1;
		ports[i]->hba_port->serr = 0x07ef0f03;

		// Prueba simple.  WRITE DATA
		char * buffer_read = (char*) aligned_new(1024,1024);
		//simple_print((uint32_t)buffer_read);
		memset(buffer_read, 0, 1024);
		
		int res = ports[i]->read(0,0,4,buffer_read);
		if(res==1){
			simple_print("todo bien:");
		}
		// for (int t = 0; t < 1024; t++){
        //         simple_print(buffer_read[t]);
		// }
		simple_print(buffer_read);
		simple_print((uint32_t)buffer_read);
		simple_print("fin\n");
		
		
	}
	
}



AHCI::~AHCI() {
}

void AHCI::print_hba_capabilities(){
	if(abar->cap & constants::HBA_Capabilities::SAM){
		simple_print("AHCI mode only\n");
	}
	if(abar->cap & constants::HBA_Capabilities::SSS)
	{
		simple_print("CAP.SSS is set.\n");
	}else
	{
		simple_print("CAP.SSS not set");
	}
	
}

bool AHCI::is_ahci_enabled(){
	return abar->ghc & (1 << 31);
}
bool AHCI::is_ahci_interrupt_enable(){
	return abar->ghc & (1 << 1);
}


void AHCI::init() {
	if(is_ahci_enabled()){
		simple_print("AHCI habilitada.\n");
	}
	if(is_ahci_interrupt_enable()){
		simple_print("AHCI interrupt enable.\n");
	}else{
		
		abar->ghc |= 1 << 1;
		if(is_ahci_interrupt_enable()){
			simple_print("AHCI interrupt enable aja.\n");
		}
	}
	version.set_version(abar->vs);
	version.print();
	print_hba_capabilities();
	scan_ports();
	configure_ports();
}
void AHCI::finish() {
}

void AHCI::write_to_device(void* address, byte* buf, size_t n) {}

void AHCI::read_from_device(void* address, byte* buf, size_t n) {}

}  // namespace tres
