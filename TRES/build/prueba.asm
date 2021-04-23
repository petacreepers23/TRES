; Simple bootloader that dumps the bytes in the BIOS Parameter
; Block BPB. First 3 bytes should be EB 3C 90. The rest should be 0xAA
; unless you have a BIOS that wrote drive geometry information
; into what it thinks is a BPB.

; Macro to print a character out with char in BX
%macro print_char 1
    mov al, %1
    call bios_print_char
%endmacro

org 0x7c00
bits 16

boot:
    jmp main
    TIMES 3-($-$$) DB 0x90   ; Support 2 or 3 byte encoded JMPs before BPB.

    ; Fake BPB filed with 0xAA
    TIMES 59 DB 0xAA

main:
    xor ax, ax
    mov ds, ax
    mov ss, ax              ; Set stack just below bootloader at 0x0000:0x7c00
    mov sp, boot
    cld                     ; Forward direction for string instructions

    mov si, sp              ; Print bytes from start of bootloader
    mov cx, main-boot       ; Number of bytes in BPB
    mov dx, 8               ; Initialize column counter to 8
                            ;     So first iteration prints address
.tblloop:
    cmp dx, 8               ; Every 8 hex value print CRLF/address/Colon/Space
    jne .procbyte
    print_char 0x0d         ; Print CRLF
    print_char 0x0a
    mov ax, si              ; Print current address
    call print_word_hex
    print_char ':'          ; Print ': '
    print_char ' '
    xor dx, dx              ; Reset column counter to 0
.procbyte:
    lodsb                   ; Get byte to print in AL
    call print_byte_hex     ; Print the byte (in BL) in HEX
    print_char ' '
    inc dx                  ; Increment the column count
    dec cx                  ; Decrement number of  bytes to process
    jnz .tblloop

    cli                     ; Halt processor indefinitely
.end:
    hlt
    jmp .end

; Print the character passed in AL
bios_print_char:
    push bx
    xor bx, bx              ; Attribute=0/Current Video Page=0
    mov ah, 0x0e
    int 0x10                ; Display character
    pop bx
    ret

; Print the 16-bit value in AX as HEX
print_word_hex:
    xchg al, ah             ; Print the high byte first
    call print_byte_hex
    xchg al, ah             ; Print the low byte second
    call print_byte_hex
    ret

; Print lower 8 bits of AL as HEX
print_byte_hex:
    push bx
    push cx
    push ax

    lea bx, [.table]        ; Get translation table address

    ; Translate each nibble to its ASCII equivalent
    mov ah, al              ; Make copy of byte to print
    and al, 0x0f            ;     Isolate lower nibble in AL
    mov cl, 4
    shr ah, cl              ; Isolate the upper nibble in AH
    xlat                    ; Translate lower nibble to ASCII
    xchg ah, al
    xlat                    ; Translate upper nibble to ASCII

    xor bx, bx              ; Attribute=0/Current Video Page=0
    mov ch, ah              ; Make copy of lower nibble
    mov ah, 0x0e
    int 0x10                ; Print the high nibble
    mov al, ch
    int 0x10                ; Print the low nibble

    pop ax
    pop cx
    pop bx
    ret
.table: db "0123456789ABCDEF", 0

; boot signature
TIMES 510-($-$$) db 0
dw 0xAA55