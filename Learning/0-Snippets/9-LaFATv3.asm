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
%define LOAD_SEGMENT 0x1000 ;Sitio donde cargar el resto de mierdas, visto en  codigos (3-4-5)

main:
		jmp start
		nop 	 	;Esto nos deja un offset de 0x3 justo debajo

bootinfo: ;La tabla que hay que poner, descrita en el 8

  iOEM:          db "TRES    "     ; OEM String
  iSectSize:     dw  0x200         ; bytes per sector
  iClustSize:    db  32             ; sectors per cluster
  iResSect:      dw  32             ; #of reserved sectors
  iFatCnt:       db  2             ; #of FAT copies
  iRootSize:     dw  512           ; size of root directory ENTRYS
  iTotalSect:    dw  0          ; total # of sectors if over 32 MB ; --> small
  iMedia:        db  0xF8          ; media Descriptor
  iFatSize:      dw  256             ; size of each FAT
  iTrackSect:    dw  63             ; sectors per track
  iHeadCnt:      dw  255             ; number of read-write heads
  iHiddenSect:   dw  0             ; number of hidden sectors
                 dw  0			   ; 4bytes, w=16 bits
  iSect32:       dw  0x0000             ; # sectors for over 32 MB --> big son 4 bytes de numero
                 dw  0x0010			   ; 4bytes, w=16 bits  ->  512Mb
  iBootDrive:    db  80h             ; holds drive that the boot sector came from
  iReserved:     db  0             ; reserved, empty
  iBootSign:     db  0x29          ; extended boot sector signature
  iVolID:        db "tres"         ; disk serial
  acVolumeLabel: db "BOOTVOLUME "  ; volume label
  acFSType:      db "FAT16   "     ; file system type


;Los clusteres son de 4000h bytes aka 32 sectores
;
;La fat 1 ahora empieza en la 4000h
;La fat 2 ahora empieza en la 24000h
;Datos empiezan en 40000h
;La root entry que nos interesa:
;00044000  41 6b 00 65 00 72 00 6e  00 65 00 0f 00 da 6c 00  |Ak.e.r.n.e....l.|
;00044010  2e 00 62 00 69 00 6e 00  00 00 00 00 ff ff ff ff  |..b.i.n.........|
;00044020  4b 45 52 4e 45 4c 20 20  42 49 4e 20 00 c1 82 52  |KERNEL  BIN ...R|
;00044030  52 51 53 51 00 00 82 52  52 51 03 00 00 04 00 00  |RQSQ...RRQ......|
; el inicio del cluster esta en 03 00 el size en 00 04 00 00 que como es little endian. se lee: 00 00 04 00 -> 400hex->1024 bytes.
; la root entry NO OCUPA 32 bytes no, OCUPA  64 la primera entrada no se que coño es. la segunda es la que nos interesa.

;KERNEL.BIN esta en 40000h+4000h*3h = 4C000h
;Aqui comienza nuestro tipico codigo xd
;IMPORTANTE QUE EL START ESTE AQUI, PORQUE SINO EL DESPLAZAMIENTO
;SE PRODUCE A DEMASIADA DISTANCIA Y EL OFFSET DE 0X3 PASA A SER
;DE 0X4 Y SE VA TODO A LA PUTA
start:

		cli
		mov byte [iBootDrive], 80h ;NUEVA LINEA -> Debería ser 0 (Disco del q booteamos, por eso no lo poniamos antes pero puede dar problemas si no lo ponemos, esta linea guarda el disco del que booteamos a nuestra estructura de datos)
		mov ax, 7C0h  ;Creamos DS
		mov ds, ax
		xor ax,ax     ;Creamos pila
		mov ss,ax
		mov sp,7BFFh
		sti
		call enableA20
		cli
		lgdt [GDT_START]
		mov eax, cr0 ;Indicar q movemos a modo protegido, no podemos operar directamente sobre cr0 asi q lo movemos
		or eax,1;El bit 1 indica lo de arriba
		mov cr0,eax;modificamos el registro de estado.

		jmp CODE_SEG:modoTreintaYDos

		call modoTreintaYDos

		;1-Busqueda del root entry de kernel.bin
		call getRootEntry
			;comprobamos visualmente q lo ha leido bien
		push ax
		mov si,ax
		call print

		;2-Saltar al cluster de la fat de turno, y leer secotres por cluster en memoria
			;cargamos la fat en 4000h
			;cl = Numero de sectores a cargar
					;mov cl,9
			;ax = Numero de sector inicial (LBA)
					;mov ax,1
			;bx = Destino desde offset 7C0h (igual que ds)
					;mov bx,4000h
					;call leerNSector


			;leemos primer cluster
			pop ax
			add ax,1Ah
			mov si,ax
			mov ax,[si];ax==primer cluster
			
			;push ax

			;cargar el sector ax(4) de la region de datos en memoria.
			;18+1+14->inicio zona datos. (32)
			add ax,30;derian ser 31 (33-2)

			mov cl,18
			mov bx,1000h
			call leerNSector
			mov si,1000h
			call print

		call reboot
		
		
		;3-Saltar al siguiente cluster o, si hemos acabado, parar.

		;4-Es hora de hacer el salto a donde hayamos cargado. Que dios nos pille confesados.
[bits 32]
modoTreintaYDos:
 	mov ax, DATA_SEG            ; Now in PM, our old segments are meaningless
    mov ds, ax                  ; so we point our segment registers to the data selector defined GDT
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
	mov ebp, 0x90000            ; Move stack   
    mov esp, ebp
	;prueba hola m
    mov si, 0xb8000 ; si = 0; si es el source index. sirve en general para apun a cadenas.
    mov [si], byte 'H' ; metemos A en la dir de mem de si.
    add si, 2 ; aumentamos el puntero por 2. 2 bytes.
    mov [si], byte 'O'
    add si, 2
    mov [si], byte 'L'
    add si, 2
    mov [si], byte 'A'
    add si, 2
    mov [si], byte 'A'

    jmp $
    

[bits 16]
;ax numero a imprimir. no lo modifica
printDouble:
	push ax
	mov dl,ah
	call printNumber
	pop ax
	push ax
	mov dl,al
	call printNumber
	pop ax
	retw

;dl numero a imprimir
printNumber:
	xor dh,dh
	mov ax,dx
	mov bx, 9

	mov cl,100
	div cl
	add al,'0'
	mov ah, 0xe
	int 0x10

	mov ax,dx
	mov cl,10
	div cl
	xor ah,ah
	div cl
	add ah,'0'
	mov al,ah
	mov ah, 0xe
	int 0x10

	mov ax,dx
	mov cl,10
	div cl
	add ah,'0'
	mov al,ah
	mov ah, 0xe
	int 0x10
	mov al,' '
	int 0x10
	retw



;;Convertir de LBA a CHS
;;params: ax -> coord LBA
;;returns: ax: H bx: LBA cx: C dx: S
;lbaTOchs:
;		;Cilindros max = ch (10 bits) -> cilindros     		;C = lba / (dh * (cl -> 6 bits))
;		;Sector max = cl (6 bits) -> sectores/pista    		;resto = lba mod (dh * (cl -> 6 bits))
;		;Head max = dh (8 bits) -> pistas/cilindro    		;H = resto / (cl -> 6 bits)
;	mov bx,16065;bx=(dh * (cl -> 6 bits))             		;S = resto mod (cl -> 6 bits) + 1
;	xor dx,dx
;	;mov ax,[ss:sp];recargamos ax (lba)
;	push ax
;	div bx
;	push ax;Coordenada C
;	mov ax,dx;movemos el resto a ax
;	xor dx,dx
;	mov bx,63
;	div bx
;	;ax = H
;	inc dx
;	;dx = S
;	pop cx
;	pop bx
;
;	retw
;	
;----------------------------------------------------------------------------------------

;Convertir de LBA a CHS
;params: ax -> coord LBA
;returns: ax: H bx: LBA cx: C dx: S
lbaTOchs:
	; LBA es el sector deseado(recuerda que el primer sector es el zero.) sectors_per_track y number_of_head se def en la cabecera.
	; Sector   = (LBA mod sectors_per_track )+1
	; Cylinder = (LBA / sectors_per_track) / number_of_heads
	; Head     = (LBA / sectors_per_track) mod number_of_heads
	; llamas luego a int 13h subf 2 donde C->ch S->cl H->dh
	; ax contiene la coordenada CHS.
	mov     bx, iTrackSect              ; Get sectors per track
  	xor     dx, dx
  	div     bx                          ; Divide (dx:ax/bx to ax,dx)
  	                                    ; Quotient (ax) =  LBA / SectorsPerTrack
  	                                    ; Remainder (dx) = LBA mod SectorsPerTrack
  	inc     dx                          ; Add 1 to remainder, since sector ; DX contiene el Sector.
  	mov     cl, dl                      ; Store result in cl for int 13h call.

  	mov     bx, iHeadCnt                ; Get number of heads
  	xor     dx, dx
  	div     bx                          ; Divide (dx:ax/bx to ax,dx)
  	                                    ; Quotient (ax) = Cylinder
  	                                    ; Remainder (dx) = head
  	mov     ch, al                      ; ch = cylinder                      
  	xchg    dl, dh                      ; dh = head number     
    retw
; ahora: cl -> S . ch -> C dh -> H		
;----------------------------------------------------------------------------------------
		

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

;Si fallo de disco:
falloDisco:
	mov si, msgDiskErr
	call print
	call reboot

;Si fallo no encontrado
falloNoEncontrado:
	mov si,msgNotFound
	call print
	call reboot

;Leemos N sectores con N = tamaño de cluster
;parametros:
;cl = Numero de sectores a cargar
;ax = Numero de sector inicial (LBA)
;bx = Destino desde offset 7C0h (igual que ds)
leerNSector:
		;Convertimos de lba a chs:
		push bx
		push cx
		
		call lbaTOchs ;cl -> S . ch -> C dh -> H
				;Antes:
					 ;ax: H   
					 ;bx: LBA 
					 ;cx: C 
					 ;dx: S
		
		;mov dh,al;preparamos la cabeza en su sitio
		;shl cx,6;cilindros gestionados
		;or cx,dx;sectores gestionados

		pop ax; esto va con el push cx que pone bien los sectores a cargar

        mov ah, 02h  ;mas documentacion en 2-CargaSector2.asm        
    	mov dl, 80h;[iBootDrive]     
    	mov bx, 7C0h  ;si ponemos un 0 aqui, la liamos muy fuertemente
    	mov es, bx    ;es <- 7C0h y en bx hace un pop para tener la dir 8000h
    	pop bx ;;el del principio
.lectura:
    	int 0x13       							; interupcion del DISCO de la BIOS
		jc falloDisco							; si falla, no reintenter
		retw



;Funcion que recibe el nombre de fichero.extension en %si
;devuelve en ax la direccion del dir o se pira
getRootEntry:
	;1-skip sectors reservados
		;Tenemos 0 en nuestro caso
	;2-skip los sectores de la fat, la normal y la copia
		;1(Boot)+18(Fa1,Fat2) = 19
		;Cargamos en memoria el root directory
		;32*224/512=14 sectores ->Desde 8000h hasta 9C00h sera.
	mov cl, 14;a leer
	mov ax, 19;sector inicial (19)
	mov bx, 8000h
	call leerNSector

	
	;3-iterar de 32 en 32 sobre el root directory hasta acabar la totalidad del root directory
	mov dx,8000h
	.iteracion:
		cmp dx,9C00h ;vamos avanzando hasta q nos salimos del rootdir
		je .salir

		;4-para cada iteración comprobar que, en mayusculas "NOMBRE  EXT" sean iguales.
		mov si,kernelFile
		mov di,dx
		;En al tenemos el valor esperado despues de hacer lodsb, en si el puntero
		;En di apuntamos a el inicio de el dirEntry
		;bl el valor
		;bh un 11
		mov bh,11
	.comparacion:
		cmp bh,0
		je .fincompbien
			;esto nos carga los datos
		lodsb
		mov bl,byte [di]
		cmp bl,al
		jne .fincompmal ;si no son iguales nos piramos
		inc di;di++
		dec bh;bh--
		jmp .comparacion
	.fincompmal:
		add dx,32
		jmp .iteracion
	;ir al siguiente ;5-si no son iguales, ir a 3, sino continuar.
	.fincompbien:
	;6-devolver en algun registro ax, a donde apunta esto y vamos viendo
	.salir:
		cmp dx,9C00h
		je falloNoEncontrado
		mov ax,dx
	retw




 
; Function: check_a20
;
; Purpose: to check the status of the a20 line in a completely self-contained state-preserving way.
;          The function can be modified as necessary by removing push's at the beginning and their
;          respective pop's at the end if complete self-containment is not required.
;
; Returns: 0 in ax if the a20 line is disabled (memory wraps around)
;          1 in ax if the a20 line is enabled (memory does not wrap around)
check_a20:
    pushf
    push ds
    push es
    push di
    push si
 
    cli
 
    xor ax, ax ; ax = 0
    mov es, ax
 
    not ax ; ax = 0xFFFF
    mov ds, ax
 
    mov di, 0x0500
    mov si, 0x0510
 
    mov al, byte [es:di]
    push ax
 
    mov al, byte [ds:si]
    push ax
 
    mov byte [es:di], 0x00
    mov byte [ds:si], 0xFF
 
    cmp byte [es:di], 0xFF
 
    pop ax
    mov byte [ds:si], al
 
    pop ax
    mov byte [es:di], al
 
    mov ax, 0
    je check_a20__exit
 
    mov ax, 1
 
check_a20__exit:
    pop si
    pop di
    pop es
    pop ds
    popf
 
    ret

enableA20:
	xor ax, ax
	call check_a20
	cmp ax,1;
	je enaA20exit;
	;probamos con la bios
	mov ax, 2401h;
	int 15;
	jnc enaA20exit;salimos si CF=0
	;probamos con el puerto 092
	in al, 0x92 ; lee del puerto 92 y lo mete en al.
	or al, 2 ; pone el bit 2 a 1
	out 0x92, al ; y lo vuelve a escribir en el puerto.
	;check
	xor ax, ax
	call check_a20
	cmp ax,1;
	je enaA20exit;
	mov si, msgNoA20;
	call print;
	call reboot;
enaA20exit:	
	ret

;DATOS DEL PROGRAMA:
msgNoA20:	   db 'Error A20',13,10,0
msgNotFound:   db 'No kernel',13,10,0
msgDiskErr:    db 'Error de disco',13,10,0
msgReiniciar:  db 'Reinicia!',13,10,0
kernelFile:    db 'KERNEL  BIN',0

;GDT
GDT_START: 
;null descriptor 
dd 0 
dd 0  ;reservado por intel 
;data descriptor 
gdt_code:
dw 0xFFFF 
dw 0 
db 0 
db 10011010b 
db 11001111b 
db 0 
;code descriptor 
gdt_data:
dw 0xFFFF 
dw 0 
db 0 
db 10010010b 
db 11001111b 
db 0 
GDT_END: 
align 4 
GDT_32: 
dw GDT_END - GDT_START - 1 
dd GDT_START

CODE_SEG equ gdt_code - GDT_START
DATA_SEG equ gdt_data - GDT_START

; Magic numbers
times 510 - ($ - $$) db 0                       ; Rellenamos con 0.
dw 0xAA55                                       ; Ponemos el numero magico
