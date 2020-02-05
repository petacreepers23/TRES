bits 16                         ; Modo de 16 Bits

start:
	; Empezamos ajustando registros
	mov ax, 7C0h ; Segmento de carga de datos que lo ponemos como offset
	mov ds, ax	 ; Limitaciones de arquitectura

    mov ah, 02h  ; Nos preparamos pa leer                           
    mov al, 1    ; A partir del sector 65, da error                            
    mov ch, 0    ; head 0                  
	mov cl, 2    ; empezando por el segundo                           
	mov dh, 0    ; cabeza 0                           
	mov dl, 0    ; Floppy
	mov bx, 7C0h ; Le decimos el segment a int13
	mov es, bx   ; Limitaciones de arquitectura
	mov bx, 0200h; 
	int 0x13     ; interupcion del DISCO de la BIOS
	jc $

    ;tipica rutina de impresión
    mov si, msgInicial    
    mov ah, 0Eh           
    mov bh, 00h           
    mov bl, 07h           
    .loop:                
            lodsb         
            cmp al, 0     
            je .salto     
            int 10h       
            jmp .loop     
.salto:
    jmp 200h

msgInicial:    
    db 'Hola mundo!',13,10,0

    times 510 - ($ - $$) db 0  ; Rellenamos con 0.
    dw 0xAA55                  ; Ponemos el numero magico




;___SECTOR 2___
sec2:                   
                                    
    ;lol
    
;CONFIG
 	mov ah, 00h     ; tell the bios we'll be in graphics mode
 	mov al, 13h		; 320x200 256 color graphics (MCGA,VGA)
 	int 10h         ; call the BIOS

 	mov si, done
 	mov ah, 0Ch     ; set video mode
 	mov bh, 0       ; set output vga
 	mov al, 0       ; set initial color
redo:
 	mov cx, 0       ; x = 0
 	mov dx, 0       ; y = 0

draw:
	int 10h
	; cx register holds x coord and dx holds y coord
	cmp dx, 200		; if y is at 200, jump to done (200 is the max height)
	je done
	cmp cx, 320		; if x is at 320, jump to new_xy
	je new_xy
	inc cx			; else increment cx by 1
	lodsb 			; actualiza color

	jmp draw		; call the BIOS to draw it out

; here is where we increment y coord and reset x coord
new_xy:
	mov cx, 0		; reset x coord
	inc dx			; increment y coord
	jmp draw

done:
	jmp redo			; jump to the current line until the end of time
;times 16384 - ($ - $$) db 0                       ; Rellenamos con 0 el resto del sector 2