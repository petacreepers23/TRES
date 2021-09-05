#pragma once
#include "types.hpp"
#include <stdint.h>
namespace tres {

	const volatile uint32_t*  STACK_MEM   = (const uint32_t*)(0x09FF0);
	const volatile size_t STACK_SIZE  = 0x09FF0;

	const volatile uint32_t*  CODE_MEM    = (const uint32_t*)(0x10000);
	const volatile size_t CODE_SIZE   = 0x40000;

	const volatile uint32_t*  KERNEL_MEM  = (const uint32_t*)(0x50000);
	const volatile size_t KERNEL_SIZE = 0x10000;

	const volatile uint32_t*  HEAP_MEM    = (const uint32_t*)(0x60000);
	const volatile size_t HEAP_SIZE   = 0x68000;

	const volatile uint32_t*  VIDEO_MEM   = (const uint32_t*)(0xB8000);
	const volatile size_t VIDEO_SIZE  = 0x4B000;

};