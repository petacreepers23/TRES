bits 16                         ; Modo de 16 Bits
org 7C00h						; El codigo se ejecuta aqui
; CODIGO FUNCIONAL
start:
		;Empezamos ajustando registros
	mov ax, 0000h ;a la peña le gusta poner cx en vez de 0, npi de xq ; Data segment register (es un offset pa las direcciones)
	mov ds, ax								; limitaciones en la cpu, lo de ax es un tramite...

;        mov si, msgProIni
;        mov ah, 0Eh
;.print:
;        lodsb
;        or al,al 
;        jz .finPrint
;        mov bh,0
;        int 10h
;        jmp .print
.finPrint:
        mov ah, 02h    ; Preparamos la int13, que lee, con 02, que dice "leer"
        mov al, 1      ; Queremos leer 1 secor 
        mov ch, 0      ; Cilindro 0
    	mov cl, 2      ; Sector 2
    	mov dh, 0      ; Cabezal 0
    	mov dl, 80h      ; Floppy/Disco
    	mov bx, 0h   ; 
    	mov es, bx     ; dest (segment->implica las direcciones divididas por 16) (a 0)
    	mov bx, 8E00h  ; Offset (se suma a segment, y son direcciones normales)
.lectura:
    	int 0x13       	; interupcion del DISCO de la BIOS
		jc .lectura		; si falla, reintenter

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
    	jmp 8E00h ;8C00h;7E00h   							; saltamos a lo q hemos cargado


msgInicial:    
        db 'Hola -> sector 1',13,10,0
msgProIni: db 'hola mundo',0
; Magic numbers
times 510 - ($ - $$) db 0                       ; Rellenamos con 0.
dw 0xAA55                                                       ; Ponemos el numero magico



;7C00+200+el programa en si.



;___SECTOR 2___
        mov ax, 100h;7C0h                    ;tenemos que compensar, que este codigo, arriba, lo hemos cargado en la 0x8C00
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
        db '... Hola desde el sevvtor 2.',13,10,0

times 1024 - ($ - $$) db 0                       ; Rellenamos con 0 el resto del sector 2
;https://wiki.osdev.org/Real_mode_assembly_I