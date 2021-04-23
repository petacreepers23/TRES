void write_string(char colour, const char* string) {
	volatile char* mem_video = (volatile char*)0xB8000;

	while (*string != 0) {
		*mem_video++ = *string++;
		*mem_video++ = colour;
	}
}

int main() {
	write_string(15, "hola hola");
	while(1);
}
