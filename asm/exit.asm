        ;; yasm -f elf64 -g dwarf2 -l exit.lst exit.asm
        ;; ld -o exit exit.o
        ;; echo $?

        segment .text
        global _start

_start:
        mov eax, 1              ; system function 'exit' @ 1
        mov ebx, 5              ; return value
        int 0x80                ; execute system function
