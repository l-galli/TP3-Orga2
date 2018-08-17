/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
*/

#ifndef __GAME_H__
#define __GAME_H__

#include "defines.h"



typedef enum direccion_e { ARR = 0x4, ABA = 0x7, DER = 0xA, IZQ = 0xD} direccion;


#define MAX_CANT_PIRATAS_VIVOS           8

#define JUGADOR_A                         0
#define JUGADOR_B                         1

#define MAPA_ANCHO                       80
#define MAPA_ALTO                        44

#define EXPLORADOR 						 true
#define MINERO                           false

#define POS_INIT_A_X                      1
#define POS_INIT_A_Y                      1
#define POS_INIT_B_X         MAPA_ANCHO - 2
#define POS_INIT_B_Y          MAPA_ALTO - 2

struct jugador_t;

typedef struct coordenada_t
{
    uint x;
    uint y;
} coordenada_t;


typedef struct pirata_t
{
    uint indice_arreglo_jug; // indice en el arreglo de piratas del jugador
    struct jugador_t *jug; 
    uint indice_gdt ; 
    coordenada_t posicion_actual;
    bool tipo; // Minero = 1 exp = 0
    bool activo; 
    uint reloj;
    coordenada_t tesoro;
 
} pirata_t;


typedef struct jugador_t
{
    uint index; //0 jugA, 1 jugB
    pirata_t piratas[MAX_CANT_PIRATAS_VIVOS]; 
    char posiciones_recorridas[MAPA_ALTO*MAPA_ANCHO];
    uint cantActivos;
    uint cant_mineros;
    uint cant_exploradores;
    coordenada_t botines[3]; //Arrancan en (9999,9999)
    uint puntos; 
    uint mineros_a_lanzar;
} jugador_t;





jugador_t jugadorA;
jugador_t jugadorB;

uint gdt_pirata_actual; // Esto tiene el indice de la gdt del pirata actual que se ejecuta como tarea
uint jugador_actual; // Jugador cuyo pirata es el actual
// Jugador A = 0, B = 1



// ~ auxiliares dadas ~
uint game_xy2lineal(uint x, uint y);
pirata_t* id_pirata2pirata(uint id);

// ~ auxiliares sugeridas o requeridas (segun disponga enunciado) ~
void game_pirata_inicializar(pirata_t *pirata, jugador_t *jugador, uint index, uint id);
void game_pirata_erigir(pirata_t *pirata, jugador_t *j, uint tipo);
void game_pirata_habilitar_posicion(jugador_t *j, pirata_t *pirata, int x, int y);
void game_pirata_exploto(uint id);

void game_jugador_inicializar(jugador_t *j);
void game_jugador_lanzar_pirata(uint jug, bool tipo);
pirata_t* game_jugador_erigir_pirata(jugador_t *j, uint tipo);
void game_jugador_anotar_punto(jugador_t *j);
void game_explorar_posicion(jugador_t *jugador, int x, int y);

uint game_valor_tesoro(uint x, uint y);
void game_calcular_posiciones_vistas(int *vistas_x, int *vistas_y, int x, int y);
pirata_t* game_pirata_en_posicion(uint x, uint y);

uint game_syscall_pirata_posicion(int idx);
uint game_syscall_pirata_mover(direccion key);
uint game_syscall_manejar(uint syscall, uint param1);
void game_tick(uint id_pirata, uint jug);
void game_terminar_si_es_hora();
void game_atender_teclado(unsigned char tecla);
void game_reiniciar();

bool puedo_moverme(pirata_t* pir, direccion dir);
jugador_t* dameJugadorActual();
void resolver_tarea_explorador(pirata_t* pir, jugador_t* jug);
void game_syscall_cavar();
void matar_pirata();

bool nadie_en_puerto(uint jug);
void screen_imprimir_debug();

void mapear_a_todos(pirata_t* pir, jugador_t*  jug, uint indice);

void test_pirata();
void lanzar_pirata_test();
void print_pos();
void pintar_puerto(uint pos_puerto_x, uint pos_puerto_y, pirata_t* pir, jugador_t* jug);
bool no_lo_tome(jugador_t* jug, uint x, uint y);

#endif  /* !__GAME_H__ */
