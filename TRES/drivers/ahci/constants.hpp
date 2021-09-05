#include <stdint.h>






namespace constants {

    enum SATA_SIGNATURES_ {
		SIG_SATA  = 0x00000101,   // SATA drive
		SIG_ATAPI = 0xEB140101,  // SATAPI drive
		SIG_SEMB  = 0xC33C0101,   // Enclosure management bridge
		SIG_PM    = 0x96690101,     // Port multiplier
	};

    const uint32_t num_max_ports = 32; // max number of ports that a hba can have without ports multipliers

    // Port x Interrupt Status Register
    const uint32_t HBA_PxIS_TFES_bit = (1 << 30); // Task File error status -> bit 30



    // Port x Command and Status.
    const uint32_t HBA_PxCMD_ST_bit  = 0x0001; // Start -> bit 0
    const uint32_t HBA_PxCMD_FRE_bit = 0x0010; // FIS receive enable -> bit 4
    const uint32_t HBA_PxCMD_FR_bit  = 0x4000; // FIS receive running -> bit 14
    const uint32_t HBA_PxCMD_CR_bit  = 0x8000; // Command list running -> bit 15



}

