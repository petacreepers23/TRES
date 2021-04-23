bits 16     ; Modo de 16 Bits
;org 7C00h   ; El codigo se ejecuta aqui

; Este codigo lee el sector LBA 1 del disco. sector 0(boot) sector1(el que vamos a leer)
; es el mismo ejercicio que en el 3 pero usando LBA y por tanto teniendo que pasar de LBA a CHS
; CODIGO FUNCIONAL lo que es curioso porque no tiene el tamaño neceserio.

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


start:
		;Empezamos ajustando registros
	mov ax, 7C0h  ;Creamos DS ;a la peña le gusta poner cx en vez de 0, npi de xq ; Data segment register (es un offset pa las direcciones)
	mov ds, ax								; limitaciones en la cpu, lo de ax es un tramite...
; llamamos a LBA.
;;cl = Numero de sectores a cargar
;;ax = Numero de sector inicial (LBA)
;;bx = Destino desde offset 7C0h (igual que ds)


        xor ax,ax ; ax=0
		xor cx,cx ; cx=0
		xor bx,bx ; bx=0
		mov bx,0x81F0;1000;8000h;1000h;8E00hnofunciona; aqui la dir maxima sera 7c00:ffff
		mov cx,1;
        mov al, 1 ; LBA=1 ax=[ah|al]
		call leerNSector
        ; call LBAtoCHS ; devuelve : cl -> S . ch -> C dh -> H



;.finPrint:
;        mov ah, 02h    ; Preparamos la int13, que lee, con 02, que dice "leer"
;        mov al, 1      ; Queremos leer 1 secor 
;        ;mov ch, 0      ; Cilindro 0
;    	;mov cl, 2      ; Sector 2
;    	;mov dh, 0      ; Cabezal 0
;    	mov dl, 80h      ; Floppy/Disco
;    	mov bx, 0h   ; 
;    	mov es, bx     ; dest (segment->implica las direcciones divididas por 16) (a 0)
;    	mov bx, 8E00h  ; Offset (se suma a segment, y son direcciones normales)
;.lectura:
;    	int 0x13       	; interupcion del DISCO de la BIOS
;		jc .lectura		; si falla, reintenter

	;Imprimimos mierda
mov si, msgInicial
mov ah, 0Eh
mov bh, 00h
mov bl, 07h
.loop:
        lodsb    
        cmp al, 0
        je .serias  
        int 10h  
        jmp .loop

.serias:
    	jmp 0xFFFF;10000;8000h;1000h;8E00h ;8C00h;7E00h   							; saltamos a lo q hemos cargado




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
        retw
; ahora: cl -> S . ch -> C dh -> H		
;----------------------------------------------------------------------------------------

;------------------------------------------------------------------------------------------------
;
;;Leemos N sectores con N = tamaño de cluster
;;parametros:
;;cl = Numero de sectores a cargar
;;ax = Numero de sector inicial (LBA)
;;bx = Destino desde offset 7C0h (igual que ds)
leerNSector:
		;Convertimos de lba a chs:
		push bx
		push cx

		; mov si, cx
        ; call print 

		call LBAtoCHS ; param ax: LBA return: cl: S  ch: C dh: H	
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
;---------------------------------------------------------------------------------------------


;-----------+ funciones
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
	call reboot
;--------
msgInicial:    
        db 'Hola -> sector 1',13,10,0
msgProIni: db 'hola mundo',0

debugmsg:      db 'Aqui toy ', 13,10,0
msgNoA20:	   db 'Error A20',13,10,0
msgNotFound:   db 'No kernel',13,10,0
msgDiskErr:    db 'Error de disco',13,10,0
msgReiniciar:  db 'Reinicia!',13,10,0
; Magic numbers
times 510 - ($ - $$) db 0                       ; Rellenamos con 0.
dw 0xAA55                                                       ; Ponemos el numero magico



;7C00+200+el programa en si.



;___SECTOR 2___
        mov ax,0xFFF;0xFA0;8A0h;1080h;7C0h                    ;tenemos que compensar, que este codigo, arriba, lo hemos cargado en la 0x8C00
        mov ds, ax                      ;pero, el compilador, nos lo pone en la 0x200 (512d), por lo que, hay que
                                        ;restarle 512 a la 8C00h, asi que nos queda 8A00h, que es el segment (dividido x 16) 8A0h


        mov si, msg                     ; Hacemos que si apunte al mensaje de mierda
        mov ah, 0Eh                     ; Le pasamos a la bios el parametro 0Eh (imprimir) de la INT10h en ah
        mov bh, 00h 					; Reponemos los parametros, fondo negro
        mov bl, 07h 					; Idem, texto blanco
        mov al,65
        int 0x10
.loop:
        lodsb                           ; Metemos lo de SI en al (param de char a imprimir) e incrementa SI, apuntando asi al siguiente char
        cmp al, 0                       ; Comprobamos si encontramos un '\0' osea, un 0, fin del string
        je halt                         ; Si es asi, halteamos el CPU
        int 10h                         ; Sino llamamos a la interupcion de print
        jmp .loop                       ; Next iteration of the loop

halt:   
        jmp $

msg:    
        db '... Hola desde el sector 2.',13,10,0

times 1024 - ($ - $$) db 0                       ; Rellenamos con 0 el resto del sector 2
;https://wiki.osdev.org/Real_mode_assembly_I



