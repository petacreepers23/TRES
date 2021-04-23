org 0x0
bits 16 
; Estamos al comienzo del ds
;Codigo del kernel, que para usar en AX hay que dejar la dirección en la que está ANTES
;de saltar.
%DEFINE dirBASE 0x8200
jmp main
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

main:
		cli
		;mov byte [iBootDrive], 80h ;NUEVA LINEA -> Debería ser 0 (Disco del q booteamos, por eso no lo poniamos antes pero puede dar problemas si no lo ponemos, esta linea guarda el disco del que booteamos a nuestra estructura de datos)
		mov ax, 820h  ;Creamos DS
        ;xor ax,ax
		mov ds, ax
		mov     es, ax
		mov     fs, ax
		mov     gs, ax
		mov si, msg
		call print


		xor ax,ax     ;Creamos pila
		mov ss,ax
		mov sp,7BFFh
		sti
		call enableA20
        sti
        mov si, msgA20
        call print
        ; set vga to be normal mode
        mov ax, 0x3
        int 0x10
        ;Hasta aqui sabemos q llega bn






;Igual aqui se hacen mas cosas xd







;










		cli
        
        xor ax, ax
        mov ds, ax
		mov es, ax
    	mov fs, ax
    	mov gs, ax

        jmp 0:aux+dirBASE ; Arreglar CS , valía 7c0 y ahora 0.
aux:
		lgdt [GDT_END+dirBASE];828h;[GDT_END+dirBASE]

        ;sti
        ;mov si, msgGDT

        ;int 16h
        ;cli

		mov eax, cr0 ;Indicar q movemos a modo protegido, no podemos operar directamente sobre cr0 asi q lo movemos
		or eax,0x1;El bit 1 indica lo de arriba
		mov cr0,eax;modificamos el registro de estado.
        ;jmp 08h:(boot2 + 0x7C00) 
        ;jmp 190h; luego probar con 400d
        jmp  08h:clear_pipe+dirBASE;; Limpia la cola de prefech y vuelve a corregir el cs
		[bits 32]
		;[bits 16] 
		;jmp $
		;jmp algo; = pc = cs:algo modo real: cs*16+algo modo protegido: 32bits reg=[cs|algo]


;-----------------------------------------------------------------

	;E PROTECTED MODE
	;bits 32
	clear_pipe:  
    mov ax, 10h
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov esp,9ff0h
	;JUMP TO EXECUTE KERNEL!
	jmp 08h:0x10000; Para que nasm sepa que es un far jmp. 

    ; [bits 32]
    ; mov ax, 10h
    ; mov ds, ax
    ; mov es, ax
    ; mov fs, ax
    ; mov gs, ax
    ; mov ss, ax
    ; sti

    ; mov byte [0x0B8000], 'H'
    ; mov byte [0x0B8001], 1Bh
    ; mov byte [0x0B8002], 'O'
    ; mov byte [0x0B8003], 1Bh
    ; mov byte [0x0B8004], 'L'
    ; mov byte [0x0B8005], 1Bh
    ; mov byte [0x0B8006], 'A'
    ; mov byte [0x0B8007], 1Bh
    ; mov byte [0x0B8008], 'U'
    ; mov byte [0x0B8009], 1Bh
    ; in eax, 0x64

    ; cli
	[bits 16]
    ; hlt
;----------------------------------------------------









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
; falloDisco:
; 	mov si, msgDiskErr
; 	call print
; 	call reboot
;
;Si fallo no encontrado
; falloNoEncontrado:
; 	mov si,msgNotFound
; 	call print
; 	;call reboot


 




; Function: check_a20
;
; Purpose: to check the status of the a20 line in a completely self-contained state-preserving way.
;          The function can be modified as necessary by removing push's at the beginning and their
;          respective pop's at the end if complete self-containment is not required.
;
; Returns: 0 in ax if the a20 line is disabled (memory wraps around)
;          1 in ax if the a20 line is enabled (memory does not wrap around)
check_a20:
    pushf
    push ds
    push es
    push di
    push si
 
    cli
 
    xor ax, ax ; ax = 0
    mov es, ax
 
    not ax ; ax = 0xFFFF
    mov ds, ax
 
    mov di, 0x0500
    mov si, 0x0510
 
    mov al, byte [es:di]
    push ax
 
    mov al, byte [ds:si]
    push ax
 
    mov byte [es:di], 0x00
    mov byte [ds:si], 0xFF
 
    cmp byte [es:di], 0xFF
 
    pop ax
    mov byte [ds:si], al
 
    pop ax
    mov byte [es:di], al
 
    mov ax, 0
    je check_a20__exit
 
    mov ax, 1
 
check_a20__exit:
    pop si
    pop di
    pop es
    pop ds
    popf
 
    ret

enableA20:
	xor ax, ax;ffffffffa escribir en el puerto.
	;check
	xor ax, ax
	call check_a20
	cmp ax,1;
	je enaA20exit;
	mov si, msgNoA20;
	call print;
	;call reboot;
enaA20exit:	
	ret







;DATOS DEL PROGRAMA: 
msgA20: db 'A20 OK',13,10,0
msgGDT: db 'GDT OK',13,10,0


msgNoA20:	   db 'Error A20',13,10,0
msgNotFound:   db 'No kernel',13,10,0
msgDiskErr:    db 'Error de disco',13,10,0
;msgReiniciar:  db 'Reinicia!',13,10,0
kernelFile:    db 'KERNEL  BIN',0
times 512 - ($ - $$) db 0  



;GDT
GDT_START: 
;null descriptor 
dq 0  ;reservado por intel 

gdt_code_32:
dw 0xFFFF 
dw 0 
db 0 
db 10011010b 
db 11001111b 
db 0 

gdt_data_32:
dw 0xFFFF 
dw 0 
db 0 
db 10010010b 
db 11001111b 
db 0 

gdt_code_16:
dw 0xFFFF 
dw 0 
db 0 
db 10011010b 
db 0 
db 0 

gdt_data_16:
dw 0xFFFF 
dw 0 
db 0 
db 10010010b 
db 0
db 0 
GDT_END: 
;GDT_32: 
dw GDT_END - GDT_START - 1 ;27h
dd GDT_START + dirBASE; - 0x7c00 ;=0x8400
; Hay que restar 1 porque el max valor del tam de la gdt 65536 Bytes y el del reg GDtr 65535
CODE_SEG equ gdt_code_32 - GDT_START ; x - (x+8) = 8
DATA_SEG equ gdt_data_32 - GDT_START 





msg:    db 'Hola desde el sector 2.',13,10,'Puedes empezar a escribir.',13,10,0
msgReiniciar:  db 'Presiona una tecla para reiniciar.',13,10,0
times 1024 - ($ - $$) db 0                       ; Rellenamos con 0 el resto del sector 2