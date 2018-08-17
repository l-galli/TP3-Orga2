/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================

    Definiciones globales del sistema.
*/

#ifndef __DEFINES_H__
#define __DEFINES_H__

/* Tipos basicos */
/* -------------------------------------------------------------------------- */
#define NULL                    0
#define TRUE                    0x00000001
#define FALSE                   0x00000000
#define ERROR                   1

typedef enum bool_e { true = 0x1, false = 0x0} bool;
typedef unsigned char  uchar;
typedef unsigned short ushort;
typedef unsigned int   uint;

/* Constantes basicas */
/* -------------------------------------------------------------------------- */
#define PAGE_SIZE               0x00001000
#define TASK_SIZE               4096

#define BOOTSECTOR              0x00001000 /* direccion fisica de comienzo del bootsector (copiado) */
#define KERNEL                  0x00001200 /* direccion fisica de comienzo del kernel */


/* Indices en la gdt */
/* -------------------------------------------------------------------------- */
#define GDT_COUNT 31

#define GDT_IDX_NULL_DESC           0x0
#define GDT_IDX_DAT_0   		    0x8
#define GDT_IDX_DAT_3         		0x9
#define GDT_IDX_COD_0         		0xA
#define GDT_IDX_COD_3 				0xB
#define GDT_IDX_VIDEO 				0xC
#define TSS_INICIAL					0xD
#define TSS_IDLE					0xE

/* Offsets en la gdt */
/* -------------------------------------------------------------------------- */
#define GDT_OFF_NULL_DESC           (GDT_IDX_NULL_DESC      << 3)

/* Selectores de segmentos */
/* -------------------------------------------------------------------------- */
#define SEG_SEL_CODE_0				0x50 /* index-> 10(codigo 0), TI-> 0 (global), RPL-> 00(privilegios)*/

/* Manejo de memoria */
/* -------------------------------------------------------------------------- */
#define ADDRESS_PAGE_DIR     0x27000 // direccion directorios de paginas
#define ADDRESS_PAGE_TABLE   0x28000 // direccion tablas de paginas

void* error();

#define ASSERT(x) while(!(x)) {};


#endif  /* !__DEFINES_H__ */
