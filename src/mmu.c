/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
  definicion de funciones del manejador de memoria
*/

#include "mmu.h"
#include "i386.h"
#include "screen.h"

/* Atributos paginas */
/* -------------------------------------------------------------------------- */


/* Direcciones fisicas de codigos */
/* -------------------------------------------------------------------------- */
/* En estas direcciones estan los códigos de todas las tareas. De aqui se
 * copiaran al destino indicado por TASK_<i>_CODE_ADDR.
 */
unsigned int cant_tablas;
unsigned int proxima_pagina_libre;
unsigned int proxima_pagina_fisica = MAPA_BASE_FISICA;

void mmu_inicializar(){
	proxima_pagina_libre = 0x100000;
}

void mmu_inicializar_dir_kernel(){
	mmu_inicializar_page_dir(ADDRESS_PAGE_DIR);
	mmu_inicializar_page_table(ADDRESS_PAGE_TABLE);
}


unsigned dame_pagina_mapa_libre(){
	unsigned int pag = proxima_pagina_fisica;
	proxima_pagina_fisica += PAGE_SIZE;
	return pag;

}

void mmu_inicializar_page_dir(unsigned int direccion){
	dir_paginas* dp = (dir_paginas*) direccion; 

	dp[0].us = 0;
	dp[0].p = 1;
	dp[0].rw = 1;
	dp[0].base = (ADDRESS_PAGE_TABLE >> 12);
}

void mmu_inicializar_page_table(uint dir){
	tabla_paginas* td = (tabla_paginas*) dir;
	int i;
	// Identity mapping 
	for(i = 0; i < 1024; i++)
	{
		td[i].us = 0;
		td[i].base = i;
		td[i].rw = 1;
		td[i].p = 1;
	}
}

unsigned int dame_pagina_libre()
{
	unsigned int pagina_libre = proxima_pagina_libre;
	proxima_pagina_libre += PAGE_SIZE;
	return pagina_libre;
}

unsigned int nueva_tabla()
{

	tabla_paginas* td = (tabla_paginas*) dame_pagina_libre();
	int i;
	for(i = 0; i < 1024; i++)
	{
		*((unsigned int*) &td[i]) = 0;
	}

	for(i = 0; i < 1024; i++)
	{
		td[i].us = 0;
		td[i].base = ((unsigned int)td >> 12);
		td[i].rw = 1;
		td[i].p = 1;
	}
	return (unsigned int)td;
}

void prueba()
{
	mmu_mappear_pagina(MAPA_BASE_VIRTUAL, 0x27000, MAPA_BASE_FISICA, 0,1);
	mmu_unmappear_pagina(MAPA_BASE_VIRTUAL, 0x27000);
}

void mmu_mappear_pagina(unsigned int virtual, unsigned int dir_pd, unsigned int fisica, unsigned char rw, unsigned char us){
	

	unsigned int dir = (virtual >> 22);
	unsigned int table = (virtual << 10) >> 22;

	dir_paginas* pd = (dir_paginas*) dir_pd; 
	if(!pd[dir].p)
	{
		pd[dir].p = 1;
		pd[dir].rw = rw;
		pd[dir].us = us;
		pd[dir].base = dame_pagina_libre() >> 12;
	}
	
	tabla_paginas* td = (tabla_paginas*) (pd[dir].base << 12);
	td[table].base = (fisica >> 12);
	td[table].p = 1;
	td[table].rw = rw;
	td[table].us = us;
	tlbflush(); 	
}

void mmu_unmappear_pagina(unsigned int virtual, unsigned int cr3){
	unsigned int pde_index = (virtual >> 22);
	unsigned int pdt_index = (virtual << 10) >> 22;
	dir_paginas* pd = (dir_paginas*) cr3;
	
	tabla_paginas* td = (tabla_paginas*) (pd[pde_index].base << 12);
	if(td[pdt_index].p)
	{
		td[pdt_index].p = 0;
	}
}

unsigned int mmu_inicializar_dir_pirata(jugador_t* jug, bool esMinero, coordenada_t posicion_pirata){
	


	// Inicializo directorio en el area roja
	unsigned int direccion = dame_pagina_libre();
	mmu_inicializar_page_dir(direccion);


	// Copiemos el codigo
	//Hay que ponerse en el contexto del kernel
	uint cr3 = rcr3();
	lcr3(direccion);
	
	unsigned int posicion_final = game_xy2lineal(posicion_pirata.x, posicion_pirata.y)*0x1000 + MAPA_BASE_FISICA;
	// Mapeo
	mmu_mappear_pagina(0x400000, rcr3(), posicion_final, 1, 1);
	
	
	
	unsigned int aca_arranca_el_codigo = 0x10000;
	if(jug->index == JUGADOR_A){
		//jugadorA 
		if(esMinero){
			//minero
			aca_arranca_el_codigo = 0x11000;
		}
	}
	else{
		//jugadorB
		if(!esMinero){
			aca_arranca_el_codigo = 0x12000;
		}
		else{
			//minero 
			aca_arranca_el_codigo = 0x13000;
		}
	}


	copiar_pagina((uint*)0x400000, (uint*)aca_arranca_el_codigo);
	lcr3(cr3);

	//mapeo recorridas


	unsigned int i = 0;
	for(i = 0; i < MAPA_ALTO*MAPA_ANCHO; i++){
		if(jug -> posiciones_recorridas[i] != 0){
			//breakpoint();
			mmu_mappear_pagina(MAPA_BASE_VIRTUAL + i*0x1000, direccion, MAPA_BASE_FISICA + i*0x1000, 1, 1);
		}
	}

	return direccion;

}


void copiar_pagina(uint* dir_destino, uint* dir_fuente){

	unsigned int i = 0;
	//Copio los 4k de la pagina
	
	while(i < 1024){
		dir_destino[i] = dir_fuente[i];
		i++;
	}
}
