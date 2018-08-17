/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
  definicion de funciones del manejador de memoria
*/

#ifndef __MMU_H__
#define __MMU_H__

#include "defines.h"
#include "game.h"

#define CODIGO_BASE         0x400000

#define MAPA_BASE_FISICA    0x500000
#define MAPA_BASE_VIRTUAL   0x800000

typedef struct str_dir_paginas{
	unsigned char   p:1;
    unsigned char   rw:1;
    unsigned char   us:1;
    unsigned char   pwt:1;
    unsigned char   pcd:1;
    unsigned char   a:1;
    unsigned char   i:1;
    unsigned char   ps:1;
    unsigned char   g:1;
    unsigned char   disp:3;
    unsigned int base:20;
}__attribute__((__packed__, aligned (4))) dir_paginas;

typedef struct str_tabla_paginas{
	unsigned char   p:1;
    unsigned char   rw:1;
    unsigned char   us:1;
    unsigned char   pwt:1;
    unsigned char   pcd:1;
    unsigned char   a:1;
    unsigned char   d:1;
    unsigned char   pat:1;
    unsigned char   g:1;
    unsigned char   disp:3;
    unsigned int base:20;
}__attribute__((__packed__, aligned (4))) tabla_paginas;

void mmu_inicializar();
unsigned int mmu_inicializar_dir_pirata(jugador_t* jug, bool esMinero, coordenada_t posicion_pirata);
void copiar_pagina(uint* dir_destino, uint* dir_fuente);
void mmu_inicializar_dir_kernel();
void mmu_inicializar_page_table(uint dir);
void mmu_inicializar_page_dir(unsigned int direccion);
unsigned int dame_pagina_libre();
void mmu_mappear_pagina(unsigned int virtual, unsigned int dir_pd, unsigned int fisica, unsigned char rw, unsigned char us);
void mmu_unmappear_pagina(unsigned int virtual, unsigned int cr3);
void prueba();
#endif	/* !__MMU_H__ */
