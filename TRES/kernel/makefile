CC := i686-elf-g++
LN := i686-elf-g++
CCFLAGS := -O3 -std=c++17 -Wall -ffreestanding  -c  -fno-exceptions -fno-rtti
LNFLAGS := -T linkerscript.ld -ffreestanding -nostdlib -lgcc 

KERNEL_NAME := KERNEL
DEST_DIR := ./build

LIB_PATH := ../lib
SOURCE_MAIN := kernel_main
SOURCE_FUNC := kernel_functions

SOURCE_ALLOCATOR = basic_allocator

# drivers
DRIVER_PATH := ../drivers
PCI_DRIVER := pci
DRIVER_BASE := driver_base

# ahci
AHCI_PATH := $(DRIVER_PATH)/ahci
DRIVER_AHCI := ahci
DRIVER_AHCI_PORT := port


OUT_1 = $(DEST_DIR)/$(SOURCE_MAIN)
OUT_2 = $(DEST_DIR)/$(SOURCE_FUNC)
OUT_3 = $(DEST_DIR)/$(PCI_DRIVER)
#OUT_4 := $(DEST_DIR)/$(DRIVER_BASE)
OUT_5 = $(DEST_DIR)/$(SOURCE_ALLOCATOR)
OUT_6 = $(DEST_DIR)/$(DRIVER_AHCI)
OUT_7 = $(DEST_DIR)/$(DRIVER_AHCI_PORT)

# $(CC) $(CCFLAGS) $(DRIVER_PATH)/$(DRIVER_BASE).cpp -o $(OUT_4).o
clean:
	rm *.o
	rm *.bin

compile:
	$(CC) $(CCFLAGS) $(SOURCE_MAIN).cpp -o $(OUT_1).o
	$(CC) $(CCFLAGS) $(SOURCE_FUNC).cpp -o $(OUT_2).o
	$(CC) $(CCFLAGS) $(DRIVER_PATH)/$(PCI_DRIVER).cpp -o $(OUT_3).o
	
	$(CC) $(CCFLAGS) $(AHCI_PATH)/$(DRIVER_AHCI).cpp -o $(OUT_6).o
	$(CC) $(CCFLAGS) $(AHCI_PATH)/$(DRIVER_AHCI_PORT).cpp -o $(OUT_7).o
	$(CC) $(CCFLAGS) $(LIB_PATH)/$(SOURCE_ALLOCATOR).cpp -o $(OUT_5).o

	$(LN) $(LNFLAGS) -o $(DEST_DIR)/$(KERNEL_NAME).BIN $(OUT_1).o $(OUT_2).o $(OUT_3).o $(OUT_5).o $(OUT_6).o $(OUT_7).o

all: clean compile

.DEFAULT_GOAL := all
