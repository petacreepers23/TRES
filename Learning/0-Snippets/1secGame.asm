bits 16                         ; Modo de 16 Bits

	; Config basica
	mov ax, 7C0h ; Segmento de carga de datos que lo ponemos como offset en ds
	mov ds, ax

	xor ax, ax		;configuramos una pila
	mov ss, ax
	mov sp, 700h
	mov bp, 700h

	mov ax, 0A000h   ; direccion de la memoria de video
	mov es, ax 		

	xor ax, ax
	mov fs, ax 		; direccion de los cuerpos de las snake


 	mov ah, 00h     ; tell the bios we'll be in graphics mode
 	mov al, 13h		; 320x200 256 color graphics (MCGA,VGA)
 	int 10h         ; call the BIOS
 	jmp main

;--------------------------------------------------------------------------------------------------------------------------
; dados cx y dx centrados en un punto, y en al color, operamos con ellos (hacia más) y los devolvemos igual
;--------------------------------------------------------------------------------------------------------------------------
drawMatrix:
	push ax;ss:bx+4
	push cx;ss:bx+2
	push dx;ss:bx
	add cx, 20
	add dx, 20
.bucY:
	mov bx,sp
	mov ax,[ss:bx]		;sacamos original dx
	cmp dx,ax	;comparamos
	je .fin
.bucX:
	mov bx,sp
	mov ax,[ss:bx+2]		;sacamos original cx
	cmp cx,ax	;comparamos
	je .ajusteCx
.continua:
	;ñapa porque la pantalla empieza en 0
	dec dx
	dec cx
;aqui pintamos
	mov ax,320
	mov bx,dx; |
	mul dx   ;----> mul, que multiplica 16bitsx16bits, dela la parte alta del resultado en dx, asi que hay que tenerlo en cuenta
	mov dx,bx; |
	add ax,cx
	mov di,ax ;data index = desplazamiento calculado para el pixel
	mov bx,sp
	mov ax,[ss:bx+4] ;sacamos color
	mov [es:di],al
;finpintar
	;deshacemos ñapa
	inc dx
	inc cx
	
	dec cx
	jmp .bucX

.ajusteCx:
	add cx,20
	dec dx
	jmp .bucY
.fin:
	pop dx
	pop cx
	pop ax
	ret

;--------------------------------------------------------------------------------------------------------------------------
;en cx y dx el  punto
;--------------------------------------------------------------------------------------------------------------------------
drawFrutica:
	add dx, 2
	add cx, 2
	push cx ;ss:bx+2  -> punto inicial a pintar, desde ahi 16 mas.
	push dx ;ss:bx+0  -> punto inicial a pintar, desde ahi 16 mas.
	;TODO: calcular di
	mov ax,320
	mul dx;
	add ax,cx
	mov di,ax
	;done
	xor cx,cx
	mov cx,4
	mov si,frutica
.pintar1:
	mov bx,16 ;contador
.pintar:
	lodsb;Cargamos color
	add di,bx
	mov [es:di],al;pintamos al final de la linea
	dec di;
	mov [es:di],al;los dos pixeles
	add di,320
	mov [es:di],al;y debajo
	inc di
	mov [es:di],al;y debajo los 2 xd
	sub di,320;restauramos di
	sub di,bx ;di vuelve a apuntar a donde estaba

	mov [es:di],al;pintamos al principio de la linea
	inc di;
	mov [es:di],al;los 2 pixeles
	add di,320;
	mov [es:di],al;pintamos debajo
	dec di;
	mov [es:di],al;los 2 pixeles
	sub di,320
	;add di,2
	sub bx,2
	cmp bx,0
	jne .pintar
	cmp cx,0
	je .fin
	dec cx
	add di,640
	jmp .pintar1

.fin:
	pop dx
	pop cx
	ret

;--------------------------------------------------------------------------------------------------------------------------
; limpia la pantalla
;--------------------------------------------------------------------------------------------------------------------------
clearscr:
    xor eax, eax
    xor di, di
    mov cx, 0x3E80
    rep stosd
    ret


;--------------------------------------------------------------------------------------------------------------------------
;El main
;--------------------------------------------------------------------------------------------------------------------------
main:
	;pos inicial
	xor ax,ax
	push ax;X = 0 [bp-2]
	push ax;Y = 0 [bp-4]
	;score inicial
	push ax;sc=0 [bp-6]
	;direccion inicial; 0=d,1=w,2=a,3=s
	push ax;dr=0 [bp-8]
	;pos inicial frutica
	mov ax,100
	push ax; [bp-10]
	push ax; [bp-12]
	;la cola de la snake ya veremos donde la guardo xd


.bucle:
;1 proceso de logica
;espera segundos
	;xor ax,ax
	;int 0x1A
	;cmp dx,[tiempo]
	;je .bucle
	;mov [tiempo],dx

	;mov cx,[bp-10];x fruta
	;mov ax,[bp-2];x serpiente
	;cmp ax,ax
	;jne .pintar; no tienen la misma x



;2 se dibuja
.pintar:
	call clearscr

	mov cx,[bp-10];xf
	mov dx,[bp-12];yf
	call drawFrutica

	mov cx,[bp-2];x
	mov dx,[bp-4];y
	mov ax,0xE;el color ya lo gestionaremos mejor
	call drawMatrix

;3 input
.tecla:
	mov cx,[bp-2];x actuales
	mov dx,[bp-4];y actuales

	xor ax, ax
    int 16h;en ah tenemos la letra 
    mov ah, al;nos la deja en ah
    cmp al, 13 ;enter
    je .fin
    cmp al, 87; W
    je .arriba
    cmp al,119; w
    je .arriba
    cmp al, 83; S
    je .abajo
    cmp al,115; s
    je .abajo
    cmp al, 68; D
    je .derecha
    cmp al,100; d
    je .derecha
    cmp al, 65; A
    je .izquierda
    cmp al, 97; a
    je .izquierda
    jmp .tecla

	.arriba:
	cmp dx,0
	je .vueltaabajo
	sub dx,20
	jmp .fintecla
	.vueltaabajo:
	mov dx,180
	jmp .fintecla
	
	.abajo:
	cmp dx,180
	je .vueltaarriba
	add dx,20
	jmp .fintecla
	.vueltaarriba:
	mov dx,0
	jmp .fintecla
	
	.derecha:
	cmp cx,300
	je .vueltaizquierda
	add cx,20
	jmp .fintecla
	.vueltaizquierda:
	mov cx,0
	jmp .fintecla
	
	.izquierda:
	cmp cx,0
	je .vueltaderecha
	sub cx,20
	jmp .fintecla
	.vueltaderecha:
	mov cx,300
	jmp .fintecla

.fintecla:
	mov [bp-2],cx
	mov [bp-4],dx
	jmp .bucle

.fin:
	jmp $

;--------------------------------------------------------------------------------------------------------------------------
; DATOS DEL PROGRAMA
;--------------------------------------------------------------------------------------------------------------------------


;frutica. de 20x20, rescalada a 10x10 porque no cabe.
;los pixeles recudrantes (en la de 10x10) se dejan de margen, asi que no se incluye en la db
;se guarda solo el cuadrante superior izquierdo, y por simetria se saca el resto.
;se guarda en ascii el valor del color
frutica: db 'ºº))º)AA)AAB)ABB',0


;--------------------------------------------------------------------------------------------------------------------------
;esto siempre asi
;--------------------------------------------------------------------------------------------------------------------------
	times 510 - ($ - $$) db 0  ; Rellenamos con 0.
	dw 0xAA55                  ; Ponemos el numero magico
