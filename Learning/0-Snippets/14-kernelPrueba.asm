    [bits 32]
    mov ax, 10h
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    sti

    mov byte [0x0B8000], 'H'
    mov byte [0x0B8001], 1Bh
    mov byte [0x0B8002], 'O'
    mov byte [0x0B8003], 1Bh
    mov byte [0x0B8004], 'L'
    mov byte [0x0B8005], 1Bh
    mov byte [0x0B8006], 'A'
    mov byte [0x0B8007], 1Bh
    mov byte [0x0B8008], 'U'
    mov byte [0x0B8009], 1Bh
    in eax, 0x64

    cli
    hlt