nasm "./0-BootStc.asm" -f bin -o boot.bin
nasm "./1-SndStg.asm" -f bin -o SNDSTG.BIN

cp boot.bin ../build/
cp SNDSTG.BIN ../build/

