;Este codigo tiene la implementacion de la fat.


bits 16; Modo de 16 Bits
%define LOAD_SEGMENT 0x1000 ;Sitio donde cargar el resto de mierdas, visto en  codigos (3-4-5)

main:
		jmp start
		nop 	 	;Esto nos deja un offset de 0x3 justo debajo

bootinfo: ;La tabla que hay que poner, descrita en el 8

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
  iSect32:       dw  0             ; # sectors for over 32 MB
  iBootDrive:    db  0             ; holds drive that the boot sector came from
  iReserved:     db  0             ; reserved, empty
  iBootSign:     db  0x29          ; extended boot sector signature
  iVolID:        db "seri"         ; disk serial
  acVolumeLabel: db "MYVOLUME   "  ; volume label
  acFSType:      db "FAT16   "     ; file system type


;Funcion que lee sectores CHA pidiendole uno como LBA 
readSector:
	xor     cx, cx                      ; intentos = 0
	
	.readsect:
	push    ax                          ; parametro: bloque logico
	push    cx                          ; parametro: numero de intentos anterior
	push    bx                          ; parametro: offset
	
	; Calculo de CHA a partir de lba:
	; Cylinder = (LBA / SectorsPerTrack) / NumHeads
	; Sector   = (LBA % SectorsPerTrack) + 1
	; Head     = (LBA / SectorsPerTrack) % NumHeads
	
	mov     bx, iTrackSect              ; SectorsPerTrack lo metemos en bx
	xor     dx, dx
	div     bx                          ; Dividir (dx:ax/bx a ax,dx)
	                                    ; Cociente (ax) =  LBA / SectorsPerTrack
	                                    ; Resto (dx) = LBA % SectorsPerTrack
	inc     dx                          ; se le añade 1 al resto, por el tema de que empiezan por 1
	mov     cl, dl                      ; Se guarda el resultado en cl, para dejarlo listo pa la llamada a int13
	
	mov     bx, iHeadCnt                ; NumHeads
	xor     dx, dx
	div     bx                          ; Dividir (dx:ax/bx a ax,dx)
	                                    ; Cociente (ax) = los cilindros
	                                    ; Resto (dx) = cabezas
	mov     ch, al                      ; ch = los cilindros                      
	xchg    dl, dh                      ; dh = numero de cabezas
	
	; Llamada tipica a int13

	mov     ax, 0x0201                  ; leer(2) secotres(1)
	mov     dl, iBootDrive              ; disco actual
	pop     bx                          ; con el ofset del parametro
	int     0x13
	jc      .readfail
	
	; si va dpm, volvemos.
	pop     cx                          ; sacamos parametros
	pop     ax                          ; sacamos parametros
	ret
	
	; si falla
	.readfail:   
	pop     cx                          ; numero de intentos
	inc     cx                          ; ++
	cmp     cx, 4              ; si == 4, adios
	je      falloDisco
	
	; si !=4, reset:
	xor     ax, ax
	int     0x13
	
	; y reintentamos, preservando el parametro inicial.
	pop     ax
	jmp     .readsect

;funcion que lee un sector desde el directorio root de la fat.
; Load a sector from the root directory.
; If sector reading fails, a reboot will occur.
read_next_sector:
push   cx
push   ax
xor    bx, bx
call   readSector
 
check_entry:
mov    cx, 11                      ; Directory entries filenames are 11 bytes.
mov    di, bx                      ; es:di = Directory entry address
lea    si, filename                ; ds:si = Address of filename we are looking for.
repz   cmpsb                       ; Compare filename to memory.
je     found_file                  ; If found, jump away.
add    bx, 32             ; Move to next entry. Entries are 32 bytes.
cmp    bx, iSectSize      ; Have we moved out of the sector yet?
jne    check_entry                 ; If not, try next directory entry.
 
pop    ax
inc    ax                          ; check next sector when we loop again
pop    cx
loopnz read_next_sector            ; loop until either found or not
jmp    bootFailure                 ; could not find file: abort

;Funcion que, tras haber leido de el root directory un fichero, nos da el numero de cluster
found_file:
    ; The directory entry stores the first cluster number of the file
    ; at byte 26 (0x1a). BX is still pointing to the address of the start
    ; of the directory entry, so we will go from there.
    ; Read cluster number from memory:
    mov    ax, es:[bx+0x1a]
    mov    file_strt, ax




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
	retw

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


;Para poder usar la fat, tenemos que rellenar en el arranque,
;datos de la estructura principal, como el tamaño del directorio raiz
;Calculo del tamaño del root directory
;tamaño = 32 * numero de entradas / 512
mov     ax, 32
xor     dx, dx
mul     iRootSize
div     iSectSize          ; Divide (dx:ax,sectsize) to (ax,dx)
mov     cx, ax
mov     root_scts, cx

;Tambien tenemos que calcular donde esta ese root directory,
;esto se hace 
;sumando el numero de tablas FAT * los esctores que ocupan,
;sumando el numero de sectores ocultos (0)
;sumando el numero de sectores reservados (0)
xor   ax, ax                      ; find the root directory
mov   al, iFatCnt        ; ax = number of FAT tables
mov   bx, iFatSize       ; bx = sectors per FAT
mul   bx                          ; ax = #FATS * sectors per FAT
add   ax, iHiddenSect    ; Add hidden sectors to ax
adc   ax, iHiddenSect+2
add   ax, iResSect       ; Add reserved sectors to ax
mov   root_strt, ax
; root_strt is now the number of the first root sector


  		jmp $

;DATOS DEL PROGRAMA:
msgInicial:    db 'Hola mundo!',13,10,0
msgCarga:      db 'Cargando TRES...',13,10,0
msgDiskErr:    db 'Error irrecuperable de disco.',13,10,0
msgReiniciar:  db 'Presiona una tecla para reiniciar.',13,10,0

root_strt:   db 0,0      ; hold offset of root directory on disk
root_scts:   db 0,0      ; holds # sectors in root directory
file_strt:   db 0,0      ; holds offset of bootloader on disk


; Magic numbers
times 510 - ($ - $$) db 0                       ; Rellenamos con 0.
dw 0xAA55                                       ; Ponemos el numero magico