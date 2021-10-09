#pragma once
#include <stdint.h>

/**
 * FIS(Frame Information Structure) 
 * FIS es un conjunto de estructuras de datos, utilizadas por SATA para
 * transportar datos entre el dispositivo sata y el host.
 * HOST  <-----> SATA
 *         FIS
 * Refencias utiles
 * https://wiki.osdev.org/AHCI
 * 
 * Please do not confuse the FIS type with ate ATA Command Type.
 * 
*/

/*
REFERENCE:
	Siguiendo la SATA Revision 3.0 , 10 Transport Layer

usefull links but maybe broken :
http://www.usedsite.co.kr/pds/file/SerialATA_Revision_3_0_RC11.pdf
*/
enum FIS_TYPE
{
	FIS_TYPE_REG_H2D	= 0x27,	// Register FIS - host to device
	FIS_TYPE_REG_D2H	= 0x34,	// Register FIS - device to host
	FIS_TYPE_DMA_ACT	= 0x39,	// DMA activate FIS - device to host
	FIS_TYPE_DMA_SETUP	= 0x41,	// DMA setup FIS - bidirectional
	FIS_TYPE_DATA		= 0x46,	// Data FIS - bidirectional
	FIS_TYPE_BIST		= 0x58,	// BIST activate FIS - bidirectional
	FIS_TYPE_PIO_SETUP	= 0x5F,	// PIO setup FIS - device to host
	FIS_TYPE_DEV_BITS	= 0xA1,	// Set device bits FIS - device to host
	FIS_TYPE_VENDOR_SPECIFIC1 = 0xC7,
	FIS_TYPE_VENDOR_SPECIFIC2 = 0xD4,
} ; // SATA Revision 3.0, 10.3.1 FIS Type values.



/*Register Host to Device FIS.
transfer the content of the shadow register block (host to device)

*/
struct FIS_Register_host_to_device {
	// DWORD 0
	uint8_t fis_type;  // FIS_TYPE_REG_H2D

	uint8_t pmport : 4;  // Port multiplier
	uint8_t rsv0 : 3;    // Reserved
	uint8_t c : 1;       // 1: Command, 0: Control
    /* c bit.
	1 -> update of the command register.
	0 -> update of the device control register.
	Undefined-> to put to 1 and SRST bit to 1. 
	*/

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
};// 20 Bytes

// DEVICE TO HOST sata -> hba,leer.
struct FIS_Register_device_to_host {
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
} ;

// Bidirectional for data payload.
struct FIS_data {
	// DWORD 0
	uint8_t fis_type;  // FIS_TYPE_DATA

	uint8_t pmport : 4;  // Port multiplier
	uint8_t rsv0 : 4;    // Reserved

	uint8_t rsv1[2];  // Reserved

	// DWORD 1 ~ N
	uint32_t data[1];  // Payload
} ;// 4-8196 bytes

struct FIS_PIO_setup {
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
} ;

struct FIS_DMA_setup {
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

};