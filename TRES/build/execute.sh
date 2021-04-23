sudo qemu-system-i386 -drive format=raw,id=disk,file=disco.img,if=none \
 -device ahci,id=ahci \
 -device ide-hd,drive=disk,bus=ahci.0 
