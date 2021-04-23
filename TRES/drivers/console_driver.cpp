#include "memory.h"
#include "types.h"

namespace tres::drivers {

	static class console_driver : public driver_base {

	public:

		void set_color(byte color) {
			color = color;
		}

		//For this driver, address is an offset. We already start at VIDEO_MEM
		//The buf array is a char array.
		void write_to_device(void* address, byte* buf, size_t n){
			size_t offset = (size_t)address;
			volatile char* mem_video = (volatile byte*)(VIDEO_MEM + address);

			for (size_t i = 0; i < n; ++i) {
				mem_video[i*2] = buf[i];
				mem_video[i*2+1] = color;
			}

		}

		void clean_console() {
			for (size_t i = 0; i < VIDEO_SIZE; ++i) {
				VIDEO_MEM[i] = 0;
			}
		}

		bool is_pos_inside_console(size_t final_pos){
			final_pos = final_pos * 2;//por el color
			return final_pos<=VIDEO_SIZE;
		}

		void init() {
			return;
		}

		void finish() {
			return;
		}

		//Not implemented here :-<
		void read_from_device(void* address, byte* buf, size_t n){
			return;
		}


	private:
		byte color = 15;
	}
}