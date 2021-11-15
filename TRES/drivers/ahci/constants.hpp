#include <stdint.h>






namespace constants {

    enum SATA_SIGNATURES_ {
		SIG_SATA  = 0x00000101,   // SATA drive
		SIG_ATAPI = 0xEB140101,  // SATAPI drive
		SIG_SEMB  = 0xC33C0101,   // Enclosure management bridge
		SIG_PM    = 0x96690101,     // Port multiplier
	};

    enum  HBA_Capabilities : uint32_t {
        S64A = (uint32_t)1 << 31, /* Supports 64-bit Addressing */
        SNCQ = 1 << 30,      /* Supports Native Command Queuing */
        SSNTF = 1 << 29,     /* Supports SNotification Register */
        SMPS = 1 << 28,      /* Supports Mechanical Presence Switch */
        SSS = 1 << 27,       /* Supports Staggered Spin-up */
        SALP = 1 << 26,      /* Supports Aggressive Link Power Management */
        SAL = 1 << 25,       /* Supports Activity LED */
        SCLO = 1 << 24,      /* Supports Command List Override */
        SAM = 1 << 18,       /* Supports AHCI mode only */
        SPM = 1 << 17,       /* Supports Port Multiplier */
        FBSS = 1 << 16,      /* FIS-based Switching Supported */
        PMD = 1 << 15,       /* PIO Multiple DRQ Block */
        SSC = 1 << 14,       /* Slumber State Capable */
        PSC = 1 << 13,       /* Partial State Capable */
        CCCS = 1 << 7,       /* Command Completion Coalescing Supported */
        EMS = 1 << 6,        /* Enclosure Management Supported */
        SXS = 1 << 5         /* Supports External SATA */
    };
 
    enum HBA_Capabilities2: uint32_t {
        DESO = 1 << 5, /* DevSleep Entrance from Slumber Only */
        SADM = 1 << 4, /* Supports Aggressive Device Sleep Management */
        SDS = 1 << 3,  /* Supports Device Sleep */
        APST = 1 << 2, /* Automatic Partial to Slumber Transitions */
        NVMP = 1 << 1, /* NVMHCI Present */
        BOH = 1 << 0,  /* BIOS/OS Handoff */
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

