/* ** por compatibilidad se omiten tildes **
================================================================================
TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
definicion de funciones del scheduler
*/

#include "sched.h"
#include "i386.h"
#include "gdt.h"
#include "screen.h"
#include "tss.h"


extern uint estado_juego;

void inicializar_estructuras_sched(){
	jug_actual = JUGADOR_A;
	actual_tarea_jug_A = 0;
	actual_tarea_jug_B = 0;

	uint i = 0;
	while(i < 8){
		tareas_A[i] = false;
		tareas_B[i] = false;
		i++;
	}


}

uint sched_tick(){
	// Esto se llama desde la interrupcion del reloj

	uint prox_pir = prox_tarea_a_ejecutar();
	// prox pir es un indice de la gdt

	if(prox_pir != TSS_IDLE){
		if(jug_actual == JUGADOR_A){
			game_tick(prox_pir, JUGADOR_A);
		}
		else{
			game_tick(prox_pir, JUGADOR_B);
		}
		if(estado_juego == 0)
		{
			prox_pir = TSS_IDLE;
		}
	}
	return (prox_pir << 3);

}


uint prox_tarea_a_ejecutar(){
	// Modifica tareas y jugador actuales
	// Devuelve indice de la gdt
	if((noHayTareasA() && noHayTareasB()) || (estado_juego == 0)){
		return TSS_IDLE;
	}
	// Si llego acá, hay al menos un pirata


	if(jug_actual == JUGADOR_A){
		
		if(noHayTareasB()){
			return proximo_pirata_jugadorA();
		}
		else{
			jug_actual = JUGADOR_B;
			return proximo_pirata_jugadorB();
		}
	}
	else{
		// Jug actual es el B
		if(noHayTareasA()){
			return proximo_pirata_jugadorB();
		}
		else{
			jug_actual = JUGADOR_A;
			return proximo_pirata_jugadorA();
		}
	}
}


uint proximo_pirata_jugadorA(){
	uint i = actual_tarea_jug_A;
	while(i <= actual_tarea_jug_A + 8){
		//Para que recorra 8 elementos
		if(tareas_A[(i+1) % 8] == true){
			actual_tarea_jug_A = (i+1) % 8;
			return dame_inidice_gdt(JUGADOR_A, actual_tarea_jug_A);
		}
		i++;
	}
	return TSS_IDLE;
}



uint proximo_pirata_jugadorB(){
	uint i = actual_tarea_jug_B;
	while(i <= actual_tarea_jug_B + 8){
		//Para que recorra 8 elementos
		if(tareas_B[(i+1) % 8] == true){
			actual_tarea_jug_B = (i+1) % 8;
			return dame_inidice_gdt(JUGADOR_B, actual_tarea_jug_B);
		}
		i++;
	}
	return TSS_IDLE;
}

bool noHayTareasA(){
	uint i = 0;
	while(i < 8){
		if(tareas_A[i] == true){
			return false;
		}
		i++;
	}
	return true;
}

bool noHayTareasB(){
	uint i = 0;
	while(i < 8){
		if(tareas_B[i] == true){
			return false;
		}
		i++;
	}
	return true;
}