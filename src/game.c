/* ** por compatibilidad se omiten tildes **
================================================================================
TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
*/

#include "game.h"
#include "mmu.h"
#include "tss.h"
#include "screen.h"
#include "sched.h"
#include "colors.h"






#define CANT_POSICIONES_VISTAS            9
#define MAX_SIN_CAMBIOS                999

#define BOTINES_CANTIDAD 8


uint botines[BOTINES_CANTIDAD][3] = { // TRIPLAS DE LA FORMA (X, Y, MONEDAS)
                                        {30,  3, 50}, {31, 38, 50}, {15, 21, 100}, {45, 21, 100} ,
                                        {49,  3, 50}, {48, 38, 50}, {64, 21, 100}, {34, 21, 100}
                                    };

jugador_t jugadorA;	
jugador_t jugadorB;

uint estado_juego; // estado del juego

uint contador_juego; //contador del juego

uint jugador_actual; // Jugador cuyo pirata es el actual
// Jugador A = 0, B = 1
uint proxima_id = 1;

void* error()
{   
	__asm__ ("int3");
	return 0;
}

uint game_xy2lineal (uint x, uint y) {
	return (y * MAPA_ANCHO + x);
}

uint game_posicion_valida(int x, int y) {
	return (x >= 0 && y >= 0 && x < MAPA_ANCHO && y < MAPA_ALTO);
}

pirata_t* id_pirata2pirata(uint id_pirata_gdt)
{
	
    jugador_t* act;
    if(jugador_actual == JUGADOR_A){
    	act = &jugadorA;
    }
    else{
    	act = &jugadorB;
    }
    uint i = 0;
    while(i < 8){
    	if(id_pirata_gdt == act->piratas[i].indice_gdt){
    		return &(act->piratas[i]);
    	}
    	i++;
    }
  
	return NULL;
}

jugador_t* dameJugadorActual(){
    jugador_t* act;
    if(jugador_actual == JUGADOR_A){
        act = &jugadorA;
    }
    else{
        act = &jugadorB;
    }
    return act;
}

uint game_dir2xy(direccion dir, int *x, int *y)
{
	switch (dir)
	{
		case IZQ: *x = -1; *y =  0; break;
		case DER: *x =  1; *y =  0; break;
		case ABA: *x =  0; *y =  1; break;
		case ARR: *x =  0; *y = -1; break;
    	default: return -1;
	}

	return 0;
}

uint game_valor_tesoro(uint x, uint y)
{
	int i;
	for (i = 0; i < BOTINES_CANTIDAD; i++)
	{
		if (botines[i][0] == x && botines[i][1] == y && botines[i][2] != 0)
			return botines[i][2];
	}
	return 0;
}

// dada una posicion (x,y) guarda las posiciones de alrededor en dos arreglos, uno para las x y otro para las y
void game_calcular_posiciones_vistas(int *vistas_x, int *vistas_y, int x, int y)
{
	int next = 0;
	int i, j;
	for (i = -1; i <= 1; i++)
	{
		for (j = -1; j <= 1; j++)
		{
			vistas_x[next] = x + j;
			vistas_y[next] = y + i;
			next++;
		}
	}
}


void game_inicializar()
{	
	gdt_pirata_actual = 0;
	jugador_actual = JUGADOR_A;
	jugadorA.index = JUGADOR_A;
	jugadorB.index = JUGADOR_B;
	game_jugador_inicializar(&jugadorA);
	game_jugador_inicializar(&jugadorB);

	estado_juego = true; // el juego comienza
	contador_juego = MAX_SIN_CAMBIOS;
}

void game_jugador_inicializar_mapa(jugador_t *jug)
{
}

void game_jugador_inicializar(jugador_t *j)
{

	j->cantActivos  = 0;
	j->mineros_a_lanzar = 0;
	j->puntos = 0;
	uint k = 0;
	while(k < 3){
		j->botines[k].x = 9999;
		j->botines[k].y = 9999;
		k++;
	}
	j->cant_exploradores = 0;
	j->cant_mineros = 0;

	uint l = 0;
	while(l < MAX_CANT_PIRATAS_VIVOS){
		j->piratas[l].activo = false;
		l++;
	}


   
    unsigned int i = 0;
	while(i < MAPA_ALTO*MAPA_ANCHO){
		j -> posiciones_recorridas[i] = 0;
		i++;
	}

	

	int x = -1;
	int y = -1;
	for(x = -1; x <= 1; x++){
		for(y = -1; y <= 1; y++){
			int posX;
			int posY;
			if(j->index == JUGADOR_A){
				posX = x + POS_INIT_A_X;
				posY = y + POS_INIT_A_Y;
			}
			else{
				posX = x + POS_INIT_B_X;
				posY = y + POS_INIT_B_Y;
			}
			j->posiciones_recorridas[game_xy2lineal(posX, posY)] = 1;
			
			
		}
		
	}


}
uint id_actual()
{
	uint actual = proxima_id;
	proxima_id++;
	return actual;
}

void game_pirata_inicializar(pirata_t *pirata, jugador_t *j, uint indice_arreglo_jug, uint indice_gdt)
{	
	pirata->indice_gdt = indice_gdt;
	pirata->indice_arreglo_jug = indice_arreglo_jug;
	j->piratas[indice_arreglo_jug] = *pirata;
}

void game_tick(uint id_pirata, uint jugador)
{
	gdt_pirata_actual = id_pirata;
	jugador_actual = jugador;
	jugador_t* jug = id_pirata2pirata(id_pirata)->jug;
	screen_actualizar();
	if(jug->cantActivos < MAX_CANT_PIRATAS_VIVOS && jug->mineros_a_lanzar > 0){
		
		jug->mineros_a_lanzar--;
		game_jugador_lanzar_pirata(jug->index, true);
	}
	game_terminar_si_es_hora();
}

bool nadie_en_puerto(uint jug){
	uint i = 0;
	jugador_t* jugA = &jugadorA;
	jugador_t* jugB = &jugadorB;

	uint x = 0;
	uint y = 0;

	if(jug == JUGADOR_B){
		x = 79;
		y = 44;
	} 


	while(i < MAX_CANT_PIRATAS_VIVOS){
		pirata_t* pirA = &(jugA->piratas[i]);
		if((pirA->activo) && (pirA->posicion_actual.x == x) && (pirA->posicion_actual.y == y)){
			return false;
		}

		pirata_t* pirB = &(jugB->piratas[i]);
		if((pirB->activo) && (pirB->posicion_actual.x == x) && (pirB->posicion_actual.y == y)){
			return false;
		}


		i++;
	}
	return true;
}


void game_pirata_relanzar(pirata_t *pirata, jugador_t *j, uint tipo)
{
}

pirata_t* game_jugador_erigir_pirata(jugador_t *j, uint tipo)
{
    
	return NULL;
}

void test_pirata(){
	// Lanzamos explorador
	//game_jugador_lanzar_pirata(JUGADOR_A, false);
	lanzar_pirata_test();
}

void lanzar_pirata_test(){
	jugador_t* jug = &jugadorA;
	jug->cantActivos++;

	uint indice_a_usar = 0;
	jug->piratas[indice_a_usar].activo = true;
	jug->piratas[indice_a_usar].jug = jug;
	jug->piratas[indice_a_usar].indice_arreglo_jug = indice_a_usar;
	jug->piratas[indice_a_usar].indice_gdt = 0xF + indice_a_usar;
	jug->piratas[indice_a_usar].posicion_actual.x = 1;
	jug->piratas[indice_a_usar].posicion_actual.y = 1;
	jug->piratas[indice_a_usar].tipo = false;
	jug->piratas[indice_a_usar].reloj = 0;

	jug->cant_exploradores++;

	gdt_pirata_actual = 0xF;
}

void game_jugador_lanzar_pirata(uint _jugador, bool esMinero){
	// minero = true
	
	jugador_t* jug = &jugadorA;
	if(_jugador == JUGADOR_B){
		jug = &jugadorB;


	}

	if(jug->cantActivos < MAX_CANT_PIRATAS_VIVOS && estado_juego == true){
		jug->cantActivos++;


		uint i = 0;
		uint indice_a_usar; 

		while(i < 8){
			if(jug->piratas[i].activo == false){
				indice_a_usar = i;
				break;
			}
			i++;
		}
		if(indice_a_usar == 1){
			//breakpoint();
		}

		uint pos_puerto_x = ((_jugador == JUGADOR_A) ? POS_INIT_A_X : POS_INIT_B_X);
		uint pos_puerto_y = ((_jugador == JUGADOR_A) ? POS_INIT_A_Y : POS_INIT_B_Y);
		
		jug->piratas[indice_a_usar].activo = true;
		jug->piratas[indice_a_usar].jug = jug;
		jug->piratas[indice_a_usar].indice_arreglo_jug = indice_a_usar;
		jug->piratas[indice_a_usar].indice_gdt = 0xF + indice_a_usar + ((_jugador == JUGADOR_B) ? 8 : 0);
		jug->piratas[indice_a_usar].posicion_actual.x = pos_puerto_x;
		jug->piratas[indice_a_usar].posicion_actual.y = pos_puerto_y;
		jug->piratas[indice_a_usar].tipo = esMinero;
		jug->piratas[indice_a_usar].reloj = 0;


		// FLAG
		tss_inicializar_tarea_pirata(indice_a_usar, jug, esMinero);

		pirata_t* pir = &jug->piratas[indice_a_usar];

		pintar_puerto(pos_puerto_x, pos_puerto_y, pir, jug);
	

		if(esMinero){
			jug->cant_mineros++;
			// Si es minero entonces hubo un botin 
			// Hay que pasarle las coordenadas


			uint posicion_de_posicion;
			uint k = 0;
			for(k = 0; k < 3; k++){
				if(jug->botines[k].x != 9999){
					posicion_de_posicion = k;
					break;
				}
				
			}


	
			uint* valor_x = (uint*)(0x400000 + 0x1000 - 8);
			uint* valor_y = (uint*)(0x400000 + 0x1000 - 4); 
				
			uint cr3_pirata = dame_cr3(_jugador, indice_a_usar);
			uint cr3_actual = rcr3();

			lcr3(cr3_pirata);
			*valor_x = (uint)jug->botines[posicion_de_posicion].x;
			*valor_y = (uint)jug->botines[posicion_de_posicion].y;
			lcr3(cr3_actual);

			jug->piratas[indice_a_usar].tesoro.x = (uint)jug->botines[posicion_de_posicion].x;
			jug->piratas[indice_a_usar].tesoro.y = (uint)jug->botines[posicion_de_posicion].y;


			jug->botines[posicion_de_posicion].x = 9999;
			jug->botines[posicion_de_posicion].y = 9999;



		}
		else{
			jug->cant_exploradores++;
		}

		
	}



}

void pintar_puerto(uint pos_puerto_x, uint pos_puerto_y, pirata_t* pir, jugador_t* jug){
	screen_descubrir_area(pos_puerto_x,pos_puerto_y, pos_puerto_x, pos_puerto_y, pir, jug, 1);

	int i = -1;
    int j = -1;
    for(i = -1; i <= 1; i++){
    	for(j = -1; j <= 1; j++){

    		if(i != 0 || j != 0){
    			screen_descubrir_area(pos_puerto_x,pos_puerto_y, pos_puerto_x + i, pos_puerto_y + j, pir, jug, 0);
    		}
    		if(i == 1 && j == 1){
    			screen_descubrir_area(pos_puerto_x ,pos_puerto_y , pos_puerto_x , pos_puerto_y , pir, jug, 1);
    		}
    	}
    }
}

void game_pirata_habilitar_posicion(jugador_t *j, pirata_t *pirata, int x, int y)
{
}


void game_explorar_posicion(jugador_t *jugador, int c, int f)
{
}


uint game_syscall_pirata_mover(direccion dir)
{
	
    pirata_t* pir = id_pirata2pirata(gdt_pirata_actual);
   
   
    if(puedo_moverme(pir, dir)){
    	/* Me muevo*/
       
    	uint posicion_vieja_x = pir->posicion_actual.x;
    	uint posicion_vieja_y = pir->posicion_actual.y;

    	uint posicion_vieja = game_xy2lineal(posicion_vieja_x, posicion_vieja_y)*0x1000 + MAPA_BASE_VIRTUAL;

        int x = 0;
        int y = 0;
        game_dir2xy(dir, &x, &y);
        pir->posicion_actual.x += x;
        pir->posicion_actual.y += y;


       	mmu_unmappear_pagina(0x400000, rcr3());
        unsigned int posicion_final = game_xy2lineal(pir->posicion_actual.x, pir->posicion_actual.y)*0x1000 + MAPA_BASE_FISICA;
        mmu_mappear_pagina(0x400000, rcr3(), posicion_final, 1, 1);
  

        copiar_pagina((uint*)0x400000, (uint*)posicion_vieja);
        

        screen_descubrir_area(posicion_vieja_x, posicion_vieja_y, pir->posicion_actual.x, pir->posicion_actual.y, pir, dameJugadorActual(), 1);

     

        if(!(pir->tipo)){
        	//explorador
        
            resolver_tarea_explorador(pir, dameJugadorActual()); 
        }
        else{
        	
        }
        return 0;
	}
    else{
    	matar_pirata();
    	return 0;
    }
}

void resolver_tarea_explorador(pirata_t* pir, jugador_t* jug){
    int i = -1;
    int j = -1;
    for(i = -1; i <= 1; i++){
    	for(j = -1; j <= 1; j++){
    		uint indice = game_xy2lineal(pir->posicion_actual.x + i, pir->posicion_actual.y + j);
			if(game_posicion_valida(pir->posicion_actual.x + i, pir->posicion_actual.y + j)){
		        
		        if(j != 0 || i != 0){
					screen_descubrir_area(pir->posicion_actual.x , pir->posicion_actual.y , pir->posicion_actual.x + i, pir->posicion_actual.y + j + 1, pir, dameJugadorActual(), 0);
					
				}
				


		        if(jug->posiciones_recorridas[indice] == 0){
		        	jug->posiciones_recorridas[indice] = 1;
		        	mmu_mappear_pagina(MAPA_BASE_VIRTUAL + indice*0x1000, rcr3(), MAPA_BASE_FISICA + indice*0x1000, 0, 1);
		        	
		        	mapear_a_todos(pir, jug, indice);

		        }
				if(game_valor_tesoro(pir->posicion_actual.x + i, pir->posicion_actual.y + j)){
					//si hay tesoro en zona descubierta
						
					uint bot = 0;
					for(bot = 0; bot < 3; bot++){
						// busca una posición libre del arreglo de botines y agrega la coord
						// como vas explorando de a 3, máximo tenés 3 botines
						if(jug->botines[bot].x == 9999 && no_lo_tome(jug, pir->posicion_actual.x + i, pir->posicion_actual.y + j)){
							
							jug->botines[bot].x = pir->posicion_actual.x + i;
							jug->botines[bot].y = pir->posicion_actual.y + j;
							jug->mineros_a_lanzar++;
							break;
						}
					}

				}

			}
					


        }
	}
}
// No lo tomeeeeeeeeeeeeeeeee ieeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee
bool no_lo_tome(jugador_t* jug, uint x, uint y){
	uint i;
	for(i = 0; i < MAX_CANT_PIRATAS_VIVOS; i++){
		pirata_t* pir = &(jug->piratas[i]);
		if(pir->activo && pir->tipo){
		
			if(pir->tesoro.x == x && pir->tesoro.y == y){
		
				return false;
			}
		}
	}
	return true;
}

void mapear_a_todos(pirata_t* pir, jugador_t* jug, uint indice){
	uint i = 0;

	while(i < MAX_CANT_PIRATAS_VIVOS){
		if(jug->piratas[i].activo && i != pir->indice_arreglo_jug){
			uint PD;

			if(jug->index == JUGADOR_A){
				PD = tss_jugadorA[i].cr3;
			}
			else{
				PD = tss_jugadorB[i].cr3;
			}
        	mmu_mappear_pagina(MAPA_BASE_VIRTUAL + indice*0x1000, PD, MAPA_BASE_FISICA + indice*0x1000, 0, 1);
		}
		i++;
	}
}

bool puedo_moverme(pirata_t* pir, direccion dir){
	



	int x = 0;
	int y = 0;
	
	game_dir2xy(dir, &x, &y);
	coordenada_t posicion_pirata = pir->posicion_actual;



	posicion_pirata.x += x;
	posicion_pirata.y += y;
	bool se_puede = true;
	
	if(!game_posicion_valida(posicion_pirata.x, posicion_pirata.y)){
		
        se_puede = false;

	}

	if(pir->tipo){
		// Es un minero
		if(pir->jug->posiciones_recorridas[game_xy2lineal(posicion_pirata.x, posicion_pirata.y)] == 0){
			
			se_puede = false;
		}

	}
    return se_puede;

}


void game_syscall_cavar(){
	
	jugador_t* jug = dameJugadorActual();
	pirata_t* pir = id_pirata2pirata(gdt_pirata_actual);

	if(game_valor_tesoro(pir->posicion_actual.x, pir->posicion_actual.y) > 0){
		jug->puntos++;
		int i;
		for(i = 0; i < BOTINES_CANTIDAD; i++){
			if(botines[i][0] == pir->posicion_actual.x && botines[i][1] == pir->posicion_actual.y){
				botines[i][2]--;
				contador_juego = MAX_SIN_CAMBIOS;
				screen_pintar('M', C_FG_WHITE + C_BG_RED, pir->posicion_actual.y + 1, pir->posicion_actual.x);
			}
		}

	}else{
		matar_pirata();
	}


}


void matar_pirata(){
	jugador_t* jug = dameJugadorActual();
	pirata_t* pir = id_pirata2pirata(gdt_pirata_actual);
	pir->activo = false;
	jug->piratas[pir->indice_arreglo_jug].activo = false; 

	jug->cantActivos--;
	if(pir->tipo){
		jug->cant_mineros--;
	}else{
		jug->cant_exploradores--;

	}

	if(jug->index == JUGADOR_A){
		tareas_A[pir->indice_arreglo_jug] = false;
	}else{
		tareas_B[pir->indice_arreglo_jug] = false;
	}

	screen_matar_pirata(pir);


}

uint game_syscall_pirata_posicion(int idx){
    
    if(idx == -1){
    	pirata_t* pir = id_pirata2pirata(gdt_pirata_actual);
    	return ((pir->posicion_actual.y << 8) | (pir->posicion_actual.x));
    }else{
    	
    	jugador_t* jug = dameJugadorActual();
    	return ((jug->piratas[idx].posicion_actual.y << 8) | (jug->piratas[idx].posicion_actual.x));
    }




    return 0;
}

void game_reiniciar()
{

	botines[0][3] = 50; 
	botines[1][3] = 50;
	botines[2][3] = 100;	
	botines[3][3] = 100;
	botines[4][3] = 50;
	botines[5][3] = 50;
	botines[6][3] = 100;
	botines[7][3] = 100;

	inicializar_estructuras_sched();
	game_inicializar();
	screen_inicializar();


}

uint game_syscall_manejar(uint syscall, uint param1)
{

    return 0;
}

void game_pirata_exploto(uint id)
{
}

pirata_t* game_pirata_en_posicion(uint x, uint y)
{
	return NULL;
}


void game_jugador_anotar_punto(jugador_t *j)
{
}

bool sin_monedas()
{
	int i = 0;
	for (i = 0; i < BOTINES_CANTIDAD; i++)
	{
		if(botines[i][2] > 0)
		{
			return false;
		}
	}
	return true;
}

void game_terminar_si_es_hora()
{
	
	if( (jugadorA.cantActivos == MAX_CANT_PIRATAS_VIVOS) && (jugadorB.cantActivos == MAX_CANT_PIRATAS_VIVOS)){
		contador_juego--;
	}else{
		contador_juego = MAX_SIN_CAMBIOS;
	}
	

	if(contador_juego == 0 || sin_monedas() == true)
	{
		estado_juego = 0;
		screen_game_over();
		uint ganador;
		if(jugadorA.puntos > jugadorB.puntos){
			ganador = JUGADOR_A;
		}
		else if(jugadorA.puntos < jugadorB.puntos){
			ganador = JUGADOR_B;
		}
		else{
			ganador = 9999;
		}
		screen_pintar_ganador(ganador);
		//Matar tareas
		uint k;
		for(k = 0; k < MAX_CANT_PIRATAS_VIVOS; k++){
			tareas_A[k] = false;
			tareas_B[k] = false;
		}
	}


}


#define KB_w        0x11 // 0x91
#define KB_s        0x1f // 0x9f
#define KB_a        0x1e // 0x9e
#define KB_d        0x20 // 0xa0
#define KB_e        0x12 // 0x92
#define KB_q        0x10 // 0x90
#define KB_i        0x17 // 0x97
#define KB_k        0x25 // 0xa5
#define KB_j        0x24 // 0xa4
#define KB_l        0x26 // 0xa6
#define KB_shiftL   0x2a // 0xaa
#define KB_shiftR   0x36 // 0xb6


void game_atender_teclado(unsigned char tecla)
{
}

void print_pos(){
		
	int i;
	for(i = 0; i < 8; i++){
		print_dec(jugadorA.piratas[i].posicion_actual.x, 5, 5, 5 + i*2, 0x2 << 4);
    	print_dec(jugadorA.piratas[i].posicion_actual.y, 5, 5, 6 + i*2, 0x2 << 4);

    	print_dec(jugadorB.piratas[i].posicion_actual.x, 5, 15, 5 + i*2, 0x2 << 4);
    	print_dec(jugadorB.piratas[i].posicion_actual.y, 5, 15, 6 + i*2, 0x2 << 4);
	}


}