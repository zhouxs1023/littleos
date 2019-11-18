;********************************************
;
;   Hopefully fool-proof A20 Line set
;
;       Carry Flag is ON if error!
;
;********************************************

enablea20kbwait:                        ; wait for safe to write to 8042
        xor cx,cx
enablea20kbwaitl0:
        jmp short $+2
        jmp short $+2
        jmp short $+2
        in al,64h                       ; read 8042 status
        test al,2                       ; buffer full?
        loopnz enablea20kbwaitl0      ; if yes, loop
        ret

enablea20test:                          ; test for enabled A20
        mov al,byte [fs:0]                   ; get byte from 0:0
        mov ah,al                       ; preserve old byte
        not al                          ; modify byte
        xchg al,byte [gs:10h]                ; put modified byte to 0ffffh:10h
        cmp ah,byte [fs:0]                   ; set zero if byte at 0:0 not modified
        mov [gs:10h],al                 ; put back old byte at 0ffffh:10h
        ret                             ; return, zero if A20 enabled


EnableA20:                              ; hardware enable gate A20

        xor ax,ax                       ; set A20 test segments 0 and 0ffffh
        mov fs,ax
        dec ax
        mov gs,ax

        call enablea20test              ; is A20 already enabled?
        jz short enablea20done        ; if yes, done

        in al,92h                       ; PS/2 A20 enable
        or al,2
        jmp short $+2 ;IODELAYS
        jmp short $+2
        jmp short $+2
        out 92h,al

        call enablea20test              ; is A20 enabled?
        jz short enablea20done        ; if yes, done

        call enablea20kbwait            ; AT A20 enable
        jnz short enablea20f0

        mov al,0d1h
        out 64h,al

        call enablea20kbwait
        jnz short enablea20f0

        mov al,0dfh
        out 60h,al

        call enablea20kbwait

enablea20f0:                          ; wait for A20 to enable
        mov cx,800h                     ; do 800h tries

enablea20l0:
        call enablea20test              ; is A20 enabled?
        jz enablea20done              ; if yes, done

        in al,40h                       ; get current tick counter
        jmp short $+2
        jmp short $+2
        jmp short $+2
        in al,40h
        mov ah,al

enablea20l1:                          ; wait a single tick
        in al,40h
        jmp short $+2
        jmp short $+2
        jmp short $+2
        in al,40h
        cmp al,ah
        je enablea20l1

        loop enablea20l0              ; loop for another try
        stc ;Set Carry

enablea20done:
        clc
ret
