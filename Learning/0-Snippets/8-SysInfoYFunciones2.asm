;En este codigo configuramos el primer sector
;con una fat para que lo entienda un pc normal
;y no haya que hacer un formateo a bajo nivel
;cada vez que grabamos el so.

;Este codigo esta estructurado en un salto a nuestro codigo,
;una BPB y EBPB, funciones, "nuestro codigo" y los datos.

;La bios tiene un estándar que es el siguiente:
;0000	3	Code		Jump to rest of code	 
;0003	8	BPB			OEM name										Great-OS
;0011	2	 			Bytes per sector								512
;0013	1	 			Number of sectors per cluster					1
;0014	2	 			Number of reserved sectors						1
;0016	1	 			Number of FAT tables							2
;0017	2	 			Number of root directory entries (usually 224)	224
;0019	2	 			Total number of sectors							2880
;0021	1	 			Media descriptor								0xf0
;0022	2	 			Number of sectors per FAT						9
;0024	2	 			Number of sectors/track							9
;0026	2	 			Number of heads									2
;0028	2	 			Number of hidden sectors						0
;0030	2	EBPB		Number of hidden sectors (high word)			0
;0032	4	 			Total number of sectors in filesystem	 
;0036	1	 			Logical drive number							0
;0037	1	 			Reserved	 
;0038	1	 			Extended signature								0x29
;0039	4	 			Serial number	 
;0043	11	 			Volume label									MYVOLUME
;0054	8	 			Filesystem type									FAT16
;0062	448	Code		Boot code	 
;0510	2	Required	Boot signature									0xaa55


bits 16; Modo de 16 Bits
%define LOAD_SEGMENT 0x1000 ;Sitio donde cargar el resto de mierdas, visto en  codigos (3-4-5)

main:
		jmp start
		nop 	 	;Esto nos deja un offset de 0x3 justo debajo

bootinfo: ;La tabla que hay que poner, descrita arriba

  iOEM:          db "TRES    "     ; OEM String
  iSectSize:     dw  0x200         ; bytes per sector
  iClustSize:    db  1             ; sectors per cluster
  iResSect:      dw  1             ; #of reserved sectors
  iFatCnt:       db  2             ; #of FAT copies
  iRootSize:     dw  224           ; size of root directory
  iTotalSect:    dw  2880          ; total # of sectors if over 32 MB
  iMedia:        db  0xF0          ; media Descriptor
  iFatSize:      dw  9             ; size of each FAT
  iTrackSect:    dw  9             ; sectors per track
  iHeadCnt:      dw  2             ; number of read-write heads
  iHiddenSect:   dw  0             ; number of hidden sectors
  				 dw  0             ; son de 4 bytes no 2
  iSect32:       dw  0             ; # sectors for over 32 MB
  				 dw  0
  iBootDrive:    db  0             ; holds drive that the boot sector came from
  iReserved:     db  0             ; reserved, empty
  iBootSign:     db  0x29          ; extended boot sector signature
  iVolID:        db "seri"         ; disk serial
  acVolumeLabel: db "MYVOLUME    "  ; volume label
  acFSType:      db "FAT16   "     ; file system type

;A contunuacion unas funciones ricas que ya conocemos de hacer antes:
;Imprime con el mensaje ya puesto con un lea en el registro %si
print:
	lodsb
	or        al, al
	jz        .fin
	mov       ah, 0xe
	mov       bx, 9
	int       0x10
	jmp print
.fin:
	ret

reboot:
	mov si, msgReiniciar
	call print
	xor ax,ax
	int 0x16 ; Espera a que se pulse una tecla
	;Codigo maquina muy turbio que reinicia,
	;en esencia deberia ser un jmp FFFF:0000
	db 0xEA
	dw 0x0000
	dw 0xFFFF


;Si fallo de disco:
falloDisco:
	mov si, msgDiskErr
	call print
	call reboot



;Aqui comienza nuestro tipico codigo xd
start:
		cli
		mov [iBootDrive], dl ;NUEVA LINEA -> Debería ser 0 (Disco del q booteamos, por eso no lo poniamos antes pero puede dar problemas si no lo ponemos, esta linea guarda el disco del que booteamos a nuestra estructura de datos)
		mov ax, 7C0h  ;Creamos DS
		mov ds, ax
		xor ax,ax     ;Creamos pila
		mov ss,ax
		mov sp,7BFFh
		sti
;Saludamos
		mov si, msgInicial
		call print

;NUEVO CODIGO (tampoco tan nuevo)
	;Para ahorrarnos iterar 5 veces sobre fallidos intentos de lectura xd, como haciamos antes, mejor hacer una int13 con subparametro 0 (reset disk state)
		mov  dl, iBootDrive   ; disco a resetear
  		xor  ax, ax           ; subfunction 0
  		int  0x13             ; int 13h
  		jc   falloDisco       ; si error, nos vamos a la puta
  		;Por ahora nada mas, añadimos mas mierda en el 9
  		jmp $

;DATOS DEL PROGRAMA:
msgInicial:    db 'Hola mundo!',13,10,0
msgCarga:      db 'Cargando TRES...',13,10,0
msgDiskErr:    db 'Error irrecuperable de disco.',13,10,0
msgReiniciar:  db 'Presiona una tecla para reiniciar.',13,10,0

; Magic numbers
times 510 - ($ - $$) db 0                       ; Rellenamos con 0.
dw 0xAA55                                       ; Ponemos el numero magico
