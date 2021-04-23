#include "types.hpp"

namespace tres {

	const volatile byte*  STACK_MEM   = (const byte*)(0x09FF0);
	const volatile size_t STACK_SIZE  = 0x09FF0;

	const volatile byte*  CODE_MEM    = (const byte*)(0x10000);
	const volatile size_t CODE_SIZE   = 0x40000;

	const volatile byte*  KERNEL_MEM  = (const byte*)(0x50000);
	const volatile size_t KERNEL_SIZE = 0x10000;

	const volatile byte*  HEAP_MEM    = (const byte*)(0x60000);
	const volatile size_t HEAP_SIZE   = 0x68000;

	const volatile byte*  VIDEO_MEM   = (const byte*)(0xB8000);
	const volatile size_t VIDEO_SIZE  = 0x4B000;

};