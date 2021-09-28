#!/bin/bash 

if [ -d "build" ]; then rm -Rf "build"; fi
mkdir build
cd build
cmake ..
make 
cp boot/boot.bin ../tools 
cp boot/SNDSTG.BIN ../tools
cp kernel/KERNEL.BIN ../tools
cd ../tools
./setup.sh
cd ..
cp tools/disco.img .
echo "FIN"