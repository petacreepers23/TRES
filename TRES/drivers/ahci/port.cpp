#include <stdint.h>


#include "ahci.hpp"
#include "port.hpp"
#include "constants.hpp"


namespace tres {

void* memset(void* ptr, int value, size_t num) {
    unsigned char* p = (unsigned char*)ptr;
    for (size_t i = 0; i < num; i++) {
        p[i] = (unsigned char)value;
    }
    return ptr;
}

void * memcpy ( void * destination, const void * source, size_t num ){
    unsigned char* p = (unsigned char*)source;
    unsigned char* d = (unsigned char*)destination;
    for(size_t i = 0; i < num; i++) {
        d[i] = p[i];
    }
    return destination;
    
}
/*
 Find a empty command list slot. A slot = command header -> hay 32.
 Para encontrar un command slot vacio  basta con recorrer los reg PxCI y PxSACT y en sus respectivos bits, si ambos estan
 a 0 entonces es un command slot libre. 
 @return The empty slot o si no hay -1

*/
int Port::find_empty_cmd_slot()
{
    // If not set in SACT and CI, the slot is empty
    uint32_t slots = (hba_port->sact | hba_port->ci);
    for (int i=0; i<32; i++) // loop of a 32 bit register.
    {
        if ((slots&1) == 0)
            return i;
        slots >>= 1; // slots = slots/2
    }
    simple_print("Cannot find free command list entry\n");
    return -1;
}






int Port::build_and_send_command(uint32_t sectorl, uint32_t sectorh, uint32_t sector_count, void *buf, uint8_t write, uint16_t cmd_fis_command){
    /*Los num se corresponden a los pasos del punto 5.5.1 de la especificacion ahci 1.3.1 pag 70 */
    
    //0 find an empty command slot.
    int empty_slot;
    if ((empty_slot = find_empty_cmd_slot()) == -1) {
        return -1;
    }
    // 2 if is an atapi command-> Esto no ha sido contemplado..

    // 3 build a command header
    HBA_Command_Header* command_header = (HBA_Command_Header*) hba_port->clb + empty_slot;// ls command headers son un 
    command_header->command_FIS_length = sizeof(FIS_Register_host_to_device)/sizeof(uint32_t);
    command_header->write = 0; // solo se enciende(1) si la data se va a escribir en el dispositivo(el disco sata)
    command_header->clear_busy_upon_R_OK = 1;
    //command_header->bist=
    command_header->prefetchable=1;
    command_header->atapi=0; // TODO: See when necessary for atapi commands, prob an extra parameter or a build_atapi_command.
    command_header->physical_region_descriptor_table_length = (uint16_t)((sector_count-1)>>4) + 1;// numero de entradas PRDT en la command table. 16 sectore por PRDT
    HBA_Command_Table *command_table = (HBA_Command_Table *)(&command_header->command_table_base_address);
    
    /* IN ATA a sector use to be 512 bytes, but to be sure you should send a 
    identify device command*/


    //assert physical_region_descriptor_table_length <=8 o will explote because the number of prdt reserved are 8.
    uint32_t size_sector = 512; 
    command_table->prdt_entry[0].data_base_address = (uint32_t) buf;
    command_table->prdt_entry[0].data_byte_count = size_sector* sector_count-1;
    command_table->prdt_entry[0].i=1;
    
    
    //1 Build a command FIS
    FIS_Register_host_to_device *cmd_fis = (FIS_Register_host_to_device*)(&command_table->command_FIS);

    cmd_fis->fis_type = FIS_TYPE_REG_H2D;
    cmd_fis->c = 1;	// Command
    cmd_fis->command = cmd_fis_command;

    cmd_fis->lba0 = (uint8_t)sectorl;
    cmd_fis->lba1 = (uint8_t)(sectorl>>8);
    cmd_fis->lba2 = (uint8_t)(sectorl>>16);
    cmd_fis->device = 1<<6;	// LBA mode

    cmd_fis->lba3 = (uint8_t)(sectorl>>24);
    cmd_fis->lba4 = (uint8_t)sectorh;
    cmd_fis->lba5 = (uint8_t)(sectorh>>8);

    cmd_fis->countl = sector_count & 0xFF;
    cmd_fis->counth = (sector_count >> 8) & 0xFF;

    // 4 if is a queued command -> Caso no contemplado. 
    // 5  Enviar el comando
    hba_port->ci = 1 << empty_slot; 
    /* turn on the empty_slot bit in the HBA Port Command Issue to indicate to
        the hba that a command has been built in syttem memory for a command
    slot and may be sent to the devide*/

    /*Esperar para saber si tenemos un fallo o todo guay.*/

    // Wait for completion
    #define HBA_PxIS_TFES (1 << 30)
    while (true)
    {
        // In some longer duration reads, it may be helpful to spin on the DPS bit 
        // in the PxIS port field as well (1 << 5)
        if ((hba_port->ci & (1<<empty_slot)) == 0) 
            break;
        if (hba_port->is & HBA_PxIS_TFES)	// Task file error
        {
            simple_print("Read disk error\n");
            return false;
        }
    }

    // Check again
    if (hba_port->is & HBA_PxIS_TFES)
    {
        simple_print("Read disk error\n");
        return -1;
    }


    return 1;


}


// rename to start_command_engine
void Port::start_command(){
    /*Usamos dma, por tanto esperamos hasta que el bit 15 de command and status 
    esto es, el command list running(CR) este a 0. Lo que
    significa que el dma engine del puerto no esta corriendo.
    
    
    PxCMD.CR = Port x Command and Status. Command List Running(bit 15)
    Cuando el bit 15,CR  esta a 1 el DMA engine esta corriendo. por tanto no podemos usarlo.
    hay que esperar a que este a cero.
    */
    const uint32_t port_command_and_status_command_list_running = (1 << 15);
    while(hba_port->cmd & port_command_and_status_command_list_running);
    hba_port->cmd |= (1 << 4); // set cmd.fre
    hba_port->cmd |= 0x0001;// set cmd.st

    /*clear de PxIS, 0xffff 
    set PxIE 
    */

}


    /*Asegurarse que el puerto se encuentra en un estado inactivo o idle state.*/
void Port::stop_command(){

    // Clear Start ST (bit 0)
    hba_port->cmd &= ~constants::HBA_PxCMD_ST_bit;

    // Clear FIS_Received_Enable (bit 4)
    hba_port->cmd &= ~constants::HBA_PxCMD_FRE_bit;

    // Wait until FR (bit14), CR (bit15) are cleared
    simple_print(" ");
    while(true)
    {
        if (hba_port->cmd & constants::HBA_PxCMD_FR_bit )
            continue;
        if (hba_port->cmd & constants::HBA_PxCMD_CR_bit)
            continue;
        break;
    }

}




    /*

rename to allocate memory or something

Summary:
   Allocates memory for the received FIS Data structure,
   the command list data structure and the command Table
   of every command header from the command list Data structure
   
In detail:

- Allocate memory for the Receive FIS structure, 256 bytes aligned to 256 bytes.

- Allocate memory for the Command List struture. number
  of Commands/slots(32) * 32 bytes -> 1024 bytes
 - Allocate memory for the buffers of each Command of the
  command List structure.
  Each Command Table needs: 
   CFIS Size (up to 64 Bytes)
   + ACMD Size(up to 16 Bytes)
   + 48 bytes reserved
   + num_PRDT * Size_PRDT(16 Bytes)
   = 128 + num_PRDT(8) *Size_PRDT(16 Bytes)= 256 bytes

*/
void Port::configure(){
    //simple_print("debug 0");
    stop_command();
    //simple_print("debug 1");
        //Reserva 1024 bytes para la command list structure.
        // alinear la estructura command list base addres con 1024 bytes. La dir de mem debe ser multiplo de 1024.
    hba_port->clb =  *(uint32_t *) aligned_new(1024,1024);
    hba_port->clbu = 0;
    memset((void*)hba_port->clb,0,1024);
    //simple_print("debug 2");

    // Reservamos espacio para la Receive FIS structure. alineado 256 bytes.
    hba_port->fb = *(uint32_t *) aligned_new(256,256);
    hba_port->fbu = 0;
    memset((void*)hba_port->clb,0,256);

    //por cada entrada de la command list, puede haber una comand base addres que apunte a la command table, que también tienes que reservar y alinear a 256 Bytes.[los 8 bits reservados de cla ctba]
    HBA_Command_Header *hba_command_header = (HBA_Command_Header *)hba_port->clb;
    for (int i = 0; i < 32; i++)
    {
        hba_command_header[i].physical_region_descriptor_table_length = 8;
        hba_command_header[i].command_table_base_address =  *(uint32_t *) aligned_new(256,256);
        hba_command_header[i].command_table_base_address_upper = 0;
        memset((void*)hba_command_header[i].command_table_base_address , 0, 256);
    }
    start_command();
}


}