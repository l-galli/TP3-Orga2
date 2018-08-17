/* ** por compatibilidad se omiten tildes **
================================================================================
TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
definicion de funciones del scheduler
*/

#include "screen.h"



extern jugador_t jugadorA, jugadorB;

extern uint contador_juego;

static ca (*p)[VIDEO_COLS] = (ca (*)[VIDEO_COLS]) VIDEO;

const char reloj[] = "|/-\\";

ca pantalla_mapa[VIDEO_FILS][VIDEO_COLS];

#define reloj_size 4


void screen_actualizar_reloj_global()
{
    static uint reloj_global = 0;

    reloj_global = (reloj_global + 1) % reloj_size;

    screen_pintar(reloj[reloj_global], C_BW, 49, 79);
}

void screen_actualizar_reloj_pirata(pirata_t* pir)
{
    uint reloj_pir = (pir->reloj + 1) % reloj_size;
    pir->reloj = reloj_pir;

    if(pir->jug->index == JUGADOR_A){
        screen_pintar(reloj[reloj_pir], C_BW, 48, (4 + pir->indice_arreglo_jug * 2));
    }else{
        screen_pintar(reloj[reloj_pir], C_BW, 48, (60 + pir->indice_arreglo_jug * 2));
    }

}

void screen_guardar(){
	int i,j;
    for(i = 0; i < VIDEO_FILS; i++){
        for(j = 0; j < VIDEO_COLS; j++){
            pantalla_mapa[i][j] = p[i][j];
        }
    }
}

void screen_pintar(uchar c, uchar color, uint fila, uint columna)
{
    p[fila][columna].c = c;
    p[fila][columna].a = color;

}

uchar screen_valor_actual(uint fila, uint columna)
{
    return p[fila][columna].c;
}

void print(const char * text, uint x, uint y, unsigned short attr) {
    int i;
    for (i = 0; text[i] != 0; i++)
     {
        screen_pintar(text[i], attr, y, x);

        x++;
        if (x == VIDEO_COLS) {
            x = 0;
            y++;
        }
    }
}

void print_hex(uint numero, int size, uint x, uint y, unsigned short attr) {
    int i;
    char hexa[8];
    char letras[16] = "0123456789ABCDEF";
    hexa[0] = letras[ ( numero & 0x0000000F ) >> 0  ];
    hexa[1] = letras[ ( numero & 0x000000F0 ) >> 4  ];
    hexa[2] = letras[ ( numero & 0x00000F00 ) >> 8  ];
    hexa[3] = letras[ ( numero & 0x0000F000 ) >> 12 ];
    hexa[4] = letras[ ( numero & 0x000F0000 ) >> 16 ];
    hexa[5] = letras[ ( numero & 0x00F00000 ) >> 20 ];
    hexa[6] = letras[ ( numero & 0x0F000000 ) >> 24 ];
    hexa[7] = letras[ ( numero & 0xF0000000 ) >> 28 ];
    for(i = 0; i < size; i++) {
        p[y][x + size - i - 1].c = hexa[i];
        p[y][x + size - i - 1].a = attr;

    }
}

void print_dec(uint numero, int size, uint x, uint y, unsigned short attr) {
    int i;
    char letras[16] = "0123456789";

    for(i = 0; i < size; i++) {
        int resto  = numero % 10;
        numero = numero / 10;
        p[y][x + size - i - 1].c = letras[resto];
        p[y][x + size - i - 1].a = attr;

    }
}

//Limpar la pantalla poniendo 0(null) en cada dato de componente
void screen_limpiar()
{
    int i,j;
    for (i = 0; i < VIDEO_COLS; ++i)
    {
        for (j = 0; j < VIDEO_FILS; ++j)
        {
            p[j][i].c = 0;
            p[j][i].a = 0;
        }
    }


}

void screen_inicializar()
{
    int i,j;
    int izq, der;
    screen_limpiar();
    for (i = 0; i < VIDEO_COLS; ++i)
    {
        for (j = 0; j < VIDEO_FILS - 5 ; ++j)
        {
            if(j == 0){
                screen_pintar(0, C_BG_BLACK, j, i);

            }else{
                screen_pintar(0, C_BG_LIGHT_GREY, j, i);

            }
        }
    }

    screen_pintar_rect(0, C_BG_RED, VIDEO_FILS - 5, 33, 5, 7);
    screen_pintar_rect(0, C_BG_BLUE, VIDEO_FILS - 5, 40, 5, 7);
    izq = 4;
    der = 60;
    screen_pintar_numeros(izq);
    screen_pintar_numeros(der);
    screen_pintar_rect(0, C_BG_GREEN, POS_INIT_A_Y, POS_INIT_A_X - 1, 3, 3);
    screen_pintar_rect(0, C_BG_CYAN, POS_INIT_B_Y, POS_INIT_B_X - 1, 3, 3);

    print_dec(0, 3, 35, 47, C_FG_WHITE + C_BG_RED);
    print_dec(0, 3, 42, 47, C_FG_WHITE + C_BG_BLUE);
    print("Modo Debug: OFF", 65, 0, C_BG_BLACK + C_FG_WHITE);
    screen_actualizar();
    
    
}

void screen_pintar_numeros(int comienzo){
    int i,j;
    j = 0;
    const char* text[8] = {"1", "2", "3", "4", "5", "6", "7", "8"};
    for(i = comienzo; i < comienzo + 16; i += 2){

        print(text[j], i, 46, C_FG_WHITE);
        j++;

    }
}

void screen_pintar_linea_h(unsigned char c, unsigned char color, int fila, int columna, int ancho){
	//Hago que se pueda pintar a izquierda o a derecha, dependiendo del signo de ancho.
	//Si es negativo pinta para la izquierda, si es positivo a la derecha, si es 0 no pinta nada.
	if(ancho != 0){
		bool negativo = (ancho < 0) ? true : false;
		int cuanto_pinto = 0;

		if(negativo){
			if(columna + 1 > abs(ancho)){
				cuanto_pinto = columna + 1;
			}
			else{
				cuanto_pinto = abs(ancho);
			}
		}
		else{
			if(VIDEO_COLS - columna > ancho){
				cuanto_pinto = ancho;
			}
			else{
				cuanto_pinto = VIDEO_COLS - columna;
			}
		}
		int signo = 1;
		if(negativo){
			signo *= (-1);
		}

        int desplazamiento = 0; 

        while(desplazamiento < cuanto_pinto){
            screen_pintar(0, color, (uint)fila, (uint)(columna + signo*desplazamiento));
            desplazamiento++;
        }
	}

	
}

//negativo -> pinta para abajo | positivo -> arriba
void screen_pintar_linea_v(unsigned char c, unsigned char color, int fila, int columna, int alto){
	if(alto != 0){
		bool negativo = (alto < 0) ? true : false;
		int cuanto_pinto = 0;

		if(negativo){
			if(VIDEO_FILS - fila){
				cuanto_pinto = VIDEO_FILS - fila;
			}
			else{
				cuanto_pinto = abs(alto);
			}
		}
		else{
			if(fila + 1 > alto){
				cuanto_pinto = alto;
			}
			else{
				cuanto_pinto = fila + 1;
			}
		}

		int signo = 1;
		if(negativo){
			signo *= (-1);
		}

        int desplazamiento = 0; 

        while(desplazamiento < cuanto_pinto){
            screen_pintar(0, color, (uint)(fila - signo*desplazamiento), (uint)columna);
            desplazamiento++;
        }
	}
}

void screen_pintar_rect(unsigned char c, unsigned char color, int fila, int columna, int alto, int ancho){
    int i = 0;
    while(i < alto){
        screen_pintar_linea_h(c, color, fila + i, columna, ancho);
        i++;
    }

}

uchar screen_color_actual(uint fila, uint columna)
{
    return p[fila][columna].a;
}

void screen_pintar_name(){
    uchar color_actual = screen_color_actual(0, VIDEO_COLS - 10);
    color_actual &= 0xF8;
    print("Itanium 2", VIDEO_COLS - 10, 1, color_actual);
}

void screen_descubrir_area(uint x_vieja, uint y_vieja, uint x, uint y, pirata_t* pir, jugador_t* jug, int pos_real){
    
    if(y < 45 && y_vieja < 45){
    

        if(jug->index == JUGADOR_A){
            if(y > 0){
                screen_pintar(p[y][x].c, C_FG_WHITE + C_BG_GREEN, y, x);
            }
           

            if(pir->posicion_actual.x == x && pir->posicion_actual.y == y){
                if(pir->tipo == 1){
                    //minero
                    if(pos_real && y < 44 && y_vieja < 44){
    	                screen_pintar('M', C_FG_WHITE + C_BG_GREEN, y_vieja + 1, x_vieja);
    	                screen_pintar('M', C_FG_WHITE + C_BG_RED, y + 1, x);
                	}else{
                        
                        ca temp = p[y][x];
                    	screen_pintar(temp.c, C_FG_WHITE + C_BG_GREEN, y, x);
                	    
                    }
                }else{
                	if(pos_real && y < 44 && y_vieja < 44){
    	                screen_pintar('E', C_FG_WHITE + C_BG_GREEN, y_vieja + 1, x_vieja);
    	                screen_pintar('E', C_FG_WHITE + C_BG_RED, y + 1, x);
                	}else{
                        ca temp = p[y][x];
                    	screen_pintar(temp.c, C_FG_WHITE + C_BG_GREEN, y, x);
                	    
                    }
                }
            }

        }else{
            if(y > 0){
                screen_pintar(p[y][x].c, C_FG_WHITE + C_BG_CYAN, y, x);
            }

            if(pir->posicion_actual.x == x && pir->posicion_actual.y == y){
                if(pir->tipo == 1){
                    //minero
                    if(pos_real && y < 44 && y_vieja < 44){
    	                screen_pintar('M', C_FG_WHITE + C_BG_CYAN, y_vieja + 1, x_vieja);
    	                screen_pintar('M', C_FG_WHITE + C_BG_RED, y + 1, x);
                	}else{
                        
                        ca temp = p[y][x];
                		screen_pintar(temp.c, C_FG_WHITE + C_BG_CYAN, y, x);
                	    
                    }
                }else{
                	if(pos_real && y < 44 && y_vieja < 44){
    	                screen_pintar('E', C_FG_WHITE + C_BG_CYAN, y_vieja + 1, x_vieja);
    	                screen_pintar('E', C_FG_WHITE + C_BG_RED, y + 1, x);
                	}else{
                        ca temp = p[y][x];
                		screen_pintar(temp.c, C_FG_WHITE + C_BG_CYAN, y, x);
                	    
                    }
                }
            }
        }
    

    }
    if(game_valor_tesoro(x, y - 1)){
        screen_pintar('o', C_BG_GREEN, y, x);
    }
}

//esta tiene que correr al final de cada tick
void screen_actualizar(){
    int i = 0;
    jugador_t* jugA = &jugadorA;
    jugador_t* jugB = &jugadorB;

    //reloj o cruces
    while(i < 16){
        if(i < 8){
            if(!jugA->piratas[i].activo){
                screen_pintar('X', C_FG_RED + C_BG_BLACK, 48, (4 + i * 2));
            }else{
                bool tipo = jugA->piratas[i].tipo;
                coordenada_t pos = jugA->piratas[i].posicion_actual;
                if(tipo){
                    screen_pintar('M', C_BG_RED + C_FG_WHITE, pos.y + 1, pos.x);
                }else{
                    screen_pintar('E', C_BG_RED + C_FG_WHITE, pos.y + 1, pos.x);
                }
                screen_actualizar_reloj_pirata(&(jugA->piratas[i]));
            }
        }else{
            if(!jugB->piratas[i % 8].activo){
                screen_pintar('X', C_FG_RED + C_BG_BLACK, 48, (60 + (i % 8) * 2));
            }else{
                bool tipo = jugB->piratas[i % 8].tipo;
                coordenada_t pos = jugB->piratas[i % 8].posicion_actual;
                if(tipo){
                    screen_pintar('M', C_BG_RED + C_FG_WHITE, pos.y + 1, pos.x);
                }else{
                    screen_pintar('E', C_BG_RED + C_FG_WHITE, pos.y + 1, pos.x);
                }
                screen_actualizar_reloj_pirata(&(jugB->piratas[i % 8]));
            }
        }
        i++;
    }

    //contador de juego
    print_dec(contador_juego, 3, 37, 0, C_FG_WHITE + C_BG_BLACK);
    //puntos
    print_dec((int) jugA->puntos, 3, 35, 47, C_FG_WHITE + C_BG_RED);
    print_dec((int) jugB->puntos, 3, 42, 47, C_FG_WHITE + C_BG_BLUE);
}

void screen_game_over(){
    int i,j;
    for (i = 0; i < VIDEO_COLS; ++i)
    {
        for (j = 0; j < VIDEO_FILS - 5 ; ++j)
        {
            screen_pintar(0, C_BG_BLACK, j, i);
            
        }
    }

    print( "GAME", 37, 21, C_FG_WHITE + C_BG_BLACK);
    print( "OVER", 37, 23, C_FG_WHITE + C_BG_BLACK);


}


void screen_pintar_ganador(uint ganador){
   
    if(ganador == JUGADOR_A){
        print( "GANADOR: ", 27, 5, C_FG_WHITE + C_BG_BLACK);
        print( "A", 37, 5, C_FG_WHITE + C_BG_BLACK);
    }
    else if(ganador == JUGADOR_B){
        print( "GANADOR: ", 27, 5, C_FG_WHITE + C_BG_BLACK);
        print( "B", 37, 5, C_FG_WHITE + C_BG_BLACK);
    }
    else{
        print( "EMPATE", 35, 7, C_FG_WHITE + C_BG_BLACK);
    }

}


//imprime la matriz que tiene la info de la pantalla
void screen_imprimir_todo(){
    int i,j;

    for(i = 0; i < VIDEO_FILS; i++){
        for(j = 0; j < VIDEO_COLS; j++){
            screen_pintar(pantalla_mapa[i][j].c, pantalla_mapa[i][j].a, i, j);
        }
    }
}


void screen_imprimir_debug(uint edi, uint esi, uint ebp, uint otro_esp, uint ebx, uint edx, uint ecx, uint eax, uint errorCode, uint eip, uint cs, uint eflags, uint esp, uint ss)
{
	
    screen_guardar();
 

	screen_pintar_linea_h(0, C_BG_BLACK, 5, 25, 29);
	screen_pintar_linea_h(0, C_BG_BLACK, 42, 25, 29);

	screen_pintar_linea_v(0, C_BG_BLACK, 42, 25, 38);
	screen_pintar_linea_v(0, C_BG_BLACK, 42, 54, 38);

	screen_pintar_linea_h(0, C_BG_RED, 6, 26, 28);
	screen_pintar_rect(0, C_BG_LIGHT_GREY, 7, 26, 41-6, 28);

	tss* pirata;
	pirata_t* pir = id_pirata2pirata(gdt_pirata_actual);
    
    if (jugador_actual == JUGADOR_A){
        pirata = &(tss_jugadorA[pir->indice_arreglo_jug]);
    }else{
        pirata = &(tss_jugadorB[pir->indice_arreglo_jug]);
	}


	char* col1[16] = {"eax", "ebx", "ecx", "edx", "esi", "edi", "ebp", "esp", "eip", "cs", "ds", "es", "fs", "gs", "ss", "eflags"};
	char* col2[5] = {"cr0", "cr2", "cr3", "cr4", "stack"};

	uint ds = pirata->ds;
	uint es = pirata->es;
	uint fs = pirata->fs;
	uint gs = pirata->gs;

	uint cr0 = rcr0();
	uint cr2 = rcr2();
	uint cr3 = pirata->cr3;
	uint cr4 = rcr4();

	
                                                        //otro_esp
	uint col1v[16] = {eax, ebx, ecx, edx, esi, edi, ebp, esp, eip, cs, ds, es, fs, gs, ss, eflags};
	uint col2v[4] = {cr0, cr2, cr3, cr4};

	int i = 0;
	int j = 0;
	for(i = 0; i < 15; i++){
		if(i < 9){
			print(col1[i], 27, 8 + i*2, C_FG_BLACK + C_BG_LIGHT_GREY);
			print_hex(col1v[i], 8, 31, 8 + i*2, C_FG_WHITE + C_BG_LIGHT_GREY);
		}else{
			print(col1[i], 28, 8 + i*2, C_FG_BLACK + C_BG_LIGHT_GREY);
			print_hex(col1v[i], 4, 31, 8 + i*2, C_FG_WHITE + C_BG_LIGHT_GREY);
		}
	
	}

	//eflags
	print(col1[i], 28, 8 + i*2, C_FG_BLACK + C_BG_LIGHT_GREY);
	print_hex(col1v[i], 8, 31, 8 + i*2, C_FG_WHITE + C_BG_LIGHT_GREY);

	for(j = 0; j < 5; j++){
		if(j < 4){
			print(col2[j], 41, 8 + j*2, C_FG_BLACK + C_BG_LIGHT_GREY);
			print_hex(col2v[j], 8, 45, 8 + j*2, C_FG_WHITE + C_BG_LIGHT_GREY);
		}else{
			print(col2[j], 41, 27, C_FG_BLACK + C_BG_LIGHT_GREY);
		}
		
	}

	int reg;
	uint* stack = (uint*) esp;

	reg = *stack;
    print_hex(reg, 8, 41, 29, C_BG_LIGHT_GREY + C_FG_WHITE);

    stack++;
    reg = *stack;
    print_hex(reg, 8, 41, 30, C_BG_LIGHT_GREY + C_FG_WHITE);

    stack++;
    reg = *stack;
    print_hex(reg, 8, 41, 31, C_BG_LIGHT_GREY + C_FG_WHITE);

	
}


void screen_matar_pirata(pirata_t* pir){
    p[pir->posicion_actual.y + 1][pir->posicion_actual.x].a = C_BG_BLACK + C_FG_WHITE;
}

void print_dbg_msg(){
    print("Modo Debug: ON ", 65, 0, C_BG_BLACK + C_FG_WHITE);
}

//AUX ------------------------------

int abs(int n){
    int res;

    if(n < 0){
        res = -n;
    }else{res = n;}

    return res;
}

void screen_probar(){
    int i = 0;

    while(i < 16){
        if(i < 8){
            screen_pintar('X', C_FG_RED + C_BG_BLACK, 48, (4 + i * 2));
          
        }else{
            screen_pintar('X', C_FG_RED + C_BG_BLACK, 48, (60 + (i % 8) * 2));
          
        }
        i++;
    }
}