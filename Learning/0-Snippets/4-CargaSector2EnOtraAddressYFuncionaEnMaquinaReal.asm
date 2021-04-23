bits 16                         ; Modo de 16 Bits
;El org nos lo quitamos, pa usar el Data segment, como apoyo.
start:
		;Empezamos ajustando registros
	mov ax, 7C0h ;a la peña le gusta poner cx en vez de 0, npi de xq ; Data segment register (es un offset pa las direcciones)
	mov ds, ax								; limitaciones en la cpu, lo de ax es un tramite...

        mov ah, 02h                             
        mov al, 1                                
        mov ch, 0                               
    	mov cl, 2                               
    	mov dh, 0                               
    	mov dl, 80h                             ; JOOOOOOOOOOOOOOOOOOOOOOOOOODER  
    	mov bx, 7C0h                           	; ESTO YA NO DEBE ESTAR A 0, INT13, necesita saber el segment que tenemos
    	mov es, bx                              ; dest (segment->implica las direcciones divididas por 16)
    	mov bx, 1000h                           ; Offset de 1000 + 7C0 del registro "es"
.lectura:
    	int 0x13       							; interupcion del DISCO de la BIOS
	jc .lectura								; si falla, reintenter

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
    	jmp 1000h   							; saltamos a lo q hemos cargado (0x8C00) (recuerda la compensacion de"ds", puesta al principio)


msgInicial:    
        db 'Hola -> sector 1',13,10,0
; Magic numbers
times 510 - ($ - $$) db 0                       ; Rellenamos con 0.
dw 0xAA55                                                       ; Ponemos el numero magico




;___SECTOR 2___
        mov ax, 8A0h                    ;tenemos que compensar, que este codigo, arriba, lo hemos cargado en la 0x8C00
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