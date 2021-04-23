#include "types.hpp"
#include "basic_io.hpp"
#include "../drivers/console_driver.hpp"

namespace tres {
	console::console() {
		driver.init();
	}

	console::console(byte color){
		driver.init();
		driver.set_color(color);
	}

	void console::set_color(byte color){
		driver.set_color(color);
	}

	void console::print(const tres::string& str) {
		if(!driver.is_pos_inside_console(pos+str.size())){
			pos = 0;
		}
		driver.write_to_device(pos,str.data(),str.size());
		pos+=str.size();
	}

	void console::move_cursor(size_t cursor) {
		//if(!driver.is_pos_inside_console(pos)){
		//	pos = 0;
		//}else{
			pos = cursor;
		//}
	}

	size_t console::get_cursor() {
		return pos;
	}

}