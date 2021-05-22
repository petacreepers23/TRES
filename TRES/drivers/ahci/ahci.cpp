#include <inttypes.h>

#include "../driver_base.hpp"

#include "../pci.hpp"

/*
The Advance Host Controller Interface (AHCI) is a controller designed
to be a data movement engine between system memory and SATA devices.
*/

/*  Frame Information Structure, the structure used to transport the info
 There are different types of FIS, depending on the host-device relation. */
typedef enum {
	FIS_TYPE_REG_H2D = 0x27,    // Register FIS - host to device
	FIS_TYPE_REG_D2H = 0x34,    // Register FIS - device to host
	FIS_TYPE_DMA_ACT = 0x39,    // DMA activate FIS - device to host
	FIS_TYPE_DMA_SETUP = 0x41,  // DMA setup FIS - bidirectional
	FIS_TYPE_DATA = 0x46,       // Data FIS - bidirectional
	FIS_TYPE_BIST = 0x58,       // BIST activate FIS - bidirectional
	FIS_TYPE_PIO_SETUP = 0x5F,  // PIO setup FIS - device to host
	FIS_TYPE_DEV_BITS = 0xA1,   // Set device bits FIS - device to host
} FIS_TYPE;

// HOST TO DEVICE
typedef struct tagFIS_REG_H2D {
	// DWORD 0
	uint8_t fis_type;  // FIS_TYPE_REG_H2D

	uint8_t pmport : 4;  // Port multiplier
	uint8_t rsv0 : 3;    // Reserved
	uint8_t c : 1;       // 1: Command, 0: Control

	uint8_t command;   // Command register
	uint8_t featurel;  // Feature register, 7:0

	// DWORD 1
	uint8_t lba0;    // LBA low register, 7:0
	uint8_t lba1;    // LBA mid register, 15:8
	uint8_t lba2;    // LBA high register, 23:16
	uint8_t device;  // Device register

	// DWORD 2
	uint8_t lba3;      // LBA register, 31:24
	uint8_t lba4;      // LBA register, 39:32
	uint8_t lba5;      // LBA register, 47:40
	uint8_t featureh;  // Feature register, 15:8

	// DWORD 3
	uint8_t countl;   // Count register, 7:0
	uint8_t counth;   // Count register, 15:8
	uint8_t icc;      // Isochronous command completion
	uint8_t control;  // Control register

	// DWORD 4
	uint8_t rsv1[4];  // Reserved
} FIS_REG_H2D;

// DEVICE TO HOST
typedef struct tagFIS_REG_D2H {
	// DWORD 0
	uint8_t fis_type;  // FIS_TYPE_REG_D2H

	uint8_t pmport : 4;  // Port multiplier
	uint8_t rsv0 : 2;    // Reserved
	uint8_t i : 1;       // Interrupt bit
	uint8_t rsv1 : 1;    // Reserved

	uint8_t status;  // Status register
	uint8_t error;   // Error register

	// DWORD 1
	uint8_t lba0;    // LBA low register, 7:0
	uint8_t lba1;    // LBA mid register, 15:8
	uint8_t lba2;    // LBA high register, 23:16
	uint8_t device;  // Device register

	// DWORD 2
	uint8_t lba3;  // LBA register, 31:24
	uint8_t lba4;  // LBA register, 39:32
	uint8_t lba5;  // LBA register, 47:40
	uint8_t rsv2;  // Reserved

	// DWORD 3
	uint8_t countl;   // Count register, 7:0
	uint8_t counth;   // Count register, 15:8
	uint8_t rsv3[2];  // Reserved

	// DWORD 4
	uint8_t rsv4[4];  // Reserved
} FIS_REG_D2H;

// Bidirectional for data payload.
typedef struct tagFIS_DATA {
	// DWORD 0
	uint8_t fis_type;  // FIS_TYPE_DATA

	uint8_t pmport : 4;  // Port multiplier
	uint8_t rsv0 : 4;    // Reserved

	uint8_t rsv1[2];  // Reserved

	// DWORD 1 ~ N
	uint32_t data[1];  // Payload
} FIS_DATA;

typedef struct tagFIS_PIO_SETUP {
	// DWORD 0
	uint8_t fis_type;  // FIS_TYPE_PIO_SETUP

	uint8_t pmport : 4;  // Port multiplier
	uint8_t rsv0 : 1;    // Reserved
	uint8_t d : 1;       // Data transfer direction, 1 - device to host
	uint8_t i : 1;       // Interrupt bit
	uint8_t rsv1 : 1;

	uint8_t status;  // Status register
	uint8_t error;   // Error register

	// DWORD 1
	uint8_t lba0;    // LBA low register, 7:0
	uint8_t lba1;    // LBA mid register, 15:8
	uint8_t lba2;    // LBA high register, 23:16
	uint8_t device;  // Device register

	// DWORD 2
	uint8_t lba3;  // LBA register, 31:24
	uint8_t lba4;  // LBA register, 39:32
	uint8_t lba5;  // LBA register, 47:40
	uint8_t rsv2;  // Reserved

	// DWORD 3
	uint8_t countl;    // Count register, 7:0
	uint8_t counth;    // Count register, 15:8
	uint8_t rsv3;      // Reserved
	uint8_t e_status;  // New value of status register

	// DWORD 4
	uint16_t tc;      // Transfer count
	uint8_t rsv4[2];  // Reserved
} FIS_PIO_SETUP;

typedef struct tagFIS_DMA_SETUP {
	// DWORD 0
	uint8_t fis_type;  // FIS_TYPE_DMA_SETUP

	uint8_t pmport : 4;  // Port multiplier
	uint8_t rsv0 : 1;    // Reserved
	uint8_t d : 1;       // Data transfer direction, 1 - device to host
	uint8_t i : 1;       // Interrupt bit
	uint8_t a : 1;       // Auto-activate. Specifies if DMA Activate FIS is needed

	uint8_t rsved[2];  // Reserved

	//DWORD 1&2

	uint64_t DMAbufferID;  // DMA Buffer Identifier. Used to Identify DMA buffer in host memory. SATA Spec says host specific and not in Spec. Trying AHCI spec might work.

	//DWORD 3
	uint32_t rsvd;  //More reserved

	//DWORD 4
	uint32_t DMAbufOffset;  //Byte offset into buffer. First 2 bits must be 0

	//DWORD 5
	uint32_t TransferCount;  //Number of bytes to transfer. Bit 0 must be 0

	//DWORD 6
	uint32_t resvd;  //Reserved

} FIS_DMA_SETUP;






// ....................................................................
typedef volatile struct tagHBA_MEM
{
	// 0x00 - 0x2B, Generic Host Control
	uint32_t cap;		// 0x00, Host capability
	uint32_t ghc;		// 0x04, Global host control
	uint32_t is;		// 0x08, Interrupt status
	uint32_t pi;		// 0x0C, Port implemented
	uint32_t vs;		// 0x10, Version
	uint32_t ccc_ctl;	// 0x14, Command completion coalescing control
	uint32_t ccc_pts;	// 0x18, Command completion coalescing ports
	uint32_t em_loc;		// 0x1C, Enclosure management location
	uint32_t em_ctl;		// 0x20, Enclosure management control
	uint32_t cap2;		// 0x24, Host capabilities extended
	uint32_t bohc;		// 0x28, BIOS/OS handoff control and status
 
	// 0x2C - 0x9F, Reserved
	uint8_t  rsv[0xA0-0x2C];
 
	// 0xA0 - 0xFF, Vendor specific registers
	uint8_t  vendor[0x100-0xA0];
 
	// 0x100 - 0x10FF, Port control registers
	HBA_PORT	ports[1];	// 1 ~ 32
} HBA_MEM;
 
typedef volatile struct tagHBA_PORT
{
	uint32_t clb;		// 0x00, command list base address, 1K-byte aligned
	uint32_t clbu;		// 0x04, command list base address upper 32 bits
	uint32_t fb;		// 0x08, FIS base address, 256-byte aligned
	uint32_t fbu;		// 0x0C, FIS base address upper 32 bits
	uint32_t is;		// 0x10, interrupt status
	uint32_t ie;		// 0x14, interrupt enable
	uint32_t cmd;		// 0x18, command and status
	uint32_t rsv0;		// 0x1C, Reserved
	uint32_t tfd;		// 0x20, task file data
	uint32_t sig;		// 0x24, signature
	uint32_t ssts;		// 0x28, SATA status (SCR0:SStatus)
	uint32_t sctl;		// 0x2C, SATA control (SCR2:SControl)
	uint32_t serr;		// 0x30, SATA error (SCR1:SError)
	uint32_t sact;		// 0x34, SATA active (SCR3:SActive)
	uint32_t ci;		// 0x38, command issue
	uint32_t sntf;		// 0x3C, SATA notification (SCR4:SNotification)
	uint32_t fbs;		// 0x40, FIS-based switch control
	uint32_t rsv1[11];	// 0x44 ~ 0x6F, Reserved
	uint32_t vendor[4];	// 0x70 ~ 0x7F, vendor specific
} HBA_PORT;


enum Kind_sata_device { AHCI_DEV_NULL   = 0,
	                    AHCI_DEV_SATA   = 1, // el sata tipico.
	                    AHCI_DEV_SEMB   = 2,
	                    AHCI_DEV_PM     = 3,
	                    AHCI_DEV_SATAPI = 4
};



/*
TODO: Detect attached sata devices.
	1 a que puerto esta atacheao.
	2 que tipo de dispositivo.
*/

// void probe_port(HBA_MEM *abar)
// {
// 	// Search disk in implemented ports
// 	uint32_t pi = abar->pi;
// 	int i = 0;
// 	while (i<32)
// 	{
// 		if (pi & 1)
// 		{
// 			int dt = check_type(&abar->ports[i]);
// 			if (dt == AHCI_DEV_SATA)
// 			{
// 				trace_ahci("SATA drive found at port %d\n", i);
// 			}
// 			else if (dt == AHCI_DEV_SATAPI)
// 			{
// 				trace_ahci("SATAPI drive found at port %d\n", i);
// 			}
// 			else if (dt == AHCI_DEV_SEMB)
// 			{
// 				trace_ahci("SEMB drive found at port %d\n", i);
// 			}
// 			else if (dt == AHCI_DEV_PM)
// 			{
// 				trace_ahci("PM drive found at port %d\n", i);
// 			}
// 			else
// 			{
// 				trace_ahci("No drive found at port %d\n", i);
// 			}
// 		}
 
// 		pi >>= 1;
// 		i ++;
// 	}
// }

class AHCI : public tres::drivers::driver_base {
   private:
	/* data */
	PCI_header_type_0x00 pci_header;
   public:
	AHCI(PCI_header_type_0x00* pci_header);
	~AHCI();
	void scan_ports();

	void init();
};

AHCI::AHCI(PCI_header_type_0x00* pci_header) {
	//this->pci_header = pci_header;
	// en bar[5] se encuentra la direccion correspondiente al registro 
	// HBA memory.
}

AHCI::~AHCI() {
}



void AHCI::scan_ports(){
	// loop 32 ports and check bit.
	for (uint8_t i = 0; i < 32; i++)
	{
		// check port
	}
	
}
void AHCI::init() {
	// FIS_REG_H2D fis;
	// memset(&fis, 0, sizeof(FIS_REG_H2D));
	// fis.fis_type = FIS_TYPE_REG_H2D;
	// fis.command = ATA_CMD_IDENTIFY;  // 0xEC
	// fis.device = 0;                  // Master device
	// fis.c = 1;                       // Write command register
}
