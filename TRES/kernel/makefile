CC := i686-elf-g++
LN := i686-elf-g++
CCFLAGS := -O3 -std=c++17 -Wall -ffreestanding  -c  -fno-exceptions -fno-rtti
LNFLAGS := -T linkerscript.ld -ffreestanding -nostdlib -lgcc 

KERNEL_NAME = KERNEL
DEST_DIR = ./build/
DRIVER_PATH = ../drivers/
LIB_PATH = ../lib/
SOURCE_MAIN = kernel_main
SOURCE_FUNC = kernel_functions
SOURCE_DRIVER = pci
SOURCE_ALLOCATOR = basic_allocator


OUT_1 = $(DEST_DIR)$(SOURCE_MAIN)
OUT_2 = $(DEST_DIR)$(SOURCE_FUNC)
OUT_3 = $(DEST_DIR)$(SOURCE_DRIVER)
OUT_4 = $(DEST_DIR)$(SOURCE_ALLOCATOR)


clean:
	rm *.o
	rm *.bin

compile:
	$(CC) $(CCFLAGS) $(SOURCE_MAIN).cpp -o $(OUT_1).o
	$(CC) $(CCFLAGS) $(SOURCE_FUNC).cpp -o $(OUT_2).o
	$(CC) $(CCFLAGS) $(DRIVER_PATH)$(SOURCE_DRIVER).cpp -o $(OUT_3).o
	$(CC) $(CCFLAGS) $(LIB_PATH)$(SOURCE_ALLOCATOR).cpp -o $(OUT_4).o
	$(LN) $(LNFLAGS) -o $(DEST_DIR)$(KERNEL_NAME).BIN $(OUT_1).o $(OUT_2).o $(OUT_3).o $(OUT_4).o

all: clean compile

.DEFAULT_GOAL := all
