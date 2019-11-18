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

;
; asm.asm - some useful functions (hardware specific) used in C
;

[segment .text]
[BITS 32]

%imacro FUNC 1
  GLOBAL %1
  align 4
  %1:
%endmacro

; unsigned char get_byte(unsigned short sel, unsigned long addr)
FUNC _get_byte
	mov 	ax, [esp+4]
        mov 	gs, ax
        mov 	edx, [esp+8]
        xor 	eax, eax
        mov 	al, byte [gs:edx]
        ret

; unsigned short get_word(unsigned short sel, unsigned long addr)
FUNC _get_word
	mov 	ax, [esp+4]
        mov 	gs, ax
        mov 	edx, [esp+8]
        xor 	eax, eax
        mov 	ax, word [gs:edx]
        ret

; unsigned long get_long(unsigned short sel, unsigned long addr)
FUNC _get_long
	mov 	ax, [esp+4]
        mov 	gs, ax
        mov 	edx, [esp+8]
        mov 	eax, dword [gs:edx]
        ret

; void put_byte(unsigned short sel, unsigned long addr, unsigned char val)
FUNC _put_byte
	mov 	ax, [esp+4]
        mov 	gs, ax
        mov 	edx, [esp+8]
        mov 	al, byte [esp+12]
        mov 	[gs:edx], al
        ret

; void put_word(unsigned short sel, unsigned long addr, unsigned short val)
FUNC _put_word
	mov 	ax, [esp+4]
        mov 	gs, ax
        mov 	edx, [esp+8]
        mov 	ax, word [esp+12]
        mov 	[gs:edx], ax
        ret

; void put_long(unsigned short sel, unsigned long addr, unsigned long val)
FUNC _put_long
	mov 	ax, [esp+4]
        mov 	gs, ax
        mov 	edx, [esp+8]
        mov 	eax, [esp+12]
        mov 	[gs:edx], eax
        ret


FUNC _kb_wait
        push    eax
.L1:    in      al, 0x64
        test    al, 2
        jne     .L1
        pop     eax
        ret

