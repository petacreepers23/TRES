bits 16                         ; Modo de 16 Bits
;El org nos lo quitamos, pa usar el Data segment, como apoyo.
start:
		mov ax, 7C0h ;Creamos DS
		mov ds, ax

		xor ax,ax     ;Creamos pila
		mov ss,ax
		mov sp,7BFFh

        mov ah, 02h   ;Cargamos siguiente sector                            
        mov al, 1                                
        mov ch, 0                               
    	mov cl, 2                               
    	mov dh, 0                               
    	mov dl, 0h         ; adasda  
    	mov bx, 7C0h                           	
    	mov es, bx                              
    	mov bx, 200h                           
.lectura:
    	int 0x13       							
		jc .lectura								

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
    	jmp 200h   							


msgInicial:    
        db 'Hola -> sector 1',13,10,0
; Magic numbers
times 510 - ($ - $$) db 0                       ; Rellenamos con 0.
dw 0xAA55                                                       ; Ponemos el numero magico




;___SECTOR 2___
sec2:
    xor ax,ax;Vamos a hacer una int 13, asi q leemos
    int 16h;en ah tenemos la letra 
    mov ah,al;nos la deja en ah
    cmp al,13
    je .saltoLinea

    ;print
    mov ah,0Eh
    mov bh, 00h
	mov bl, 07h
	int 10h
	jmp sec2

.saltoLinea:
	mov ah,0Eh
    mov bh, 00h
	mov bl, 07h
	mov al,13
	int 10h
	mov ah,0Eh
	mov al,10
	int 10h
	jmp sec2


times 1024 - ($ - $$) db 0                       ; Rellenamos con 0 el resto del sector 2