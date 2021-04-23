#include "../lib/types.hpp"

namespace tres::drivers {

	//Abstract class that has the minimum methods for a driver fucntion
	class driver_base {

	public:
		//Inits the driver
		virtual void init();
		//Clears the driver
		virtual void finish();
		//writes buf xd
		virtual void write_to_device(void* address, byte* buf, size_t n);
		//returns in buf
		virtual void read_from_device(void* address, byte* buf, size_t n);

	};

}