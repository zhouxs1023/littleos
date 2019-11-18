;
; LittleOS
; Copyright (C) 1998 Lacroix Pascal (placr@mygale.org)
;
; This program is free software; you can redistribute it and/or modify
; it under the terms of the GNU General Public License as published by
; the Free Software Foundation; either version 2 of the License, or
; (at your option) any later version.
;
; This program is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
; GNU General Public License for more details.
;
; You should have received a copy of the GNU General Public License
; along with this program; if not, write to the Free Software
; Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
;

; this need to be preprocessed by cpp
#include <gdt.h>
; must be the same as in sys/traps.h
#define TRAP_STOP_EXECUTION 1
#define TRAP_CONTINUE_EXECUTION 0


%imacro ENTER_KERNEL 0
	push 	dword ds
        push 	dword es
        pushad
        mov	ax, KERNEL_DS
        mov	ds, ax
        mov 	es, ax
%endmacro

%imacro LEAVE_KERNEL 0
	popad
        pop	dword es
        pop	dword ds
%endmacro

[SECTION .text]
[BITS 32]

%imacro IRQ 2
GLOBAL _irq%1
EXTERN _proc_irq%1
EXTERN _num_irq%1

align 4
_irq%1:
	push 	dword ds
        push 	dword es
        pushad
        mov	ax, KERNEL_DS
        mov	ds, ax
        mov	es, ax
        cmp 	dword [_proc_irq%1], 0
        je	.exit
        mov	eax, [_proc_irq%1]
        push	dword %1
        call	eax
        add	esp, 4
        inc	dword [_num_irq%1]
.exit:
	mov	al, 0x20
        out	%2, al
        popad
        pop 	dword es
        pop 	dword ds
        iret
%endmacro

IRQ 0, 0x20
IRQ 1, 0x20
IRQ 2, 0x20
IRQ 3, 0x20
IRQ 4, 0x20
IRQ 5, 0x20
IRQ 6, 0x20
IRQ 7, 0x20
IRQ 8, 0xA0
IRQ 9, 0xA0
IRQ 10, 0xA0
IRQ 11, 0xA0
IRQ 12, 0xA0
IRQ 13, 0xA0
IRQ 14, 0xA0
IRQ 15, 0xA0

;-- Exceptions --

%imacro DO_TRAP_NO_ERR 2
GLOBAL _do_%1
align 4
_do_%1: \
	push dword 0
        push dword %2
        jmp all_traps
%endmacro

%imacro DO_TRAP_ERR 2
GLOBAL _do_%1
align 4
_do_%1:
	push dword %2
        jmp all_traps
%endmacro

DO_TRAP_NO_ERR divide, 0
DO_TRAP_NO_ERR debug, 1
DO_TRAP_NO_ERR nmi, 2
DO_TRAP_NO_ERR int3, 3
DO_TRAP_NO_ERR overflow, 4
DO_TRAP_NO_ERR bounds, 5
DO_TRAP_NO_ERR invalid_op, 6
DO_TRAP_NO_ERR fpu_not_avail, 7
DO_TRAP_NO_ERR double_fault, 8
DO_TRAP_NO_ERR hardware_error, 9
DO_TRAP_ERR invalid_TSS, 10
DO_TRAP_ERR segment_not_present, 11
DO_TRAP_ERR stack_segment, 12
DO_TRAP_ERR general_protection, 13
DO_TRAP_ERR page_fault, 14
DO_TRAP_NO_ERR fpu_error, 16
DO_TRAP_NO_ERR reserved, 17

EXTERN _trap_handler

align 4
all_traps:
	pusha
        push 	dword ds
        push 	dword es
        push 	dword fs
        push 	dword gs
        mov 	ax, KERNEL_DS
        mov	ds, ax
        mov	es, ax
        push	esp
        call	_trap_handler
        pop	ebx
        cmp	eax, TRAP_CONTINUE_EXECUTION
        jne 	error_panic
	pop	dword fs
        pop	dword fs
        pop	dword es
        pop	dword ds
        popa
        add	esp, 2*4
        iret

EXTERN _trap_panic
error_panic:
        mov 	ax, KERNEL_DS
        mov	ds, ax
        mov	es, ax
        mov	eax, esp
        push	eax
        call	_trap_panic

        cli
        jmp	$

