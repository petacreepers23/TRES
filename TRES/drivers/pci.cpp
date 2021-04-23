// #include <stdint.h>
// #include <stdio.h>
// #include <stdlib.h>
// #include <sys/io.h>
#include "pci.hpp"

#include "../kernel/kernel_functions.hpp"

#define CONFIG_DIR 0xCF8
#define CONFIG_DAT 0xCFC

template<typename T>
void ins(short port, T *dest, unsigned int count) {
    asm volatile("rep ins%z2"
        : "+D" (dest), "+c" (count), "=m" (*dest)
        : "d" (port)
        : "memory");
}

// extern "C" int inl();
// asm(R"(
// .globl inl
//     .type inl, @function
//     inl:
//     //.cfi_startproc
// 	mov $0xCFC,%dx
//     in %dx
//     ret
//     //.cfi_endproc
// )");

template <typename T>
void outl(unsigned int dir, short port) {
	asm volatile("outl %0,%w1"
	             :
	             : "a"(dir), "Nd"(port));
}
// __asm__ __volatile__ ("outl %0,%w1": :"a" (dir), "Nd" (CONFIG_DIR));

int ls_pci() {
	unsigned int b, d, f;
	unsigned int dir, dat, reg, clase, subclass;
	unsigned int jj = 8;

	// if (ioperm(CONFIG_DIR, 8, 1) < 0) {
	//             perror("ioperm");
	//             return 1;
	//     }

	reg = 0x8;  // registro que contiene clase y subclase
	for (b = 0; b < 256; b++) {
		for (d = 0; d < 32; d++) {
			for (f = 0; f < 7; f++) {
				dir = (unsigned int)((b << 16) | (d << 11) |
				                     (f << 8) | (reg & 0xfc) | ((unsigned int)0x80000000));

				__asm__ __volatile__("outl %0,%w1"
				                     :
				                     : "a"(dir), "Nd"(CONFIG_DIR));
				//outl(dir, (short)CONFIG_DIR);

				//unsigned int res;
				ins(CONFIG_DAT, &dat, 2);

				//dat = inl(CONFIG_DAT);
				// no hay dispositivo en este "slot"
				if (dat == 0xFFFFFFFF)
					continue;

				// extrae la clase
				clase = dat >> 24;
				// extrae la subclase
				subclass = (dat >> 16) & 0xff;
				unsigned int algo = 0x08000000;
				// if (clase == algo)
				// 	continue;

				//printf("Bus %d Dev %d Func %d: Clase %x Subclase %x\n",b, d, f, clase, subclass);
				print((char*)"Bus", 3);
				printNumeric(b);

				print((char*)"Dev", 3);
				printNumeric(d);

				print((char*)"Fun", 3);
				printNumeric(f);

				print((char*)"Cla", 3);
				printNumeric(clase);

				print((char*)"Sub", 3);
				printNumeric(subclass);
				print((char*)"--------------", 4);
			}
		}
	}
	return 0;
}
