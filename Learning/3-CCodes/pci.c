#include <sys/io.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define CONFIG_DIR 0xCF8
#define CONFIG_DAT 0xCFC

int main() {
	int b, d, f;
	uint32_t dir, dat, reg, class, subclass;

	if (ioperm(CONFIG_DIR, 8, 1) < 0) {
                perror("ioperm");
                return 1;
        }
	reg = 0x8; // registro que contiene clase y subclase
	for (b=0; b<256; b++)
	    for (d=0; d<32; d++)
		for (f=0; f<7; f++) {
			dir = (uint32_t)((b << 16) | (d << 11) |
              			(f << 8) | (reg & 0xfc) | ((uint32_t)0x80000000));
        		outl (dir, CONFIG_DIR);
        		dat = inl(CONFIG_DAT);
			// no hay dispositivo en este "slot"
			if (dat == 0xFFFFFFFF)
				continue;

			// extrae la clase
			class = dat>>24;
			// extrae la subclase
			subclass = (dat>>16) & 0xff;

        		printf("Bus %d Dev %d Func %d: Clase %x Subclase %x\n",
				       	b, d, f, class, subclass);
		}
	return 0;
}
