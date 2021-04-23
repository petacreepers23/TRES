nasm 13-ModoProtegidoysaltoaCdeVerdad.asm -f bin -o boot.bin
nasm 12-second_stage.asm -f bin -o SNDSTG.BIN
#nasm 14-kernelPrueba.asm -f bin -o KERNEL.BIN
./create_d.sh disco.img   
./montaYdesmonta.sh 