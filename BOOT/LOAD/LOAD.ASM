;**************************************************
;
;       PMODE OS - LOADER v1.0
;
; Loads image to location at end of 1Meg
; Sets sels 8,16(Cs:DS) to base relative to image
; sel 24 is junk
; sel 32 + 40 are 32-bit DATA + CODE (resp)
;
;
;       Written 1998 Cameron Buschardt
;       This code is part of the Little OS Project
;       and falls under the GNU license as the rest
;       of the OS
;
; SPEC: Loads OS.BIN (of any size) at location
;       0x100000, and starts it with CS_SEL=8 DATASEL=16 (you must set DS=16)
;       the base of both of those segs are 0x100000
;***************************************************
%define LOADBASE 0x110000


BITS 16
ORG 100h                ;COM FILE offset
jmp main

%include "GDTNASM.INC"  ;Easy GDT Creation
%include "A20.INC"      ;Set A20 Gate
;**** GDT ****
align 16
gdt:
        ;*ZERO DESCRIPTOR -AND- GDT DATA*
        dw      gdt_limit
        dd      gdt
        dw      0

        FLAT_DS equ 8
        FLAT_CS equ 16
        desc 0,0xFFFFF,D_DATA+D_WRITE+ D_BIG + D_BIG_LIM ;8 - FLAT DATA
        desc 0,0xFFFFF,D_CODE+D_READ + D_BIG + D_BIG_LIM ;16 - FLAT CODE

        ;       Real mode compatible c-seg(24)
        ;              used to get back to 16-bit PMODE
        rseg: desc    0,0xFFFF,D_CODE+D_READ
        KERNEL_DS equ 32
        KERNEL_CS equ 40
        desc LOADBASE,0xFFFFF-LOADBASE/4096,D_DATA+D_WRITE+ D_BIG + D_BIG_LIM ;8 - FLAT DATA
        desc LOADBASE,0xFFFFF-LOADBASE/4096,D_CODE+D_READ + D_BIG + D_BIG_LIM ;16 - FLAT CODE

gdt_limit equ $-gdt-1
align 16

;*******************
; FINIT - sets GDT
;*******************
bits 16
fmeminit:
        ;Calculate linear offset -> ebp
        xor ebp,ebp
        mov dx,cs
        mov bp,dx
        shl ebp,4
        ;Relocate Linear base addrfss of the Global Dfscriptor Table
        add [gdt+2],ebp
        ;Set desc 24 to base
        mov eax,ebp        
        mov [rseg+2],ax
        shr eax,16
        mov [rseg+4],al
        mov [rseg+7],ah
        ;*Set the 16-bit pmode exit seg (CS) of 16-bit*
        mov ax,ds ;same as cs here
        mov word [exitseg],ax
        mov ax,ds
        mov word [baseseg],ax

        xor eax,eax
        mov ax,ds
        shl eax,4
        add eax, pmcopy ;routine to call
        mov dword [fmemaddr],eax
        ;Set the GDT
        lgdt [gdt]

ret

;***************************************************************
;
;  FMEMCPY - realmode
;     EAX = 32-bit src, EBX = 32-bit dest, ECX= bytes to copy
;
;***************************************************************
bits 16
fmemcpy:
        ;**Save Values**
        mov dword [msrc],eax
        mov dword [mdest],ebx
        mov dword [mlen],ecx
        ;**GDT Set enter PMODE**
        ;Set sp pos :)
        mov ax,sp
        mov word [stackpos],ax
        ;**jump is set**
        mov eax,cr0
        or al,1
        mov cr0,eax
        ;**Now do far jump **
        jmp dword FLAT_CS:0
        fmemaddr equ $ - 6 ;adjusted by fmeminit
        pmcopyexit: ;Enter back here
        ;** BACK FROM SUCESSFULL COPY! **
        mov ax,0
                baseseg equ $ - 2
        mov ds,ax ;Restore regs
        mov es,ax
        mov ss,ax
        mov ax,0 ;Stack Pos
                stackpos equ $ - 2
        mov sp,ax 
ret

bits 16
;***************************
;  16bit PMODE ->RMODE JUMP
;***************************
pmode16retn:
        ;NOW THAT WE ARE IN AN RMODE COMPAT SEG.. turn PM=OFF
        mov eax,cr0
        and al,0FEh
        mov cr0,eax
        
        jmp word 0:pmcopyexit
        exitseg equ $ -2

;****************
;  PMCOPY helper
;****************
bits 32
pmcopy:
        mov ax,FLAT_DS
        mov ds,ax
        ;Load regs EAX = 32-bit src, EBX = 32-bit dest, ECX= bytes to copy
;        mov dword [ds:0xb8000],0x21222324
        ;Use a little trick :)
        mov esi,0
                msrc equ  $-4
        mov edi,0
                mdest equ $-4
        mov ecx,0
                mlen equ  $-4

        ;Do the copy in PMODE :)
        ;riscified loop
        pmcopy.copy:
                mov al,[ds:esi]
                mov [ds:edi],al
                inc esi
                inc edi
                dec ecx ;ZF is autoset
        jnz pmcopy.copy

        ;now done with copy jump back to real mode routine
        jmp word 24:pmode16retn

;**************************************************
;       Simple File I/O Wrappers (remembers handle)
;**************************************************
bits 16
handle dw 0
openfile:       ;SET CARRY IF FAILED :)
        ;DX Points to name 
        mov ax,0x3D00 ;open for reading
        int 21h
        mov [handle],ax
ret

readfile:       ;RET AX = read IN=AX->SIZE DX->BUFFER
        mov cx,ax
        mov bx,[handle]
        mov ah,0x3F
        int 21h
ret

;**************************************************
; TEST MODE -- will fail in windows (windows lies)
;**************************************************
bits 16
testmode:
        mov eax,cr0
        or al,al
        jnz testmodeok
                stc
                ret
        testmodeok:
                clc
                ret
;***************
; MAIN FUNCTION
;***************
bits 16
showmessage:
        mov ah,9h
        int 21h
ret
bits 16
main:
        mov dx,_LOADING
        call showmessage

       CALL testmode ;Make sure PMODE is not tainted
       jnc PureRMODE
                ;Error
                mov dx,_NOTRMODE
                jmp errorexit
       PureRMODE:
       cli ;Ints off here

       call EnableA20
       call enablea20test ;ZF = means okay
       jz EnabledA20
                ;Error
                 mov dx, _A20BAD
                 jmp errorexit
       EnabledA20:

       call fmeminit
       ;*** NOW OPEN OS BINARY ***
        mov dx,imagefile
        call openfile   ;OPENS a file an remember handle.. 
        jnc ldok
                mov dx,_FILEERR
                jmp errorexit
        ldok:
        ;calculate linear location of tbuffer
        xor eax,eax
        mov ax,ds
        shl eax,4
        add eax,tbuffer
        mov ebp,eax
        cli
        readloop:
                mov ax,8192 ;Read 8k         
                mov dx,tbuffer ;Buffer
                call readfile
                test ax,ax                ;AX is bytes read
                jz eloop
                mov eax,ebp             ;EBP is const base address of TBUFFER
                mov ebx,[_write_pos]
                mov ecx,8192
                call fmemcpy            ;DO 32-bit copy
                add dword [_write_pos],8192
        jmp readloop
        eloop:
        ;NOW FILE IS LOADED AT DESTINATION LOCATION
        ;CALL IN PMODE
        mov eax,cr0
        or al,1
        mov cr0,eax
        jmp dword KERNEL_CS:0 ;call base 0

errorexit:
        sti
        mov ah,9h
        int 21h
        int 20h
_NOTRMODE : db 'System is already in Protected-Mode, please run in a clean dos session!$'
_A20BAD   : db 'Fatal error enabling to A20 gate (cannot used memory above 1mb)$'
_FILEERR  : db 'Fatal error opening or reading from os image!$'
_LOADING  : db 'LittleOS DOS mode boot-loader v1.0',10,13,'Written 1998,Cameron Buschardt',13,10,'$'
imagefile db 'kernel.32',0
_write_pos dd LOADBASE  ;Linear position to write to (do NOT change w/o updating sels)
tbuffer: ;Temp buffer

