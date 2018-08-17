; ** por compatibilidad se omiten tildes **
; ==============================================================================
; TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
; ==============================================================================


; GDT
extern gdt_descriptor
extern GDT_DESC
extern setear_gdt_pirata

; Interrupciones
extern idt_inicializar
extern idt_descriptor
extern IDT_DESC
extern resetear_pic
extern habilitar_pic

; Pantalla
extern screen_inicializar
extern screen_limpiar
extern screen_pintar_name

; Manejo de memoria
extern mmu_inicializar
extern mmu_inicializar_dir_kernel
extern prueba

; TSS
extern tss_inicializar
extern tss_inicializar_idle
extern rellenar_TSS
extern rellenar_TSS_A
extern prueba_jugA

; Scheduler
extern inicializar_estructuras_sched


extern test


; Game
extern game_inicializar
extern test_pirata

%include "imprimir.mac"


offset: dd 0
selector: dw 0


global start


;; Saltear seccion de datos
jmp start

;;
;; Seccion de datos.
;; -------------------------------------------------------------------------- ;;
iniciando_mr_msg db     'Iniciando kernel (Modo Real)...'
iniciando_mr_len equ    $ - iniciando_mr_msg

iniciando_mp_msg db     'Iniciando kernel (Modo Protegido)...'
iniciando_mp_len equ    $ - iniciando_mp_msg

;;
;; Seccion de código.
;; -------------------------------------------------------------------------- ;;

;; Punto de entrada del kernel.
BITS 16
start:
    ; Deshabilitar interrupciones
    cli

    ; Cambiar modo de video a 80 X 50
    mov ax, 0003h
    int 10h ; set mode 03h
    xor bx, bx
    mov ax, 1112h
    int 10h ; load 8x8 font

    ; Imprimir mensaje de bienvenida
    imprimir_texto_mr iniciando_mr_msg, iniciando_mr_len, 0x07, 0, 0


    ; Habilitar A20
    
    call habilitar_A20

    ; Cargar la GDT
    lgdt [GDT_DESC]


    ; Setear el bit PE del registro CR0
    mov eax, cr0
    or eax, 1
    mov cr0, eax

    ; Saltar a modo protegido

    jmp 0x50:modoprotegido

BITS 32
modoprotegido:
    cli             
    				;indice del segmento en la gdt     0 si es gdt      nivel de privilegio
    mov ax, 0x40    ; 0...01000                      | 0             |  00 -> 0x40 (arrancan en 8 (ver define.h) -> 1000)
    mov es, ax
    mov ss, ax
    mov gs, ax
    mov ds, ax
    mov ax, 0x60    ; 0... 0110 | 000 -> 0x60 (arranca en C (12) (ver define.h) -> 1100)
    mov fs, ax

    ; Establecer la base de la pila
    mov eax, 0x27000
    mov ebp, eax 
    mov esp, eax

    ; Limpiar pantalla
    call screen_limpiar    
    
    

    ; Imprimir mensaje de bienvenida 

    ;call screen_pintar_name (ejercicio temporal)

    ; Inicializar el juego
    
    call game_inicializar
    

    ; Inicializar pantalla
    
    call screen_inicializar

    ; Inicializar el manejador de memoria

    call mmu_inicializar

    ; Inicializar el directorio de paginas

    call mmu_inicializar_dir_kernel

    ; Cargar directorio de páginas
    mov eax, 0x27000
    mov cr3, eax

    ; Habilitar paginacion

    mov eax, cr0
    or eax, 0x80000000 ; seteo el bit 0 de cr0 
    mov cr0, eax	   ; habilito paginacion

   

    ; Inicializar tss
    call tss_inicializar

    ; Inicializar el scheduler
    call inicializar_estructuras_sched

    ; Inicializar la IDT

    call idt_inicializar

    ; Cargar IDT

    lidt [IDT_DESC]
    
    ; Configurar controlador de interrupciones

    call resetear_pic
    call habilitar_pic
    
    ; Cargar tarea inicial
    mov ax, 0x68 
    ltr ax

    ; Habilitar interrupciones
    sti

   
    ; Saltar a la primera tarea: Idle

    mov ax, 0x70
    mov [selector], ax
    jmp far [offset]


    ; Ciclar infinitamente (por si algo sale mal...)

    mov eax, 0xFFFF
    mov ebx, 0xFFFF
    mov ecx, 0xFFFF
    mov edx, 0xFFFF
    jmp $
    jmp $

;; -------------------------------------------------------------------------- ;;

%include "a20.asm"
