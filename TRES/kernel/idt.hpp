#pragma once
#include "../lib/types.hpp"



struct idt_entry {
    tres::double_byte offset_bytes_inferiores;
    tres::double_byte selector;
    tres::byte __reservado;
    tres::byte tipo;
    tres::double_byte offset_bytes_superiores;
};

struct idt_pointer {
    tres::double_byte limite;
    tres::size_t* base;
};

//importante static
static struct {
    struct idt_entry entradas[256];
    struct idt_pointer puntero;
} idt;


asm(R"(
.global idt_load
	.type idt_load, @function
idt_load:
    mov 4(%esp), %eax
    lidt (%eax)
    ret
)");
extern void idt_load(tres::size_t* localizacion);


void idt_init() {
    idt.puntero.limite = sizeof(idt.entradas) - 1;
    idt.puntero.base = (tres::size_t*) &idt.entradas[0];

    for (tres::size_t i = 0; i < sizeof(idt.entradas); i++)
    {
        idt_entry e = { 0, 0, 0, 0, 0 };
        idt.entradas[i] = e;
    }

    idt_load((tres::size_t*) &idt.puntero);
}