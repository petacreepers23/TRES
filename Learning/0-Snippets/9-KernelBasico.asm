bits 16 
db 'texto conocido'                        ; Modo de 16 Bits
org 1000 ;debe cargarse en esta dir                           ; Estamos al comienzo del ds
;Codigo del kernel, que para usar en AX hay que dejar la dirección en la que está ANTES
;de saltar.

;ponemos ese dato en ds, ahora que ya hemos saltado.
mov ds, ax								


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
        db 'Bienvenido a TRES.',13,10,'Hola desde el kernel que esta en la fat y en el sector 1.',13,10,0



times 512 - ($ - $$) db 0                       ; Rellenamos con 0.
;Sector 2:

        mov si, msg                     ; Hacemos que si apunte al mensaje de mierda
        mov ah, 0Eh                     ; Le pasamos a la bios el parametro 0Eh (imprimir) de la INT10h en ah
        mov bh, 00h 					; Reponemos los parametros, fondo negro
        mov bl, 07h 					; Idem, texto blanco
        ;tipica rutina de imprimir a continuacion
.loop:
        lodsb                           ; Metemos lo de SI en al (param de char a imprimir) e incrementa SI, apuntando asi al siguiente char
        cmp al, 0                       ; Comprobamos si encontramos un '\0' osea, un 0, fin del string
        je read                         ; Si es asi, halteamos el CPU
        int 10h                         ; Sino llamamos a la interupcion de print
        jmp .loop                       ; Next iteration of the loop

read:   
;Bucle de lectura de caracteres
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
    jmp read

.saltoLinea:
    mov ah,0Eh
    mov bh, 00h
    mov bl, 07h
    mov al,13
    int 10h
    mov ah,0Eh
    mov al,10
    int 10h
    jmp read

msg:    
        db 'Hola desde el sector 2.',13,10,'Puedes empezar a escribir.',13,10,0

times 1024 - ($ - $$) db 0                       ; Rellenamos con 0 el resto del sector 2