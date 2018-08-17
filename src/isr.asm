; ** por compatibilidad se omiten tildes **
; ==============================================================================
; TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
; ==============================================================================
; definicion de rutinas de atencion de interrupciones
extern fin_intr_pic1
extern game_tick
extern game_reiniciar
extern screen_actualizar_reloj_global
extern game_jugador_lanzar_pirata
extern game_syscall_pirata_mover
extern game_syscall_cavar
extern game_syscall_pirata_posicion
extern screen_guardar
extern screen_imprimir_debug
extern screen_imprimir_todo
extern screen_game_over
extern print_dbg_msg
;extern test_imp
%include "imprimir.mac"

BITS 32

; Mensajes de error

isr_msg_0 db '#DE: Error de division'
isr_msg_0_len equ $ - isr_msg_0

isr_msg_1 db '#DB: Reservado'
isr_msg_1_len equ $ - isr_msg_1

isr_msg_2 db 'Interrupcion NMI'
isr_msg_2_len equ $ - isr_msg_2

isr_msg_3 db '#BP: Breakpoint'
isr_msg_3_len equ $ - isr_msg_3

isr_msg_4 db '#OF: Overflow'
isr_msg_4_len equ $ - isr_msg_4

isr_msg_5 db '#BR: BOUND Range Exceeded'
isr_msg_5_len equ $ - isr_msg_5

isr_msg_6 db '#UD: Invalid Opcode (Undefined Opcode)'
isr_msg_6_len equ $ - isr_msg_6

isr_msg_7 db '#NM: Device Not Available (No Math Coprocessor)'
isr_msg_7_len equ $ - isr_msg_7

isr_msg_8 db '#DF: Double Fault'
isr_msg_8_len equ $ - isr_msg_8

isr_msg_9 db 'Coprocessor Segment Overrun (reserved)'
isr_msg_9_len equ $ - isr_msg_9

isr_msg_10 db '#TS: Invalid TSS'
isr_msg_10_len equ $ - isr_msg_10

isr_msg_11 db '#NP: Segment Not Present'
isr_msg_11_len equ $ - isr_msg_11

isr_msg_12 db '#SS: Stack-Segment Fault'
isr_msg_12_len equ $ - isr_msg_12

isr_msg_13 db '#GP: General Protection'
isr_msg_13_len equ $ - isr_msg_13

isr_msg_14 db '#PF: Page Fault'
isr_msg_14_len equ $ - isr_msg_14

isr_msg_15 db 'Intel Reserved'
isr_msg_15_len equ $ - isr_msg_15

isr_msg_16 db '#MF: x87 FPU Floating-Point Error (Math Fault)'
isr_msg_16_len equ $ - isr_msg_16

isr_msg_17 db '#AC: Alignament Check'
isr_msg_17_len equ $ - isr_msg_17

isr_msg_18 db '#MC: Machine Check'
isr_msg_18_len equ $ - isr_msg_18

isr_msg_19 db '#XM: SIMD Floating-Point Exception'
isr_msg_19_len equ $ - isr_msg_19

; Datos sched 

sched_tarea_offset:     dd 0x00
sched_tarea_selector:   dw 0x00


; Variables debug

modo_debug: db 0x0
pantalla_debug: db 0x0

;; PIC
extern fin_intr_pic1

;; Sched
extern sched_tick
extern sched_tarea_actual

;; game
extern matar_pirata
;extern estado_juego

;;
;; Definición de MACROS
;; -------------------------------------------------------------------------- ;;

%macro ISR 3
global _isr%1

_isr%1:
	
	mov eax, %1

    pushad
    cli
   
    
    

    cmp byte [modo_debug], 1
    jne .fin

    .debug:
    	
    	mov byte [pantalla_debug], 1
    	call screen_imprimir_debug
    	xor eax, eax

    

    .ciclo:
    	in al, 0x60
    	cmp al, 0x95
    	jne .ciclo
    	
    	;xchg bx, bx
    	
    	call screen_imprimir_todo
    	mov byte [pantalla_debug], 0

    .fin:
    	call matar_pirata
    	;; Ahora tengo que saltar a la idle
    	;; Idle -> 0xE -> 1110 -> 1110 000 -> 111 0000 -> 0x70

    	mov ax, 0x70

    	str cx
		cmp ax, cx
		je .no_hago_nada


		mov [sched_tarea_selector], ax

		jmp far [sched_tarea_offset]


.no_hago_nada:




    sti
    popad
iret

%endmacro

;;
;; Datos
;; -------------------------------------------------------------------------- ;;

; Scheduler

;;
;; Rutina de atención de las EXCEPCIONES
;; -------------------------------------------------------------------------- ;;
ISR 0, isr_msg_0, isr_msg_0_len
ISR 1, isr_msg_1, isr_msg_1_len
ISR 2, isr_msg_2, isr_msg_2_len
ISR 3, isr_msg_3, isr_msg_3_len
ISR 4, isr_msg_4, isr_msg_4_len
ISR 5, isr_msg_5, isr_msg_5_len
ISR 6, isr_msg_6, isr_msg_6_len
ISR 7, isr_msg_7, isr_msg_7_len
ISR 8, isr_msg_8, isr_msg_8_len
ISR 9, isr_msg_9, isr_msg_9_len
ISR 10, isr_msg_10, isr_msg_10_len
ISR 11, isr_msg_11, isr_msg_11_len
ISR 12, isr_msg_12, isr_msg_12_len
ISR 13, isr_msg_13, isr_msg_13_len
ISR 14, isr_msg_14, isr_msg_14_len
ISR 15, isr_msg_15, isr_msg_15_len
ISR 16, isr_msg_16, isr_msg_16_len
ISR 17, isr_msg_17, isr_msg_17_len
ISR 18, isr_msg_18, isr_msg_18_len
ISR 19, isr_msg_19, isr_msg_19_len


;;
;; Rutina de atención del RELOJ
;; -------------------------------------------------------------------------- ;;

global _isr32
_isr32:
pushad
cli

call fin_intr_pic1
cmp byte [pantalla_debug], 1
je .no_hago_nada

call screen_actualizar_reloj_global

call sched_tick

xor ecx, ecx
str cx

cmp ax, cx

je .no_hago_nada



mov [sched_tarea_selector], ax

jmp far [sched_tarea_offset]


.no_hago_nada:




sti
popad

iret


;;
;; Rutina de atención del TECLADO
;; -------------------------------------------------------------------------- ;;

global _isr33
_isr33:
pushad

;xchg bx, bx

call fin_intr_pic1

cmp byte [pantalla_debug], 1
je .fin_tecla

in al, 0x60 ; leemos la tecla


cmp al, 0x95
je .activa_debug

;cmp dword [estado_juego], 0 	
;je .Reinicio

mov edx, 0 	; recordar que 1 = true = minero
			; solo puedo largar exploradores de aca

cmp al, 0x2A; soltar lshift 0x2A + 0x80 = 0xAA
je .LSHIFT
cmp al, 0x36; solar rshift 0x36 + 0x80 = 0xB6
je .RSHIFT
jmp .fin_tecla


.activa_debug:
	mov byte [modo_debug], 1
	call print_dbg_msg

	jmp .fin_tecla




.LSHIFT:

	xor ebx, ebx ; ebx -> jugadorA -> 0

	push edx ; pusheo bool primero
	push ebx ; pusheo el jugador

	
	call game_jugador_lanzar_pirata

	pop ebx
	pop edx

	jmp .fin_tecla


.RSHIFT:

	mov ebx, 1 ; ebx -> jugadorB -> 1

	push edx ; pusheo parametros de derecha a izquierda (el bool primero)
	push ebx ; mando jugador


	call game_jugador_lanzar_pirata

	pop ebx
	pop edx

	

	jmp .fin_tecla

; Opcional
; Quisimos implementar un reinicio (funcionalidad extra), 
; pero al no llegar con el tiempo queda comentado

; .Reinicio:

; 	cmp al, 0x13
; 	jne .fin_tecla
; 	call game_reiniciar	

; 	jmp .fin_tecla




.fin_tecla:
	

	popad

iret



;; int de software


global _isr0x46
_isr0x46:
pushad


cli


cmp eax, 0x1
je .moverse
cmp eax, 0x2
je .cavar
cmp eax, 0x3
je .posicion

jmp .fin

.moverse:
	push ecx



	call game_syscall_pirata_mover
	
	pop ecx
	
	

	jmp .fin
.cavar:

	;; si me llaman de aca, es un pirata minero

	call game_syscall_cavar

	jmp .fin
.posicion:
	
	push ecx


	call game_syscall_pirata_posicion
	
	pop ecx



	mov [esp + 28], eax


	jmp .fin

.fin:

	;; Hay que saltar a idle:

	;; Idle -> 0xE -> 1110 -> 1110 000 -> 111 0000 -> 0x70

   	mov ax, 0x70

   	str dx
	cmp ax, dx
	je .no_hago_nada

	mov [sched_tarea_selector], ax
	
	jmp far [sched_tarea_offset]


.no_hago_nada:


sti
popad
iret
