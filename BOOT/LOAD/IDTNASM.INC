;       TYPES:
;
;               IDT_INT
;               IDT_TASK
;               IDT_TRAP



;       Sel,Offset,DPL,flags
I_PRESENT EQU (1 << 15)
I_GATE16  EQU (0 << 11)
I_GATE32  EQU (1 << 11)


%macro idt_int 4
        dw ($2 & 0xFFFF)
        dw $1
        dw I_PRESENT + $4 + (($3 << 5) + 110b) << 8
        dw $2 >> 16
%endmacro
