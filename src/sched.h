/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
  definicion de funciones del scheduler
*/

#ifndef __SCHED_H__
#define __SCHED_H__

#include "game.h"

uint actual_tarea_jug_A;
uint actual_tarea_jug_B;
// Tiene el indice de la gdt

uint jug_actual;

bool tareas_A[8];
bool tareas_B[8];

void inicializar_estructuras_sched();
uint sched_tick();
uint prox_tarea_a_ejecutar();
bool noHayTareasA();
bool noHayTareasB();

uint proximo_pirata_jugadorA();
uint proximo_pirata_jugadorB();

#endif	/* !__SCHED_H__ */
