void print(char *,unsigned int);
void stop();
unsigned int get_PCI_register(int bus, int slot, int func, int reg);
void set_PCI_register(int bus, int slot, int func, int reg, unsigned int value);
void search_device_class_in_bus(int bus, int classID);
void printNumeric(unsigned int num);