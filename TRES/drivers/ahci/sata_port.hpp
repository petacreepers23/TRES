#include "hba_headers.hpp"
#include "fis_headers.hpp"

enum ATTACHED_DEVICE_TYPE {
	NULL = 0,
	SATA = 1,  // el sata tipico.
	SEMB = 2,
	PORT_MULTIPLIER = 3,
	SATAPI = 4
};

/*
Esta clase representa al dispositivo que  se encuentra unido al puerto de la hba. El dispositivo utiliza la interfaz de la ahci
para comunicarse con el Sistema opertativo.
*/
	class SATA_Port{
		public:
			HBA_Port* hba_port;
			ATTACHED_DEVICE_TYPE portType;
			uint8_t* buffer;
			uint8_t portNumber;
			int drive;
			void configure();
			void start_command();
			void stop_command();
			int find_empty_cmd_slot();
			//bool read(uint32_t startl, uint32_t starth, uint32_t sector_count, void *buf);
			int build_and_send_command(uint32_t sectorl, uint32_t sectorh, uint32_t sector_count, void *buf, uint8_t write, uint16_t cmd_fis_command);
	};