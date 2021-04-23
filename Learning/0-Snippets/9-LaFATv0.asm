;Vamos a meter la FAT que ya era hora ¿no?
;Hay varias FT disponibles:
;FAT12,FAT16 y FAT32
;Como la VM trabaja con floppys, vamos a meter FAT16 pa nuestro SO, una cosa intermedia.
;El formato es el siguiente:
;(Cosa)				   (Tamanio en sectores)
;Boot sector					1
;File allocation table (FAT)	18
;Root directory					14
;Data area						1407
;Total 1440 sectores de 0x200 bytes (512)
;
;La estructura del sector de boot ya la tenemos puesta
;en el codigo 8
;
;El directorio raiz de la fat, contiene 224 entradas de 32 bits,
;con la siguiente forma:
;0x00	8	File name
;0x08	3	File extension
;0x0b	1	Attribute
;0x0c	1	Reserved
;0x0d	1	Creation timestamp
;0x0e	2	Creation time
;0x10	2	Creation date
;0x12	2	Last access date
;0x14	2	Reserved
;0x16	2	Last modified time
;0x18	2	Last modified date
;0x1a	2	Cluster
;0x1c	4	File size (bytes)
;
;Son 224 porque asi lo hemos configurado en el sector de boot (y es un valor tipico)




bits 16; Modo de 16 Bits
%define LOAD_SEGMENT 0x1000 ;Sitio donde cargar el resto de mierdas, visto en  codigos (3-4-5)
main:
		jmp start
		nop 	 	
bootinfo:

  iOEM:          db "TRES    "   
  iSectSize:     dw  0x200       
  iClustSize:    db  1           
  iResSect:      dw  1           
  iFatCnt:       db  2           
  iRootSize:     dw  224         
  iTotalSect:    dw  2880        
  iMedia:        db  0xF0        
  iFatSize:      dw  9           
  iTrackSect:    dw  9           
  iHeadCnt:      dw  2           
  iHiddenSect:   dw  0           
  iSect32:       dw  0           
  iBootDrive:    db  0           
  iReserved:     db  0           
  iBootSign:     db  0x29        
  iVolID:        db "seri"       
  acVolumeLabel: db "MYVOLUME   "
  acFSType:      db "FAT16   "   

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

;Funcion de por ahi, porque esto ya lo hemos visto
; buffer at ES:BX. This function uses interrupt 13h, subfunction ah=2.
readsector:
  xor     cx, cx                      ; Set try count = 0
 
 .readsect:
  push    ax                          ; Store logical block
  push    cx                          ; Store try number
  push    bx                          ; Store data buffer offset
 
  ; Calculate cylinder, head and sector:
  ; Cylinder = (LBA / SectorsPerTrack) / NumHeads
  ; Sector   = (LBA mod SectorsPerTrack) + 1
  ; Head     = (LBA / SectorsPerTrack) mod NumHeads
 
  mov     bx, iTrackSect              ; Get sectors per track
  xor     dx, dx
  div     bx                          ; Divide (dx:ax/bx to ax,dx)
                                      ; Quotient (ax) =  LBA / SectorsPerTrack
                                      ; Remainder (dx) = LBA mod SectorsPerTrack
  inc     dx                          ; Add 1 to remainder, since sector
  mov     cl, dl                      ; Store result in cl for int 13h call.

  mov     bx, iHeadCnt                ; Get number of heads
  xor     dx, dx
  div     bx                          ; Divide (dx:ax/bx to ax,dx)
                                      ; Quotient (ax) = Cylinder
                                      ; Remainder (dx) = head
  mov     ch, al                      ; ch = cylinder                      
  xchg    dl, dh                      ; dh = head number
  ; Call interrupt 0x13, subfunction 2 to actually
  ; read the sector.
  ; al = number of sectors
  ; ah = subfunction 2
  ; cx = sector number
  ; dh = head number
  ; dl = drive number
  ; es:bx = data buffer
  ; If it fails, the carry flag will be set.
  mov     ax, 0x0201                  ; Subfunction 2, read 1 sector
  mov     dl, iBootDrive              ; from this drive
  pop     bx                          ; Restore data buffer offset.
  int     0x13
  jc      .readfail
  ; On success, return to caller.
  pop     cx                          ; Discard try number
  pop     ax                          ; Get logical block from stack
  ret

  ; The read has failed.
  ; We will retry four times total, then jump to boot failure.
  .readfail:   
  pop     cx                          ; Get try number             
  inc     cx                          ; Next try
  cmp     cx, 4              ; Stop at 4 tries
  je      falloDisco
  ; Reset the disk system:
  xor     ax, ax
  int     0x13
  ; Get logical block from stack and retry.
  pop     ax
  jmp     .readsect

reboot:
	mov si, msgReiniciar
	call print
	xor ax,ax
	int 0x16
	db 0xEA
	dw 0x0000
	dw 0xFFFF

falloDisco:
	mov si, msgDiskErr
	call print
	call reboot



;Aqui comienza nuestro tipico codigo
start:
		cli
		mov [iBootDrive], dl
		mov ax, 7C0h  ;Creamos DS
		mov ds, ax
		xor ax,ax     ;Creamos pila
		mov ss,ax
		mov sp,7BFFh
		sti
		;Saludamos
		mov si, msgInicial
		call print
		;init disco
		mov  dl, iBootDrive   ; disco a resetear
  		xor  ax, ax           ; subfunction 0
  		int  0x13             ; int 13h
  		jc   falloDisco       ; si error, nos vamos a la puta
  		;Vamos a cargar mierdas del disco, como en los codigos 3-4-5
  		;Ahora como tenoemos una fat, vamos a calcular
  		;primero el temaño del directorio raiz de la fat
		;el numero de sectores =num entradas * tamaño entrada / tamaño sector
		;Lo calculamos y guardamos en cx.
		mov     ax, 32
		xor     dx, dx
		mul     word [iRootSize]
		div     word [iSectSize]        ; Divide (dx:ax,sectsize) to (ax,dx)
		mov     cx, ax
		mov     [root_scts], cx
		; root_scts is now the number of sectors in the root directory.
  		; http://www.independent-software.com/operating-system-development-file-allocation-table-and-reading-from-disk.html
  		jmp $

;DATOS DEL PROGRAMA:
msgInicial:    db 'Hola mundo!',13,10,0
msgCarga:      db 'Cargando TRES...',13,10,0
msgDiskErr:    db 'Error irrecuperable de disco.',13,10,0
msgReiniciar:  db 'Presiona una tecla para reiniciar.',13,10,0
root_scts:   db 0,0
; Magic numbers
times 510 - ($ - $$) db 0                       ; Rellenamos con 0.
dw 0xAA55                                       ; Ponemos el numero magico