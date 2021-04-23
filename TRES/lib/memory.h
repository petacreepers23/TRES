#include "types.h"

namespace tres {

	const volatile byte*  STACK_MEM   = 0x09FF0;
	const volatile size_t STACK_SIZE  = 0x09FF0;

	const volatile byte*  CODE_MEM    = 0x10000;
	const volatile size_t CODE_SIZE   = 0x40000;

	const volatile byte*  KERNEL_MEM  = 0x50000;
	const volatile size_t KERNEL_SIZE = 0x10000;

	const volatile byte*  HEAP_MEM    = 0x60000;
	const volatile size_t HEAP_MEM    = 0x68000;

	const volatile byte*  VIDEO_MEM   = 0xB8000;
	const volatile size_t VIDEO_SIZE  = 0x4B000;

};