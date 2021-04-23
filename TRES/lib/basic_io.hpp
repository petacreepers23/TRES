#include "types.hpp"

namespace tres {

	template<class Allocator = tres::basic_allocator> class console {
	public:
		console();
		console(byte color);
		void set_color(byte color);
		void print(tres::string str);
		//void print_line(tres::string str);
		void move_cursor(size_t cursor);
		size_t get_cursor();
	private:
		drivers::console_driver driver;
		size_t pos;
		byte color = 15;//blanco x defecto
	};

	template<class Allocator = tres::basic_allocator> class keyboard {
		//void register_callback(lambda l);//on timer or just read, it gets called
		//void set_read_interval(size_t interval);//0 to disable auto
		//void just_read();//calls the callback
		char read_char();//reads if a key is pressed
		constexpr char NO_KEY = 0;
	};

	template<class Allocator = tres::basic_allocator> class speaker {

	};

	template<class Allocator = tres::basic_allocator> class disk {

	};

	template<class Allocator = tres::basic_allocator> class screen {

	};

	enum DeviceType {
		D_HDD,D_BLA
	};
	template<class Allocator = tres::basic_allocator> class device {
	public:
		//Constructor
		device(DeviceType type);
		//Destructor
		~basic_vector();
		divers::basic_driver get_driver();
		void write(tres::string s);
		tres::string read(s);
	};
}