
#include "print.h"

void print(){
	char mensaje[10] = {'H',15,'O',15,'L',15,'A',15,'!',15};
	volatile char* mem_video = (volatile char*)0xB8000;

	for (int i = 0; i < 10; ++i)
	{
		mem_video[i] = mensaje[i];
	}
	while(1);
}