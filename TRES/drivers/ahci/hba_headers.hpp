#pragma once
/*
Un HBA, o Host Bus Adapter es un dispositivo que implementa
la especificación AHCI. Que nos va a servir para podernos comunicar 
entre la memoria del sistema y el dispositivo Serial ATA.

En este documento se cren las esctructuras correspondientes
a los HBA Memory Registers:

Y si no quieres tener problemas más te vale evitar que alguien pueda acceder 
por accidente. O tendrás un comportamiento indeterminado.

Los registros se dividen en dos partes.
Los registros globales y  los de control de puertos. De esos hay 32, por cierto,
uno por cada puerto.

Generic Host Control
otras
Port Control register
*/
#include <stdint.h>



/*
Port Controller Regiter tenemos 32 registros de este tipo.
uno por cada puerto.

*/
struct HBA_Port
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
};

/* Esta es la estructura principal, qe contiene a los registros 
de puerto. 
*/
struct HBA_Memory
{
	// 0x00 - 0x2B, Generic Host Control
	uint32_t cap;		// 0x00, Host capability
	uint32_t ghc;		// 0x04, Global host control
	uint32_t is;		// 0x08, Interrupt status
	uint32_t pi;		// 0x0C, Port implemented
	uint32_t vs;		// 0x10, Version
	uint32_t ccc_ctl;	// 0x14, Command completion coalescing control
	uint32_t ccc_pts;	// 0x18, Command completion coalescing ports
	uint32_t em_loc;	// 0x1C, Enclosure management location
	uint32_t em_ctl;	// 0x20, Enclosure management control
	uint32_t cap2;		// 0x24, Host capabilities extended
	uint32_t bohc;		// 0x28, BIOS/OS handoff control and status
 
	// 0x2C - 0x9F, Reserved
	uint8_t  rsv [0x74];//[0xA0-0x2C];
 
	// 0xA0 - 0xFF, Vendor specific registers
	uint8_t  vendo[0x60];//[0x100-0xA0];
  
	// 0x100 - 0x10FF, Port control registers
	HBA_Port port[32];	// 1 ~ 32
};



struct HBA_Command_Header
{
	// DW0
	uint8_t  command_FIS_length:5;		// Command FIS length in DWORDS, 2 ~ 16
	uint8_t  atapi:1;		// ATAPI
	uint8_t  write:1;		// Write, 1: H2D, 0: D2H
	uint8_t  prefetchable:1;		// Prefetchable
 
	uint8_t  reset:1;		// Reset
	uint8_t  bist:1;		// BIST
	uint8_t  clear_busy_upon_R_OK:1;		// Clear busy upon R_OK
	uint8_t  reserved0:1;		// Reserved
	uint8_t  port_multiplier_port:4;		// Port multiplier port
 
	uint16_t physical_region_descriptor_table_length;		// Physical region descriptor table length in entries
 
	// DW1
	volatile
	uint32_t physical_region_descriptor_byte_count;		// Physical region descriptor byte count transferred
 
	// DW2, 3
	uint32_t command_table_base_address;		// Command table descriptor base address
	uint32_t command_table_base_address_upper;		// Command table descriptor base address upper 32 bits
 
	// DW4 - 7
	uint32_t reserved1[4];	// Reserved
};

 
struct HBA_PRDT_Entry
{
	uint32_t data_base_address;		// Data base address
	uint32_t data_base_address_upper;		// Data base address upper 32 bits
	uint32_t reserved0;		// Reserved
 
	// DW3
	uint32_t data_byte_count:22;		// Byte count, 4M max 2bytes min.
	uint32_t reserved1:9;		// Reserved
	uint32_t i:1;		// Interrupt on completion
} ;

struct HBA_Command_Table{
	// 0x00
	uint8_t  command_FIS[64];	// Command FIS
 
	// 0x40
	uint8_t  atapi_command[16];	// ATAPI command, 12 or 16 bytes
 
	// 0x50
	uint8_t  reserved[48];	// Reserved
 
	// 0x80
	HBA_PRDT_Entry	prdt_entry[];	// Physical region descriptor table entries, 0 ~ 65535
};


