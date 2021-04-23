bits 16                         ; Modo de 16 Bits

start:
		; Empezamos ajustando registros
		mov ax, 7C0h ; Segmento de carga de datos que lo ponemos como offset
		mov ds, ax	 ; Limitaciones de arquitectura
 
        mov ah, 02h  ; Nos preparamos pa leer                           
        mov al, 1    ; 1 sector                            
        mov ch, 0    ; head 0                  
    	mov cl, 2    ; empezando por el segundo                           
    	mov dh, 0    ; cabeza 0                           
    	mov dl, 0    ; Floppy
.reintentar:
    	mov bx, 7C0h ; Le decimos el segment a int13
    	mov es, bx   ; Limitaciones de arquitectura
    	mov bx, 1000h; Offset de 1000h(Por ejemplo) + 7C0 del registro "es" (Queremos cargarlo en la 1000h)
    	int 0x13     ; interupcion del DISCO de la BIOS
		jc .disco	 ; si falla, probar con floppy

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


.disco:
    ;lo mismo q arriba pero dl = 80h (disco)
    mov ah, 02h  
    mov al, 1    
    mov ch, 0    
    mov cl, 2    
    mov dh, 0    
    mov dl, 80h ;DISCO  
    jmp .reintentar  

.salto:
    	jmp 1000h   	; saltamos a lo q hemos cargado (0x8C00) (recuerda la compensacion de"ds", puesta al principio)

msgInicial:    
        db 'Hola mundo!',13,10,0
; Magic numbers
times 510 - ($ - $$) db 0  ; Rellenamos con 0.
dw 0xAA55                  ; Ponemos el numero magico




;___SECTOR 2___
sec2:
        mov ax, 8A0h                    ;tenemos que compensar, que este codigo, arriba, lo hemos cargado en la 0x8C00
        mov ds, ax                      ;pero, el compilador, nos lo pone en la 0x200 (512d), por lo que, hay que
                                        ;restarle 512 a la 8C00h, asi que nos queda 8A00h, que es el segment (dividido x 16) 8A0h

;AQUI NO HAY GRAN COSA, SOLO UN PRINT

        mov si, msg 
        mov ah, 0Eh 
        mov bh, 00h 
        mov bl, 07h 
        mov al,65
        int 0x10
.loop:
        lodsb       
        cmp al, 0   
        je halt     
        int 10h     
        jmp .loop   

halt:   
        jmp $

msg:    
        db '... Hola desde el sector 2.',13,10,0

times 1024 - ($ - $$) db 0                       ; Rellenamos con 0 el resto del sector 2