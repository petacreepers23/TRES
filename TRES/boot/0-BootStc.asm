;Este codigo tiene la implementacion de la fat.
;La idea es leer un kernel.bin que esté dentro del sistema de ficheros que hemos creado.
;Tenemos un fichero hola.txt o algo así que solo sirve para que el kernel no tenga porqué ser el
;primer fichero, así nos aseguramos que funcione bien.
;Despues ese kernel.bin contendrá un hola mundo desde el segundo sector, y pesará 1024 bytes
;La idea es cargar esos 2 secotres en memoria y que al ejecutarlos nos diga hola mundo desde la fat.
;Ese fichero ya no tiene porque tener marcado como ejecutable la firma de arrancabilidad 0x55AA.
;El codigo de ese fichero va a estar en 9-KernelBasico.asm
;En cuanto a este codigo tiene que tener la funcionalidad minima para poder cargar ese fichero,
;dar saludos o reiniciarse si algo va mal.
;dd if=so.img of=\\.\e: count=1


bits 16; Modo de 16 Bits
;%define LOAD_SEGMENT 0x1000 ;Sitio donde cargar el resto de mierdas, visto en  codigos (3-4-5)
org 0

main:	jmp start
		nop
		 	 	;Esto nos deja un offset de 0x3 justo debajo

bootinfo: ;La tabla que hay que poner, descrita en el 8

  iOEM:          db "TRES    "     ; OEM String
  iSectSize:     dw  0x200         ; bytes per sector
  iClustSize:    db  1             ; sectors per cluster
  iResSect:      dw  1             ; #of reserved sectors
  iFatCnt:       db  2             ; #of FAT copies
  iRootSize:     dw  224           ; size of root directory ENTRYS
  iTotalSect:    dw  5040;5760          ; total # of sectors if over 32 MB ; --> small ;4400
  iMedia:        db  0xF0;0xF8          ; media Descriptor ;F8
  iFatSize:      dw  23             ; size of each FAT
  iTrackSect:    dw  63;36             ; sectors per track
  iHeadCnt:      dw  16;2             ; number of read-write heads
  iHiddenSect:   dw  0             ; number of hidden sectors
                 dw  0			   ; 4bytes, w=16 bits
  iSect32:       dw  0x0000             ; # sectors for over 32 MB --> big son 4 bytes de numero
                 dw  0x0000			   ; 4bytes, w=16 bits  ->  512Mb eso estaba ants, no se porque #dw  0x0010
  iBootDrive:    db  80h             ; holds drive that the boot sector came from
  iReserved:     db  0             ; reserved, empty
  iBootSign:     db  0x29          ; extended boot sector signature
  iVolID:        db "tres"         ; disk serial
  acVolumeLabel: db "BOOTVOLUME "  ; volume label
  acFSType:      db "FAT16   "     ; file system type


;IMPORTANTE QUE EL START ESTE AQUI, PORQUE SINO EL DESPLAZAMIENTO
;SE PRODUCE A DEMASIADA DISTANCIA Y EL OFFSET DE 0X3 PASA A SER
;DE 0X4 Y SE VA TODO A LA PUTA

start:
		; The MBR needs to start with 33h, because some dumb BIOSes check for that (cf. syslinux commit d0f275981c9289dc4b8df64e72cd9902bf85aebe).
        ; The following line encodes a "xor ax, ax" (but it's not the only way to encode it, so we can't just write that instruction here).
        db 0x33; .byte HEX(33), HEX(C0)
		cli
		;mov byte [iBootDrive], dl ; En algunas bios te modifica el dl y en otras te modifica el bpb y hay sopresas
		mov ax, 7C0h  ;Creamos DS
		mov ds, ax
		;..
		mov     es, ax
        mov     fs, ax
        mov     gs, ax
		;-- Esto no se si es util
		xor ax,ax     ;Creamos pila
		mov ss,ax
		mov sp,7C00h;
		sti 
		;1-Busqueda del root entry
		call getRootEntry

;-----------------------
; Cargar codigo c++ en direccion 10000h
;-----------------------
        mov ax,[rootsector]
	    mov cl,0xE ; cx = Numero de sectores que ocupa la root entry.
	    mov bx, 8400h ; 7c0:8400h-> 7c0*10h + 8400h -> 100000
        mov word [currentName],kernelFile
        call loadRootEntry
        call loadFAT
        mov word [current_cluster],8400h
        call cargar_kernel
;-----------------------
; Cargar codigo scndstage en direccion es:600h
;-----------------------
		mov ax,[rootsector]
	    mov cl,0xE ; cx = Numero de sectores que ocupa la root entry.
	    mov bx, 600h ; 7c0:8400h-> 7c0*10h + 8400h -> 100000
        mov word [currentName],sndstgFile
        call loadRootEntry
        call loadFAT
        mov word [current_cluster],600h
        call cargar_kernel

;...................................
;Saltamos a la dirección 600
;...................................
		jmp 7c0h:600h;far [7c00h+600h]  ;7c0h:600h ; stage 2.
		;jmp 7c0h:8400h

		jmp reboot

		;2-Saltar al cluster de la fat getRootEntry.de turno, y leer secotres por cluster en memoria
	
		;call reboot
		
		
		;3-Saltar al siguiente cluster o, si hemos acabado, parar.

		;4-Es hora de hacer el salto a donde hayamos cargado. Que dios nos pille confesados.






;----------------------------------------------------------------------------------------
; Convertir de LBA a CHS
; parametros 
; ax -> coord LBA
; returns:
; cl -> S , ch -> C , dh -> H
;----------------------------------------------------------------------------------------
LBAtoCHS:
	; LBA es el sector deseado(recuerda que el primer sector es el zero.) sectors_per_track y number_of_head se def en la cabecera.
	; Sector   = (LBA mod sectors_per_track )+1
	; Cylinder = (LBA / sectors_per_track) / number_of_heads
	; Head     = (LBA / sectors_per_track) mod number_of_heads
	; llamas luego a int 13h subf 2 donde C->ch S->cl H->dh
	; ax contiene la coordenada CHS.
	mov     bx, [iTrackSect]              ; Get sectors per track
  	xor     dx, dx
  	div     bx                          ; Divide (dx:ax/bx to ax,dx)
  	                                    ; Quotient (ax) =  LBA / SectorsPerTrack
  	                                    ; Remainder (dx) = LBA mod SectorsPerTrack
  	inc     dx                          ; Add 1 to remainder, since sector ; DX contiene el Sector.
  	mov     cl, dl                      ; Store result in cl for int 13h call.

  	mov     bx, [iHeadCnt]                ; Get number of heads
  	xor     dx, dx
  	div     bx                          ; Divide (dx:ax/bx to ax,dx)
  	                                    ; Quotient (ax) = Cylinder
  	                                    ; Remainder (dx) = head
  	mov     ch, al                      ; ch = cylinder                      
  	xchg    dl, dh                      ; dh = head number     
    ret
; ahora: cl -> S . ch -> C dh -> H		
;----------------------------------------------------------------------------------------

;------------------------------------------------------------------------------------------------
;;Leemos N sectores con N = tamaño de cluster
;;parametros:
;cl = Numero de sectores a cargar
;ax = Numero de sector inicial (LBA)
;bx = Destino desde offset 7C0h (igual que ds)-> es:bx=7c0h:bx-> es*10h+bx
leerNSector:
		;Convertimos de lba a chs:
		push bx
		push cx

		call LBAtoCHS ; param ax: LBA return: cl: S  ch: C dh: H	

		pop ax; esto va con el push cx que pone bien los sectores a cargar

        mov ah, 02h  ;mas documentacion en 2-CargaSector2.asm        
    	;mov al, 0x01 ; ELIMINAR LUEGO. SOLO PERRMITE CARGAR UN sector cada vez
		mov dl, [iBootDrive];80h;[iBootDrive]     
    	mov bx, 7C0h  ;si ponemos un 0 aqui, la liamos muy fuertemente
    	mov es, bx    ;es <- 7C0h y en bx hace un pop para tener la dir 8000h
    	pop bx ;;el del principio
.lectura:
    	int 0x13       							; interupcion del DISCO de la BIOS
		;jc .lectura
		; TODO: poner un contador, hay que reintentar o en un futuro fallara 1 hora pa ver que aquí fallaba
		jc falloDisco							; si falla, no reintenter
		retw
;---------------------------------------------------------------------------------------------

;mensaje en si
print:
	lodsb
	cmp al, 0
	je        .fin
	mov       ah, 0xe
	mov       bx, 9
	int       0x10
	jmp print
.fin:
	retw

;no params
reboot:
	mov si, msgReiniciar
	call print
	xor ax,ax
	int 0x16 ; Espera a que se pulse una tecla
	db 0xEA
	dw 0x0000
	dw 0xFFFF
;
;;;Si fallo de disco:
falloDisco:
	mov si, msgDiskErr
	call print
	call reboot
;
;Si fallo no encontrado
falloNoEncontrado:
	mov si,msgNotFound
	call print
	retw
	;call reboot




;Funcion que recibe el nombre de fichero.extension en %si
;devuelve en ax la direccion del dir o se pira
getRootEntry:
;-------------
; CALCULAR incio del ROOT DIRECTORY.
;-------------
    ;1: Calculamos la dirección LBA donde esta el 1 sector de la root entry
	;(1 + 23+23) * 512 =  = 5E00hex 
	; ((iFatCnt * iFatSize) +iResSect) ; De start of root sector. para obtener la direc deberías de * sectorSize

	xor ax, ax
	xor bx, bx
	mov byte al, [iFatCnt] ; ax = numero de tablas fat
	;mov bx,[iFatSize] ; bx = tam de las tablas fat en sectores.
	mul word [iFatSize]; multiplicamos ax*bx. = tam total tablas fat en sectores.
	add ax,[iResSect] ; total tablas fat + sectores reservados
	; AX = sector donde empieza el root directory.
    mov [rootsector],ax
	add ax, 0xE
	mov [datasector],ax
	sub ax,0xE
	;dec ax
;------------
; Leer los 32 sectores de la root entry.
;------------

    ret
loadRootEntry:
    push cx
    push ax
    mov bx, 400h
    mov cl, 1h
    ;cl = num sectores a cargar
	;ax = num sector inicial LBA
	;bx = destino es:bx-> es*10h+bx =dir donde se cargan los cl sectores.
	call leerNSector
; 32 sectores van desde la dir 8000-> A184
    ;mov si,kernelFile
    ;push ax
    ;mov ax,kernelFile

    ;pop ax
	call chekEntry
    pop ax
    pop cx
    
    inc ax
   
    cmp bx,1h
    je rootEntryCargada
    dec cl
    cmp cl,0000h
    jne loadRootEntry
rootEntryCargada: 
    ret

;-------------
;HAY QUE CREAR UN BUCLE; PARA Q ESTO LO HAGA CON CADA ENTRADA.
;-------------

;############################
;#	 Cargar la FAT.
;############################
loadFAT:
	;mov bx, 400h	
    add word [root_entry_bx],1Ah
    ;mov word ax ,[root_entry_bx]
    mov bx,[root_entry_bx]
	mov  ax, [bx]
	mov word [cluster_of_kernel], ax


;------------------
; Calcular tamaño FAT. Guardar en CX
;------------------
	xor ax,ax;
	;mov byte al, [iFatCnt]
	;mul word [iFatSize]
	
	mov word cx, [iFatSize]
	mov word ax,[iResSect]

	mov bx,400h
	;cl = num sectores a cargar
	;ax = num sector inicial LBA
	;bx = destino es:bx-> es*10h+bx =dir donde se cargan los cl sectores.
	call leerNSector
	;en este punto está cargada la fat en el es:bx y en [cluster_of_kernel] el nº de sector
	;Ahora habria que cargar el sector fichero en su dir destino es
	
    ret
cargar_kernel:
	mov word ax, [cluster_of_kernel]
	sub ax,2
	add ax,[datasector]
	mov cl,1h
	mov word bx,[current_cluster]
	call leerNSector
	;Leemos el elmento sig le la linked list de la fat.
	mov ax,2h
	mul word [cluster_of_kernel]
	;add ax,2h
	add ax,400h
	mov bx,ax
	mov ax,[bx];aqui
	mov word [cluster_of_kernel],ax
	;if)=...
	add word [current_cluster],200h
	cmp ax,0xff7 
	JBE cargar_kernel

	mov si,msgkernelCargado
	call print
    ret



;------------------------------
; Iteramos hasta encontrar la entrada.
; Iteramos 256 veces.
;------------------------------
;
; param
; En si , el registro, debería ir el nombre de fich buscado.
;es:bx direccion de memoria donde esta el texto
;kernelFile msg con el que se compara.
; 16 veces se ejecuta, revisa todo el sector.
chekEntry:
	.iteracion:
		;4-para cada iteración comprobar que, en mayusculas "NOMBRE  EXT" sean iguales.
        ; push bx
        ; mov bx, [currentName]
        ; xor esi,esi
        ; mov si, bx
		mov si,[currentName]
        ;pop bx
		mov di,bx
		;En al tenemos el valor esperado despues de hacer lodsb, en si el puntero
		;En di apuntamos a el inicio de el dirEntry

		mov ch,11 ; 11 es el tamaño del nombre del fichero(8) + extension(3)
	.comparacion: ; Check entry. SI file == searched file.
		cmp ch,0
		je .fincompbien
		lodsb
		mov cl,byte [di]
		cmp cl,al
		jne .fincompmal ;si no son iguales nos piramos
		inc di;di++
		dec ch;bh--
		jmp .comparacion
	.fincompbien:
	;6-devolver en algun registro ax, a donde apunta esto y vamos viendo
    mov word [root_entry_bx],bx
	mov si, msgRootEncontrado
	call print
    mov bx,1
	jmp .salir
	.fincompmal:
        cmp bx,600h
        je .finmalmal
        add bx,20h
        jmp .iteracion
    .finmalmal:
		je falloNoEncontrado
		mov ax,dx
	.salir:
	retw






;DATOS DEL PROGRAMA: 
;---
cluster_of_kernel: dw 0x0000
datasector  dw 0x0000
rootsector  dw 0x0000
current_cluster     dw 0x0000
rootdir_start: dw  0
index_aux: db 0
root_entry_bx: dw 0x0000
;---
msgkernelCargado: db 'Ke',13,10,0
msgRootEncontrado: db 'rot',13,10,0
msgNotFound:   db 'N k',13,10,0
msgDiskErr:    db 'r',13,10,0;'Error de disco',13,10,0
msgReiniciar:  db 'R',13,10,0
currentName: dw 0x0000
kernelFile:    db 'KERNEL  BIN',0
sndstgFile:    db 'SNDSTG  BIN',0


; Magic numbers
times 510 - ($ - $$) db 0                       ; Rellenamos con 0.
dw 0xAA55                                       ; Ponemos el numero magico
