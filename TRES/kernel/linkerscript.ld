OUTPUT_FORMAT(binary)
OUTPUT_ARCH (i386)

ENTRY(main)

MEMORY {
	RAM (wx) : org = 0x10000, len = 1000M
}

SECTIONS {
	/* Para ld, la dirección virtual y la dirección física por defecto
	 * es la misma, lo que nos simplifica el trabajo. */
	.text.start (0x10000)  : ALIGN(4K){
		CMakeFiles/kernel.dir/kernel_main.cpp.obj (.text)
	} > RAM
	.rodata (0x12000) : ALIGN(4K){
		*(.rodata)
	} > RAM
	.note.gnu.property (0x13000) : {
		*(.note.gnu.property)
	} > RAM

	
	
}
