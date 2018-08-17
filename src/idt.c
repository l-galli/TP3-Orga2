
/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
  definicion de las rutinas de atencion de interrupciones
*/

#include "defines.h"
#include "idt.h"
#include "isr.h"

#include "tss.h"

idt_entry idt[255] = { };

idt_descriptor IDT_DESC = {
    sizeof(idt) - 1,
    (unsigned int) &idt
};

/*
    La siguiente es una macro de EJEMPLO para ayudar a armar entradas de
    interrupciones. Para usar, descomentar y completar CORRECTAMENTE los
    atributos y el registro de segmento. Invocarla desde idt_inicializar() de
    la siguiene manera:

    void idt_inicializar() {
        IDT_ENTRY(0);
        ...
        IDT_ENTRY(19);

        ...
    }
*/


#define IDT_ENTRY(numero, segmento, atributos)                                              \
    idt[numero].offset_0_15 = (unsigned short) ((unsigned int)(&_isr ## numero) & (unsigned int) 0xFFFF);        \
    idt[numero].segsel = (unsigned short) segmento;                                                               \
    idt[numero].attr = (unsigned short) atributos;                                                                 \
    idt[numero].offset_16_31 = (unsigned short) ((unsigned int)(&_isr ## numero) >> 16 & (unsigned int) 0xFFFF);

#define IDT_TASK(numero)  \
    IDT_ENTRY(numero, SEG_SEL_CODE_0, 0x8500);
/* 8500 -> p = 1 | DPL = 0 0 | 0 0 1 0 1 | 0 0 0 | 00000 | */
#define IDT_ENTRY_INT(numero)   \
    IDT_ENTRY(numero, SEG_SEL_CODE_0, 0x8E00);
/* 8E00 -> p = 1 | DPL = 0 0 | 0 (D=1) 1 1 0 | 0 0 0 | 00000 | */

#define IDT_ENTRY_TRAP(numero)   \
    IDT_ENTRY(numero, SEG_SEL_CODE_0, 0x8F00);
/* 8F00 -> p = 1 | DPL = 0 0 | 0 (D=1) 1 1 1 | 0 0 0 | 00000 | */

#define IDT_SOFTWARE(numero)   \
    IDT_ENTRY(numero, SEG_SEL_CODE_0, 0xEE00)

void idt_inicializar() {
    IDT_ENTRY_INT(0);
    IDT_ENTRY_INT(1);
    IDT_ENTRY_INT(2);
    IDT_ENTRY_INT(3);
    IDT_ENTRY_INT(4);
    IDT_ENTRY_INT(5);
    IDT_ENTRY_INT(6);
    IDT_ENTRY_INT(7);
    IDT_ENTRY_INT(8);
    IDT_ENTRY_INT(9);
    IDT_ENTRY_INT(10);
    IDT_ENTRY_INT(11);
    IDT_ENTRY_INT(12);
    IDT_ENTRY_INT(13);
    IDT_ENTRY_INT(14);
    IDT_ENTRY_INT(15);
    IDT_ENTRY_INT(16);
    IDT_ENTRY_INT(17);
    IDT_ENTRY_INT(18);
    IDT_ENTRY_INT(19);

    IDT_ENTRY_INT(32); //reloj
    IDT_ENTRY_INT(33); //teclado

    IDT_SOFTWARE(0x46); //software
}
