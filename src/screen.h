/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
  definicion de funciones del scheduler
*/

#ifndef __SCREEN_H__
#define __SCREEN_H__

/* Definicion de la pantalla */
#define VIDEO_FILS 50
#define VIDEO_COLS 80

#define VIDEO                   0x000B8000 /* direccion fisica del buffer de video */


#include "colors.h"
#include "defines.h"
#include "game.h"
#include "tss.h"



/* Estructura de para acceder a memoria de video */
typedef struct ca_s {
    unsigned char c;
    unsigned char a;
} ca;


struct pirata_t;
typedef struct pirata_t pirata_t;

struct jugador_t;
typedef struct jugador_t jugador_t;

int ee_printf(const char *fmt, ...);


void screen_pintar(unsigned char c, unsigned char color, uint fila, uint columna);
void print(const char * text, unsigned int x, unsigned int y, unsigned short attr);
void print_hex(unsigned int numero, int size, unsigned int x, unsigned int y, unsigned short attr);
void print_dec(uint numero, int size, uint x, uint y, unsigned short attr);
void screen_pintar_rect(unsigned char c, unsigned char color, int fila, int columna, int alto, int ancho);
void screen_pintar_linea_h(unsigned char c, unsigned char color, int fila, int columna, int ancho);
void screen_pintar_linea_v(unsigned char c, unsigned char color, int fila, int columna, int alto);
void screen_pintar_name();
void screen_inicializar();
void screen_pintar_puntajes();

void screen_actualizar_reloj_global();
//void screen_actualizar_reloj_pirata (jugador_t *j, pirata_t *pirata);
unsigned char screen_color_jugador(jugador_t *j);
unsigned char screen_caracter_pirata(unsigned int tipo);
void screen_pintar_pirata(jugador_t *j, pirata_t *pirata);
void screen_borrar_pirata(jugador_t *j, pirata_t *pirata);
void screen_pintar_reloj_pirata(jugador_t *j, pirata_t *pirata);
void screen_pintar_reloj_piratas(jugador_t *j);
void screen_pintar_relojes();

void screen_actualizar_posicion_mapa(uint x, uint y);
void screen_stop_game_show_winner(jugador_t *j);
void clear_screen();
unsigned char getFormat(unsigned char fore_color, char fore_bright, unsigned char back_color, char blink);
void screen_pintar_numeros(int comienzo);
uchar screen_color_actual(uint fila, uint columna);
void screen_descubrir_area(uint x_vieja, uint y_vieja, uint x, uint y, pirata_t* pir, jugador_t* jug, int pos_real);
void screen_actualizar();
void screen_game_over();
void screen_probar();
void screen_imprimir_todo();
void screen_actualizar_reloj_pirata(pirata_t* pir);
void screen_guardar();
void screen_limpiar();
void screen_matar_pirata(pirata_t* pir);
void screen_pintar_ganador(uint ganador);
void screen_imprimir_debug(uint edi, uint esi, uint ebp, uint otro_esp, uint ebx, uint edx, uint ecx, uint eax, uint errorCode, uint eip, uint cs, uint eflags, uint esp, uint ss);
void print_dbg_msg();
//Aux
int abs(int n);
#endif  /* !__SCREEN_H__ */
