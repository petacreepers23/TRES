#pragma once
#include "../../kernel/kernel_functions.hpp" // simple print
#include "../../lib/basic_allocator.hpp"
#include "../driver_base.hpp"
#include "../pci.hpp"
#include "../../lib/types.hpp"
#include "hba_headers.hpp"
#include "fis_headers.hpp"

#include "port.hpp"

namespace tres{








	class AHCI : public drivers::driver_base {
	private:
		/* data */
		PCI_header_type_0x00* pci_header;
	public:
		AHCI(PCI_header_type_0x00* pci_header);
		~AHCI();
		void scan_ports(HBA_Memory *abar);
		/*
		A traves de abar se puede acceder a los registros de memoria de la HBA acceciendo tanto a las estructuras
		Generic Host Control como Port Registers
		*/
		HBA_Memory *abar;
		/*Has a pointer to the ports that are activated in other case */
		Port* ports[32];
		uint32_t portCount;
		
		void init();
		void write_to_device(void* address, byte* buf, size_t n);
		void finish();
		void read_from_device(void* address, byte* buf, size_t n);
		void configure_ports();
	};


/*
TODO: 
Implement
Reset 
	software reset
	port reset
	hardware reset*/



}