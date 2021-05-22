
#include "kernel_functions.hpp"
const uint32_t dir_memoria_video = 0xB8000;
const uint32_t dir_fin_memoria_video = 0xC0000;

const uint32_t x_size = 80;
const uint32_t y_size = 25;
static uint32_t x_current = 0;
static uint32_t y_current = 0;
static int index = 0;  // TODO: fix please, this is a ñapa.

int color = 15;
/* 
2 bytes per item.
------------------------------
first byte for character
second byte for color:
	8-11  bit -> forecolor
	15-12 bit -> backcolor
------------------------------


0 	BLACK 	    8 	DARK GREY
1 	BLUE 	    9 	LIGHT BLUE
2 	GREEN 	    10 	LIGHT GREEN
3 	CYAN 	    11 	LIGHT CYAN
4 	RED 	    12 	LIGHT RED
5 	MAGENTA 	13 	LIGHT MAGENTA
6 	BROWN 	    14 	LIGHT BROWN
7 	LIGHT GREY 	15 	WHITE           
*/
enum CONSOLE_COLORS : unsigned char { BLACK = 0,
	                                  BLUE = 1,
	                                  GREEN = 2,
	                                  CYAN = 3,
	                                  RED = 4,
	                                  MAGENTA = 5,
	                                  BROWN = 6,
	                                  LIGHT_GREY = 7,
	                                  DARK_GREY = 8,
	                                  LIGHT_BLUE = 9,
	                                  LIGHT_GREEN = 10,
	                                  LIGHT_CYAN = 11,
	                                  LIGHT_RED = 12,
	                                  LIGHT_MAGENTA = 13,
	                                  LIGHT_BROWN = 14,
	                                  WHITE = 15
};
//

// given actual x and y position you get the correspondiente memory address
inline uint32_t pos(uint32_t x_current, uint32_t y_current) {
	return ((y_current * x_size) + x_current) << 1;
}
void set_color(CONSOLE_COLORS forecolour, CONSOLE_COLORS backcolour) {
	color = (backcolour << 4) | (forecolour & 0x0F);
}
void print_char(char c, CONSOLE_COLORS forecolour, CONSOLE_COLORS backcolour) {
	volatile char* mem_video = (volatile char*)(dir_memoria_video + pos(x_current, y_current));
	*mem_video++ = c;
	*mem_video++ = (backcolour << 4) | (forecolour & 0x0F);
	x_current++;
	if (x_current >= x_size) {  // check if you end the line.
		x_current = 0;
		y_current++;
	}
	if (y_current == y_size) {
		// scroll operation.
		scroll();
	}
}

void console_clear() {
	volatile uint16_t* mem_video = (volatile uint16_t*)(dir_memoria_video);
	for (uint32_t i = dir_memoria_video; i < dir_fin_memoria_video; i++) {
		*mem_video++ = 0;
	}
	x_current = 0;
	y_current = 0;
}


// only one process shoul use this function at the same time. or surprises.
void simple_print(const char* msg) {
	volatile char* mem_video = (volatile char*)(dir_memoria_video + pos(x_current, y_current));
	while (*msg != 0) {
		if (*msg == '\n') {
			y_current++;
			x_current = 0;
			msg++;
		} else {
			mem_video = (volatile char*)(dir_memoria_video + pos(x_current, y_current));
			*mem_video++ = *msg++;
			*mem_video++ = color;
			x_current++;
		}
		if (x_current == x_size) {  // check if you end the line.
			x_current = 0;
			y_current++;
		}
		if (y_current == y_size) { // si se acaba la terminal, pues hacemos scroll
			scroll();
		}
	}
}

void scroll() {
	volatile uint16_t* mem_video = (volatile uint16_t*)(dir_memoria_video);
	for (uint32_t i = 0; i < ((y_size-1) * x_size); i++)
	{
		 mem_video[i] = mem_video[i+x_size];
	}
	// clear last line
	for (uint32_t i = x_size*(y_size-1); i < (y_size * x_size); i++)
	{
		 mem_video[i] = 0x20;
	}
	y_current = (y_size-1);


}

// debería funcionar con los uints perfectamente
void simple_print(int num, int base) {
	return simple_print(itoa(num, base));
	// static char buf[32] = {0};
	// int i = 30;
	// int sign;

	// if ((sign = num) < 0) {
	// 	num = -num;
	// }

	// do {
	// 	buf[i] = "0123456789abcdef"[num % base];
	// 	i--;
	// 	num /= base;
	// } while (num != 0 && i != 0);
	// if (sign < 0) {  // Si num negativo metemos el -
	// 	buf[i] = '-';
	// 	i--;
	// }
	// simple_print(&buf[i + 1]);
}
char* itoa(int num, int base) {
	static char buf[32] = {0};
	int i = 30;
	int sign;

	if ((sign = num) < 0) {
		num = -num;
	}

	do {
		buf[i] = "0123456789abcdef"[num % base];
		i--;
		num /= base;
	} while (num != 0 && i != 0);
	if (sign < 0) {  // Si num negativo metemos el -
		buf[i] = '-';
		i--;
	}
	return &buf[i + 1];
}

// deprecated
// void print(const char* msg, unsigned int size) {
// 	volatile char* mem_video = (volatile char*)dir_memoria_video;

// 	for (unsigned int i = 0; i < size; ++i) {
// 		mem_video[i * 2] = msg[i];
// 		mem_video[i * 2 + 1] = 15;
// 	}
// 	mem_video[size * 2] = ' ';
// 	mem_video[size * 2 + 1] = (char)15;
// 	dir_memoria_video += size * 2 + 2;
// }

// // deprecated
// /*Nums del 0-15*/
// char auxNumeric(unsigned char num) {
// 	if (num > 15) {
// 		return '-';
// 	}
// 	if (num > 9) {
// 		return (num - 10) + 'A';
// 	} else {
// 		return num + '0';
// 	}
// }
// // deprecated
// /*Nums de 32 bits*/
// void printNumeric(unsigned int num) {
// 	char buffer[10];
// 	buffer[0] = '0';
// 	buffer[1] = 'x';

// 	union U {
// 		int num;
// 		unsigned char bytes[4];
// 	} u;
// 	u.num = num;

// 	for (int i = 0; i < 4; ++i) {
// 		buffer[i * 2 + 2] = auxNumeric(015 & u.bytes[i]);
// 		buffer[i * 2 + 3] = auxNumeric((240 & u.bytes[i]) >> 4);
// 	}

// 	print(buffer, 10);
// }

void stop() {
	while (1)
		;
}

// TODO:
void set_cursor_pos(unsigned short position) {
}
