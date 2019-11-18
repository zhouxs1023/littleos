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

; Some code comes from ReactOS by David Welch (boot0001.zip)

; This kernel should be loaded to 0x110000 (descriptor base=0, but real addr = 0x110000)

GDT_SZ		equ 256
VIRTUAL_ADDRESS equ 0x40000000
ID_MAP_BASE	equ 0xD0000000

%include "gdtnasm.inc"

[section .text]
[BITS 32]
GLOBAL start

extern _edata, _end

align 4
start:	
        mov ax,  32 ;DS
        mov ds,  ax

        ; this must be called before paging is set !
        call    count_memory

	call 	setup_paging
        
        lgdt 	[gdt_descr]
 
        mov	eax, cr0
        or	eax, 0x80000000
        mov	cr0, eax

        jmp KERNEL_CS:.next
.next:

        ; set good registers
	mov	ax, KERNEL_DS
        mov	ds, ax
        mov 	es, ax
        mov	ss, ax
        mov	esp, kernel_stack

	lidt 	[idt_descr]

        push	dword 0
        popfd

        xor	eax, eax
        mov	fs, ax
        mov	gs, ax

        mov	edi, _edata
        mov	ecx, _end
        sub	ecx, edi
        cld
        rep	stosb

        push	eax ; eax = 0
        push	eax
        push	eax

        ; call kernel main (in C)
        call 	_kernel_main

        push    dword StopMsg
        call    _printk
        add     esp, 4

        ; endless loop
GLOBAL _die
align 4
_die:
	hlt
        jmp 	_die


StopMsg: db 10, "System is halted.", 0

BASE    equ     0x110000        ; real base (without paging)

count_memory:
	; base of DS and ES is BASE=0x110000
        mov     esi, end

        ; round and add a page
        shr     esi, 12
        inc     esi
        shl     esi, 12

.loop:
	mov	eax, [esi]
        mov	dword [esi], 0xAABBCCDD
        cmp	dword [esi], 0xAABBCCDD
        jne	.stop
        ; restore data
        mov	[esi], eax
        add	esi, 4096
        jmp 	.loop
.stop:
	add	esi, BASE

        mov	[_total_memory], esi
        ret

setup_paging:
	; first page: low memory
        mov 	eax, _lowmem_page_table
        add 	eax, BASE + 7
        mov 	[_page_directory + 0], eax

        ; also maps low memory at 0xD0000000
        mov	[_page_directory + (ID_MAP_BASE/(1024*1024)) ], eax

 	; second page: the kernel
        mov 	eax, _system_page_table
        add 	eax, BASE + 7
        mov 	[_page_directory + (VIRTUAL_ADDRESS/(1024*1024)) ], eax

        ; setup low memory page table
        xor 	ebx, ebx
.L1:
        mov	eax, ebx
        shl	eax, 12		; eax = ebx * 4096
        add	eax, 7		; user, rw, present
        mov	[_lowmem_page_table + ebx*4], eax
        inc	ebx
        cmp	ebx, 1024
        jl	.L1

        ; setup kernel page table
        mov	eax, 7
        xor	edi, edi
.L2:
        mov	edx, eax
        add	edx, BASE
        mov	[_system_page_table + edi], edx
        add	edi, 4
        add	eax, 0x1000
        cmp 	eax, end
        jl	.L2

        mov	eax, _page_directory + BASE
        mov	cr3, eax

	ret


align 4096	; align to another page (2nd)

GLOBAL _lowmem_page_table
_lowmem_page_table: times 4096 db 0

align 4096	; new page
GLOBAL _system_page_table
_system_page_table: times 4096 db 0

align 4096	; new page
GLOBAL _page_directory
_page_directory: times 4096 db 0

align 4096	; another page

GLOBAL _gdt
_gdt:
; Null descriptor - base = 0x00000000 limit = 0x00000000 ( 0.0 Mb)
	dw 0x0000, 0x0000
	dw 0x0000, 0x0000
ZERO_DS equ 0x08
        desc 0,0xFFFFF,D_DATA+D_WRITE+ D_BIG + D_BIG_LIM ;8 - FLAT DATA

KERNEL_CS equ 0x10
        desc VIRTUAL_ADDRESS, 0xFFFFF-VIRTUAL_ADDRESS/4096, D_CODE + D_READ + D_BIG + D_BIG_LIM

KERNEL_DS equ 0x18
        desc VIRTUAL_ADDRESS, 0xFFFFF-VIRTUAL_ADDRESS/4096, D_DATA + D_WRITE+ D_BIG + D_BIG_LIM

        ; space for TSS and LDT...
	times (GDT_SZ-4)*8 db 0

        ; GDT should take 2Kb
GLOBAL _idt
_idt:
	times 256*8 db 0

        ; IDT 2Kb

align 8
gdt_descr:
	dw GDT_SZ*8 - 1
        dd _gdt+BASE

align 8
idt_descr:
	dw 256*8 - 1
        dd _gdt+VIRTUAL_ADDRESS


GLOBAL _virtual_base
align 4
_virtual_base: dd VIRTUAL_ADDRESS

GLOBAL _id_map
align 4
_id_map: dd (ID_MAP_BASE - VIRTUAL_ADDRESS)

GLOBAL _phys_base
align 4
_phys_base: dd BASE

GLOBAL _total_memory
align 4
_total_memory: dd 0

; -- Kernel stack ----------
[section .bss]
align 16
ks: resb 4*1024*4
kernel_stack:
align 16

; -- Externals ----------
extern _kernel_main
extern _printk

extern end

