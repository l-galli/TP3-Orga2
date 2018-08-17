/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
  definicion de estructuras para administrar tareas
*/

#include "tss.h"
#include "mmu.h"
#include "screen.h"
#include "sched.h"


tss tss_inicial;
tss tss_idle;
unsigned int index_pirata_A = 0;
unsigned int index_pirata_B = 0;


uint indice_definitivo_gdt_A = 0xF;
uint indice_definitivo_gdt_B = 0x17;



void tss_inicializar() {
	gdt[TSS_INICIAL].base_31_24 = ((unsigned int) (&tss_inicial)) >> 24;
	gdt[TSS_INICIAL].base_23_16 = ((unsigned int) (&tss_inicial)) >> 16;
	gdt[TSS_INICIAL].base_0_15 = ((unsigned int) (&tss_inicial));

	tss_inicializar_idle();

}

void tss_inicializar_idle(){
	gdt[TSS_IDLE].base_31_24 = ((unsigned int) (&tss_idle)) >> 24;
	gdt[TSS_IDLE].base_23_16 = ((unsigned int) (&tss_idle)) >> 16;
	gdt[TSS_IDLE].base_0_15 = ((unsigned int) (&tss_idle));

	tss_idle.eip = 0x00016000;
	tss_idle.cr3 = rcr3();
	tss_idle.iomap = 0xFFFF;
	tss_idle.esp = 0x27000;
	tss_idle.ebp = 0x27000;
	tss_idle.eflags = 0x202;

	tss_idle.es = 0x40;
    tss_idle.ds = 0x40;
    tss_idle.ss = 0x40;
    tss_idle.gs = 0x40;
    tss_idle.fs = 0x60; // Segmento de video
	tss_idle.cs = 0x50;

	tss_idle.edi = 0;
	tss_idle.esi = 0;
	tss_idle.ebx = 0;
	tss_idle.edx = 0;
	tss_idle.ecx = 0;
	tss_idle.eax = 0;
}



void tss_inicializar_tarea_pirata(unsigned int indice_pirata, jugador_t* jug, bool minero){
	// Recordar:
	// #define JUGADOR_A  0
	// #define JUGADOR_B  1

	tss* direccion_tss;
	if(jug->index == JUGADOR_A){
		direccion_tss = &tss_jugadorA[indice_pirata];
	}
	else{
		direccion_tss = &tss_jugadorB[indice_pirata];
	}

	rellenar_gdt_pirata(jug->index, indice_pirata, direccion_tss);

	coordenada_t pos = jug->piratas[indice_pirata].posicion_actual;


	completarTSSlibre(direccion_tss, mmu_inicializar_dir_pirata(jug, minero, pos), minero, jug);
	
	if(jug->index == JUGADOR_A){
		tareas_A[indice_pirata] = true;
	}
	else{
		tareas_B[indice_pirata] = true;
	}

}

void completarTSSlibre(tss* _tss, unsigned int cr3, bool minero, jugador_t* jug){

	_tss->eip = 0x400000;
	_tss->cr3 = cr3;
	_tss->iomap = 0xFFFF;
	_tss->esp = 0x400000 + 0x1000 - 12;
	_tss->ebp = 0x400000 + 0x1000 - 12; 
	_tss->eflags = 0x202;

 	_tss->es = (GDT_IDX_DAT_3 << 3) + 3;
    _tss->ds = (GDT_IDX_DAT_3 << 3) + 3;
    _tss->ss = (GDT_IDX_DAT_3 << 3) + 3;
    _tss->gs = (GDT_IDX_DAT_3 << 3) + 3;
	_tss->cs = (GDT_IDX_COD_3 << 3) + 3;
	_tss->fs = (GDT_IDX_DAT_3 << 3) + 3;


	unsigned int dir_pila = dame_pagina_libre() + 0x1000; 
	_tss->esp0 = dir_pila;
	_tss->ss0 = 0x40; 


	_tss->edi = 0;
	_tss->esi = 0;
	_tss->ebx = 0;
	_tss->edx = 0;
	_tss->ecx = 0;
	_tss->eax = 0;
}


void setear_gdt_pirata(uint index, uint jug){



tss* direccion_tss = ((jug == JUGADOR_A) ? &tss_jugadorA[index % 8] : &tss_jugadorB[index % 8]);



gdt[index].base_31_24 = ((unsigned int) direccion_tss) >> 24;
gdt[index].base_23_16 = (((unsigned int) direccion_tss) << 7) >> 23;
gdt[index].base_0_15 = ((unsigned int) direccion_tss);

gdt[index].limit_0_15 = 0x67;
gdt[index].type = 0x9;
gdt[index].s = 0x00;
gdt[index].dpl = 0x0;
gdt[index].p = 0x1;
gdt[index].limit_16_19 = 0x0;
gdt[index].avl = 0x00;
gdt[index].l = 0x00;
gdt[index].db = 0x1;
gdt[index].g = 0x0;
}


uint dame_inidice_gdt(uint jugador, uint indice_tarea){
	if(jugador == JUGADOR_A){
		return 0xF + indice_tarea;
	}
	else{
		return 0xF + indice_tarea + 8;
	}
}

void rellenar_gdt_pirata(uint jugador, uint indice_tarea, tss* direccion_tss){

	uint indice_definitivo_gdt = dame_inidice_gdt(jugador, indice_tarea);

	gdt[indice_definitivo_gdt].base_31_24 = ((unsigned int) direccion_tss) >> 24;
	gdt[indice_definitivo_gdt].base_23_16 = (((unsigned int) direccion_tss) << 7) >> 23;
	gdt[indice_definitivo_gdt].base_0_15 = ((unsigned int) direccion_tss);

	gdt[indice_definitivo_gdt].limit_0_15 = 0x67;
	gdt[indice_definitivo_gdt].type = 0x9;
	gdt[indice_definitivo_gdt].s = 0x00;
	gdt[indice_definitivo_gdt].dpl = 0x0;
	gdt[indice_definitivo_gdt].p = 0x1;
	gdt[indice_definitivo_gdt].limit_16_19 = 0x0;
	gdt[indice_definitivo_gdt].avl = 0x00;
	gdt[indice_definitivo_gdt].l = 0x00;
	gdt[indice_definitivo_gdt].db = 0x1;
	gdt[indice_definitivo_gdt].g = 0x0;
}


void test(){

	tss* _tss = &(tss_jugadorA[0]);

	tareas_A[0] = true;

	uint indice_definitivo_gdt = 0xF;

	gdt[indice_definitivo_gdt].base_31_24 = ((unsigned int) _tss) >> 24;
	gdt[indice_definitivo_gdt].base_23_16 = ((unsigned int) _tss) >> 16;
	gdt[indice_definitivo_gdt].base_0_15 = ((unsigned int) _tss);


	gdt[indice_definitivo_gdt].limit_0_15 = 0x67;
	gdt[indice_definitivo_gdt].type = 0x9;
	gdt[indice_definitivo_gdt].s = 0x00;
	gdt[indice_definitivo_gdt].dpl = 0x03;
	gdt[indice_definitivo_gdt].p = 0x1;
	gdt[indice_definitivo_gdt].limit_16_19 = 0x0;
	gdt[indice_definitivo_gdt].avl = 0x00;
	gdt[indice_definitivo_gdt].l = 0x00;
	gdt[indice_definitivo_gdt].db = 0x1;
	gdt[indice_definitivo_gdt].g = 0x0;

	coordenada_t pos;
	pos.x = 1;
	pos.y = 1;

	

	_tss->eip = 0x400000;
	_tss->cr3 = mmu_inicializar_dir_pirata(&jugadorA, false, pos);
	_tss->iomap = 0xFFFF;
	_tss->esp = 0x400000 + 0x1000 - 12;
	_tss->ebp = 0x400000 + 0x1000 - 12; 
	_tss->eflags = 0x202;

 	_tss->es = (GDT_IDX_DAT_3 << 3) + 3;
    _tss->ds = (GDT_IDX_DAT_3 << 3) + 3;
    _tss->ss = (GDT_IDX_DAT_3 << 3) + 3;
    _tss->gs = (GDT_IDX_DAT_3 << 3) + 3;
	_tss->cs = (GDT_IDX_COD_3 << 3) + 3;
	_tss->fs = (GDT_IDX_DAT_3 << 3) + 3;

	unsigned int dir_pila = dame_pagina_libre() + 0x1000; 
	_tss->esp0 = dir_pila;
	_tss->ss0 = 0x40; 

	_tss->edi = 0;
	_tss->esi = 0;
	_tss->ebx = 0;
	_tss->edx = 0;
	_tss->ecx = 0;
	_tss->eax = 0;

}

uint dame_cr3(uint jugador, uint indice_tss_jug){
	if(jugador == JUGADOR_A){
		return tss_jugadorA[indice_tss_jug].cr3;
	}else{
		return tss_jugadorB[indice_tss_jug].cr3;
	}
}