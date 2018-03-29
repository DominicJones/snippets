        ;; yasm -f elf64 -g dwarf2 -l memory.lst memory.asm
        ;; gcc -o memory memory.o

        segment .data
        a dd 4
        b dd 4.4
        c times 10 dd 0
        d dw 1, 2
        e db 0xfb
        f db "string", 0

        segment .bss
        g resd 1
        h resd 10
        i resd 100

        segment text
        global main

main:
        push rbp
        mov rbp, rsp
        sub rsp, 16
        xor eax, eax
        leave
        ret
