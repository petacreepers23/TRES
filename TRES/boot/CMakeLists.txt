

set_source_files_properties(0-BootStc.asm 1-SndStg.asm PROTERTIES LANGUAGE ASM_NASM)
add_executable(boot.bin 0-BootStc.asm)
add_executable(SNDSTG.BIN 1-SndStg.asm)