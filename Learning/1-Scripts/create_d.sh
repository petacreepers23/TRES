dd bs=512 count=5760 if=/dev/zero of=$1    
dd if=boot.bin of=$1 conv=notrunc
