//#include "kernel_print_functions.hpp"

static int dir_memoria_video = 0xB8000;
class simple_print
{
private:
	/* data */
public:
	simple_print(/* args */);
	~simple_print();
	char auxNumeric(unsigned char num);
	void printNumeric(unsigned int num);
	void print(const char* msg);
	void print(unsigned int unum);
	void operator<<(unsigned int unum){
		print(unum);
	}
	void operator<<(const char* msg){
		print(msg);
	}
};

simple_print::simple_print(/* args */)
{
}

simple_print::~simple_print()
{
}

/*Nums del 0-15*/
char simple_print::auxNumeric(unsigned char num){
  if(num>15){
    return '-';
  }
  if(num>9){
    return (num-10)+'A';
  }
  return num + '0';
  
}
/*Nums de 32 bits*/
void simple_print::printNumeric(unsigned int num) {
  char buffer[10];
  buffer[0] = '0';
  buffer[1] = 'x';

  union U {
    int num;
    unsigned char bytes[4];
  } u;
  u.num = num;

  for (int i = 0; i < 4; ++i)
  {
    buffer[i*2+2] = auxNumeric(015 & u.bytes[i]);
    buffer[i*2+3] = auxNumeric((240 & u.bytes[i]) >> 4);
  }
  
  print(buffer);
}

void simple_print::print(const char* msg){
	volatile char* mem_video = (volatile char*)dir_memoria_video;
	int index = 0;
	while(*msg != 0){
	    *mem_video++ = *msg++;
		*mem_video++ = 15;
		index++;
	}
	dir_memoria_video += index*2;
}
void simple_print::print(unsigned int unum){
	printNumeric(unum);
}


template<typename T>
void printScreen(T msg){
	simple_print a;
	a.print(msg);

}