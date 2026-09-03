/* bootloader.s - ARM bootloader for Pi 4 */

.section .text.boot
.global _start

_start:
    
    // switch to Thumb mode 
    LDR R0, =1f + 1 // load the address of label 1 (forward in code) plus 1 into r0, jump to label one and +1 tells CPU to switch to Thumb mode
    BX R0

    .thumb
    .align 2

1: // label 1
    CPSID IF // disable interrupts, unmask with cpsie if
    LDR R0, =(0x80000000 + 0x10000) // load stack pointer to 0x80010000
    MOV SP, R0

    LDR R0, =__bss_start__ // R0 = __bss_start__
    LDR R1, =__bss_end__ // R1 = __bss_end__
    MOV R2, #0 // R2 = 0

    loop:
        CMP R0, R1 // compare R0 and R1
        BEQ done 

        STR R2, [R0] // store 0 in address at R0
        ADD R0, R0, #4
        B loop 

done: 
    BL main
    b .


    .end
