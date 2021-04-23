CC := g++
LN := ld
CCFLAGS := -std=c++17 -Wall -nostdlib -fno-exceptions -fno-rtti -c -m32 -fno-pie
LNFLAGS := -T linkerscript.ld

SOURCE = kernel1
DEST = kernel1
SOURCE_PRINT = kernel2
DEST_PRINT = kernel2

clean:
	rm *.o
	rm *.bin

compile:
	$(CC) $(CCFLAGS) $(SOURCE).cpp
	$(CC) $(CCFLAGS) $(SOURCE_PRINT).cpp
	$(LN) $(LNFLAGS) $(DEST).o $(DEST_PRINT).o -o $(DEST).bin

all: clean compile

.DEFAULT_GOAL := all
