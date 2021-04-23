bits 16                         ; Modo de 16 Bits

start:
	mov ax, 07C0h		    	; Metemos la direccion de donde la bios carga el codigo dividida x16
	mov ds, ax					; La metemos en el ds para que el CPU en modo real la multiplique por 16 al calcular direcciones de memoria

	mov si, msg             	; Hacemos que su apunte al mensaje de mierda
	mov ah, 0Eh            		; Le pasamos a la bios el parametro 0Eh (imprimir) de la INT10h en ah
.loop:
	lodsb                   	; Metemos lo de SI en al (param de char a imprimir) e incrementa SI, apuntando asi al siguiente char
	cmp al, 0               	; Comprobamos si encontramos un '\0' osea, un 0, fin del string
	je halt                 	; Si es asi, halteamos el CPU
	int 10h                		; Sino llamamos a la interupcion de print
	jmp .loop               	; Next iteration of the loop

halt:	
	jmp $
msg:	
	db 'Mensaje de mierda.', 0

; Magic numbers
times 510 - ($ - $$) db 0			; Rellenamos con 0
dw 0xAA55							; Ponemos el numero magico