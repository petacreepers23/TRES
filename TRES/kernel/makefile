CC := g++
LN := ld
CCFLAGS := -std=c++17 -Wall -nostdlib -fno-exceptions -fno-rtti -c -m32 -fno-pie -fno-asynchronous-unwind-tables
LNFLAGS := -T linkerscript.ld

KERNEL_NAME = KERNEL
DEST_DIR = ./build/
SOURCE_MAIN = kernel_main
SOURCE_FUNC = kernel_functions



OUT_1 = $(DEST_DIR)$(SOURCE_MAIN)
OUT_2 = $(DEST_DIR)$(SOURCE_FUNC)

clean:
	rm *.o
	rm *.bin

compile:
	$(CC) $(CCFLAGS) $(SOURCE_MAIN).cpp -o $(OUT_1).o
	$(CC) $(CCFLAGS) $(SOURCE_FUNC).cpp -o $(OUT_2).o
	$(LN) $(LNFLAGS) -o $(DEST_DIR)$(KERNEL_NAME).BIN

all: clean compile

.DEFAULT_GOAL := all
