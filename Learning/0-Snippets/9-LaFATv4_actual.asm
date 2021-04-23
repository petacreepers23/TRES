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
;pensado para un flopy disk de 1.44M
bootinfo: ;La tabla que hay que poner, descrita en el 8

  iOEM:          db "TRES    "     ; OEM String
  iSectSize:     dw  0x200         ; bytes per sector
  iClustSize:    db  1             ; sectors per cluster
  iResSect:      dw  1             ; #of reserved sectors
  iFatCnt:       db  2             ; #of FAT copies
  iRootSize:     dw  224           ; size of root directory ENTRYS
  iTotalSect:    dw  2880          ; total # of sectors if over 32 MB
  iMedia:        db  0xF0          ; media Descriptor
  iFatSize:      dw  9             ; size of each FAT
  iTrackSect:    dw  9             ; sectors per track
  iHeadCnt:      dw  2             ; number of read-write heads so 160cylinders
  iHiddenSect:   dw  0             ; number of hidden sectors
                 dw  0			   ; 4bytes, w=16 bits
  iSect32:       dw  0             ; # sectors for over 32 MB
                 dw  0			   ; 4bytes, w=16 bits
  iBootDrive:    db  80h             ; holds drive that the boot sector came from
  iReserved:     db  0             ; reserved, empty
  iBootSign:     db  0x29          ; extended boot sector signature
  iVolID:        db "tres"         ; disk serial
  acVolumeLabel: db "BOOTVOLUME "  ; volume label
  acFSType:      db "FAT12   "     ; file system type

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
	mov ax, 19;;sector inicial (19)
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

;----------------------------------------------------------------------------------------


;Leemos N sectores con N = tamaño de cluster
;parametros:
;cl = Numero de sectores a cargar
;ax = Numero de sector inicial (LBA)
;bx = Destino desde offset 7C0h (igual que ds)
leerNSector:
		;Convertimos de lba a chs:
		push bx
		push cx
		
		call lbaTOchs;ax: H 
					 ;bx: LBA 
					 ;cx: C 
					 ;dx: S
		
		mov dh,al;preparamos la cabeza en su sitio
		shl cx,6;cilindros gestionados
		or cx,dx;sectores gestionados

		pop ax; esto va con el push cx que pone bien los sectores a cargar

        mov ah, 02h  ;mas documentacion en 2-CargaSector2.asm        
    	mov dl, 80h;[iBootDrive]     
    	mov bx, 7C0h  ;si ponemos un 0 aqui, la liamos muy fuertemente
    	mov es, bx    
    	pop bx ;;el del principio
.lectura:
    	int 0x13       							; interupcion del DISCO de la BIOS
		jc falloDisco							; si falla, no reintenter
		retw

;----------------------------------------------------------------------------------------

;Convertir de LBA a CHS
;params: ax -> coord LBA
;returns: ax: H bx: LBA cx: C dx: S
LBAtoCHS:
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
; ahora: cl -> S . ax -> C dh -> H		
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








;DATOS DEL PROGRAMA:
msgNotFound:   db 'No kernel',13,10,0
msgDiskErr:    db 'Error de disco',13,10,0
msgReiniciar:  db 'Reinicia!',13,10,0
kernelFile:    db 'KERNEL  BIN',0

; Magic numbers
times 510 - ($ - $$) db 0                       ; Rellenamos con 0.
dw 0xAA55                                       ; Ponemos el numero magico
