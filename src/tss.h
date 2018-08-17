/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
  definicion de estructuras para administrar tareas
*/

#ifndef __TSS_H__
#define __TSS_H__

#include "defines.h"
#include "i386.h"
#include "gdt.h"
#include "game.h"


typedef struct str_tss {
    unsigned short  ptl;
    unsigned short  unused0;
    unsigned int    esp0;
    unsigned short  ss0;
    unsigned short  unused1;
    unsigned int    esp1;
    unsigned short  ss1;
    unsigned short  unused2;
    unsigned int    esp2;
    unsigned short  ss2;
    unsigned short  unused3;
    unsigned int    cr3;
    unsigned int    eip;
    unsigned int    eflags;
    unsigned int    eax;
    unsigned int    ecx;
    unsigned int    edx;
    unsigned int    ebx;
    unsigned int    esp;
    unsigned int    ebp;
    unsigned int    esi;
    unsigned int    edi;
    unsigned short  es;
    unsigned short  unused4;
    unsigned short  cs;
    unsigned short  unused5;
    unsigned short  ss;
    unsigned short  unused6;
    unsigned short  ds;
    unsigned short  unused7;
    unsigned short  fs;
    unsigned short  unused8;
    unsigned short  gs;
    unsigned short  unused9;
    unsigned short  ldt;
    unsigned short  unused10;
    unsigned short  dtrap;
    unsigned short  iomap;
} __attribute__((__packed__, aligned (8))) tss;







void rellenar_TSS();


uint dame_cr3(uint jugador, uint indice_tss_jug);




tss tss_jugadorA[MAX_CANT_PIRATAS_VIVOS];
tss tss_jugadorB[MAX_CANT_PIRATAS_VIVOS];


void tss_inicializar();
void tss_inicializar_idle();
void tss_inicializar_tarea_inicial();
void tss_inicializar_tarea_pirata(unsigned int indice_pirata, jugador_t* jug, bool minero);
void completarTSSlibre(tss* _tss, unsigned int cr3, bool minero, jugador_t* jug);

//t
void rellenar_TSS(jugador_t*j, unsigned int index_pirata);
void rellenar_TSS_A();
void rellenar_TSS_B();
void rellenar_TSS_A_all();
void rellenar_TSS_B_all();
void setear_gdt_pirata(uint index, uint jug);
void prueba_jugA();
void prueba_jugB();
void prueba_jugA_all(uint jugador);
void prueba_jugB_all(uint jugador);
uint dame_inidice_gdt(uint jugador, uint indice_tarea);
void rellenar_gdt_pirata(uint jugador, uint indice_tarea, tss* direccion_tss);


#endif  /* !__TSS_H__ */
