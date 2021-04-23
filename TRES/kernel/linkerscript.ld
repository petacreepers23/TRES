OUTPUT_FORMAT(binary)
OUTPUT_ARCH (i386)

ENTRY(main)

MEMORY {
	RAM (wx) : org = 0x10000, len = 1000M
}

SECTIONS {
	/* Para ld, la dirección virtual y la dirección física por defecto
	 * es la misma, lo que nos simplifica el trabajo. */
	.text.start (0x10000) : {
		./build/kernel_main.o (.text)
	} > RAM
	.rodata (0x10100) : {
		./build/kernel_main.o (.rodata)
	} > RAM
	.note.gnu.property (0x10150) : {
		./build/kernel_main.o (.note.gnu.property)
	} > RAM
	.functs (0x10200) : {
		./build/kernel_functions.o (.text)
	} > RAM
	
	
}
