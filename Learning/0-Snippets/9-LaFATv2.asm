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
  iClustSize:    db  1             ; sectors per cluster
  iResSect:      dw  1             ; #of reserved sectors
  iFatCnt:       db  2             ; #of FAT copies
  iRootSize:     dw  224           ; size of root directory ENTRYS
  iTotalSect:    dw  2880          ; total # of sectors if over 32 MB
  iMedia:        db  0xF0          ; media Descriptor
  iFatSize:      dw  9             ; size of each FAT
  iTrackSect:    dw  9             ; sectors per track
  iHeadCnt:      dw  2             ; number of read-write heads
  iHiddenSect:   dw  0             ; number of hidden sectors
                 dw  0			   ; 4bytes, w=16 bits
  iSect32:       dw  0             ; # sectors for over 32 MB
                 dw  0			   ; 4bytes, w=16 bits
  iBootDrive:    db  80h             ; holds drive that the boot sector came from
  iReserved:     db  0             ; reserved, empty
  iBootSign:     db  0x29          ; extended boot sector signature
  iVolID:        db "tres"         ; disk serial
  acVolumeLabel: db "BOOTVOLUME "  ; volume label
  acFSType:      db "FAT16   "     ; file system type

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
;Saludamos
		;mov si, msgInicial
		;call print
;Configuramos cosas necesarias
		;TODO: hacerlo
;Imprimimos que la configuraicon ha ido bien y procedemos a cargar TRES
		;1-Leer cada entrada del root directory y comparar el nombre y extensión con el de la etiqueta kernelfile
		mov si,kernelFile
		call getRootEntry
		;2-Saltar al cluster de la fat de turno, y leer secotresporcluster en memoria
		push ax
		;mov si,ax
		;call print
		
		;Vamos a cargar la fat
		mov ax,1;empieza en 1
		mov cl,9;ocupa 9
		;bx = Destino desde offset 7C0h (igual que ds)
		mov bx,6000h
		call leerNSector

		;Tenemos en ax la dir de la rootentry
		pop ax

		add ax,1Ah
		mov di,ax
		mov ax,[di]

		add ax,ax

		add ax,6000h
		mov di,ax
		mov ax,[di]

		push ax
		mov dl,ah
		call printNumber
		pop ax
		push ax
		mov dl,al
		call printNumber
		pop ax
		




		;getRootEntry nos devuelve el offset a esa root entry:
		;	11primeros bytes nombre extension
		;	offset 0x1A: inicio del fichero en los clusters
		;	offset 0x1C: tamaño del fichero

		;pa acceder a el cluster de la fat, hacemos [ax+0x1A]*2+dir_inicio_fat (512)
		

		;Leemos el sector del fichero
		;sub ax,2
		;add ax,4Eh ;sector de inicio de la data region calculado a pelo
		;;en LBA está en ax la direccion
;
		;;cl = Numero de sectores a cargar
		;mov cl,1
		;;ax = Numero de sector inicial (LBA)
;
		;;bx = Destino desde offset 7C0h (igual que ds)
		;mov bx,1000h
		;call leerNSector
		;TODO: ver si hasta aqui ha cargado bien el tema
		
;		mov dx,1000h
;	.iteracion:
;		cmp dx,1200h ;vamos avanzando hasta q nos salimos del rootdir
;		je .salir
;				mov cx,32
;				.bucle:
;				push cx
;				mov di,dx
;				mov byte al,[di]
;				cmp al,65
;				jge .nosum
;				add al,'0'
;			.nosum:	
;				mov ah, 0xe
;				mov bx, 9
;				int 10h
;				add dx,1
;				pop cx
;				sub cx,1
;				cmp cx,0
;				jne .bucle
;				mov ah, 0xe
;				mov bx, 9
;				mov al , 10
;				int 10h
;				mov ah, 0xe
;				mov bx, 9
;				mov al , 0xd
;				int 10h
;				xor ax,ax
;				int 16h
;				jmp .iteracion
;		.salir:

































		;pop ax



		

		;add ax,200h ;por ahora no tenemos secores reservados ni cosas en medio
		;ax==dir del cluster
		call reboot
		
		
		;3-Saltar al siguiente cluster o, si hemos acabado, parar.

		;4-Es hora de hacer el salto a donde hayamos cargado. Que dios nos pille confesados.

;dl numero a imprimir
printNumber:
	xor dh,dh
	mov ax,dx
	mov cl,100
	div cl
	add al,'0'
	mov       ah, 0xe
	mov       bx, 9
	int       0x10
	mov ax,dx
	mov cl,10
	div cl
	xor ah,ah
	div cl
	add ah,'0'
	mov al,ah
	mov       ah, 0xe
	int       0x10
	mov ax,dx
	mov cl,10
	div cl
	add ah,'0'
	mov al,ah
	mov       ah, 0xe
	int       0x10
	mov al,' '
	int 0x10
	retw

















;Convertir de LBA a CHS
;params: ax -> coord LBA
;returns: ax: H bx: LBA cx: C dx: S
lbaTOchs:
	push ax
	;1-> consultamos las C,H,S maximas
	;mov ah,08h
	;mov dl,80h
	;int 13h
	;jnc .novalorespordefecto

	mov cx,1111111111111111b
	mov dh,11111111b
		;Cilindros max = ch (10 bits) -> cilindros
		;Sector max = cl (6 bits) -> sectores/pista
		;Head max = dh (8 bits) -> pistas/cilindro
.novalorespordefecto:
	and cl,00111111b
	mov al,dh
	mul cl 
	;ax = 16065
	

	mov bx,ax;bx=(dh * (cl -> 6 bits))
	xor dx,dx
	;mov ax,[ss:sp];recargamos ax (lba)
	pop ax
	push ax

	div bx
	;ax = bien (0)
	;dx = bien (1)

	push ax;Coordenada C
	mov ax,dx;movemos el resto a ax
	xor dx,dx

	;ax vale 1
	;entre bx que vale 63
	xor bx,bx
	mov bl,cl

	div bx
	;ax = H

	inc dx
	;dx = S
	pop cx
	pop bx

	retw
		;C = lba / (dh * (cl -> 6 bits))
		;resto = lba mod (dh * (cl -> 6 bits))
		;H = resto / (cl -> 6 bits)
		;S = resto mod (cl -> 6 bits) + 1
.lamosliao:
	mov si,cero
	call print
	call reboot



;A contunuacion unas funciones ricas que ya conocemos de hacer antes:
;Imprime con el mensaje ya puesto con un lea en el registro %si
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

reboot:
	mov si, msgReiniciar
	call print
	xor ax,ax
	int 0x16 ; Espera a que se pulse una tecla

	;a continuacion codigo maquina muy turbio que reinicia,
	;en esencia deberia ser un jmp FFFF:0000
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
		push cx
		push bx

		call lbaTOchs;ax: H 
					 ;bx: LBA 
					 ;cx: C 
					 ;dx: S


		mov dh,al;preparamos la cabeza en su sitio
		shl cx,6;cilindros gestionados
		or cx,dx;sectores gestionados

		pop bx; esto es el offset
		pop ax; esto va con el push cx que pone bien los sectores a cargar


		push bx
        mov ah, 02h  ;mas documentacion en 2-CargaSector2.asm        
    	mov dl, 80h;[iBootDrive]     
    	mov bx, 7C0h  ;si ponemos un 0 aqui, la liamos muy fuertemente
    	mov es, bx    
    	pop bx
.lectura:
    	int 0x13       							; interupcion del DISCO de la BIOS
		jc .fallolectura							; si falla, no reintenter
		retw
.fallolectura:
		mov si,msgDiskErr
		call print
		call reboot






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
		;						mov cx,32
		;						.bucle:
		;						push cx
		;				
		;						mov di,dx
		;						mov byte al,[di]
		;				
		;						cmp al,65
		;						jge .nosum
		;						add al,'0'
		;					.nosum:	
;
;
		;						mov ah, 0xe
		;						mov bx, 9
		;						int 10h
		;						;xor ax,ax
		;						;int 16h
		;				
		;						add dx,1
		;						pop cx
		;						sub cx,1
		;						cmp cx,0
		;						jne .bucle
		;				
		;						mov ah, 0xe
		;						mov bx, 9
		;						mov al , 10
		;						int 10h
		;						mov ah, 0xe
		;						mov bx, 9
		;						mov al , 0xd
		;						int 10h
		;						xor ax,ax
		;						int 16h
		;				
		;						jmp .iteracion


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


;DATOS DEL PROGRAMA:
msgInicial:    db 'Hola',13,10,0
msgCarga:      db 'KERNEL encontrado',13,10,0
msgNotFound:   db 'No kernel',13,10,0
msgDiskErr:    db 'Error de disco',13,10,0
msgReiniciar:  db 'Reinicia!',13,10,0
kernelFile:    db 'KERNEL  BIN',0
cero: db '0...',13,10,0
;dirEntry: 
;	.name:	  db '00000000'
;	.ext:     db '000'
;	.basura:  db '000000000000000'
;	.cluster: db '00'
;	.tam:     db '0000'

; Magic numbers
times 510 - ($ - $$) db 0                       ; Rellenamos con 0.
dw 0xAA55                                       ; Ponemos el numero magico