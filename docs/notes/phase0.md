# Phase 0: Bootloader

### The Whole Pipeline
```
bootloader.s (your code)
    ↓
[Assembler]
    ↓
bootloader.o (machine code + placeholders)

kernel.c (your code)
    ↓
[Compiler]
    ↓
kernel.o (machine code + placeholders)

bootloader.o + kernel.o + linker.ld
    ↓
[Linker]
(decides all addresses, fills in placeholders)
    ↓
kernel.elf (executable with all symbols resolved)

kernel.elf
    ↓
[Objcopy]
(strip debug info, convert to binary)
    ↓
kernel8.img (pure machine code)

kernel8.img → copied to Pi → GPU loads at 0x80000 → CPU executes
```

### Memory Layout
* 0x80000 to 0x100000 --> bootloader code
* 0x80010000 --> stack, growing downward (pushing makes sp go to a lower address but has bigger number)
* 0x3F000000 --> hardware peripherals (GPIO, UART, etc)

## Boot Process
* when you turn on Raspberry Pi
    1. GPU firmware runs on the GPU chip
    2. GPU loads coad from SD card into memory at address 0x80000
    3. GPU jumps ARM CPU to 0x80000 to begin execution here (this is where bootloader code lives)
    4. bootloader code runs
    5. bootloader sets up CPU and calls kernel
* bootloader sets up the C environment since CPU is in a bare state (no stack, global variables not initialized, can't call functions, can interrupt)
    * disable interrupts so CPU isn't interrupted
    * set up the stack pointer (sp) so functions  can be called
    * clear global variables (BSS), initializes memory to 0
    * jump to main() and call kernel code in C
* boot sequence
    1. switch to 16 bit Thumb Mode (`.thumb`)
    2. disable interrupts (`cpsid if`)
    3. set up stack by pointing stack to memory safe location to start, grow down from here, (`sp = 0x80010000`)
    4. Clear BSS (Block Started by Symbol), initizialize and zero out global variables and registers
    5. call main (`bl main`), return address stores in lr
* bootloader runs in supervisor mode (as opposed to user mode) to have full access
* .thumb
    * load the address of the first label (the one you want in .thumb) forward plus 1 into register and branch to address in register
        *  why plus 1? +1 is for the CPU mode flag. ARM CPUs use the lowed bid of address to determin execution mode
            * Bit 0 = 0 (even address) runs in ARM mode with 32 bit instructions
            * Bit 0 = 1 (odd address) runs in Thumb mode with 16 bit instructions
            * EXAMPLE: 
            ```
            Address 0x8000C in binary: ...11000 (lowest bit is 0 = ARM mode)
            Address 0x8000D in binary: ...11001 (lowest bit is 1 = Thumb mode)

            0x8000D = 0x8000C + 1 
            so...
            ldr r0, = 1f + 1 ; loads 0x8000D into r0
            bx r0 ; jump to r0 (0x8000D), CPU checks lowest bit of 0x8000D which is 1 so CPU switches in Thumb mode --> exeuct instructio nat address 0x8000C in Thumb encoding

            the instruction at label 1 is actually at address 0x8000C but if you jump to 0x8000D the odd address switches to THumb mode but executes code at 0x8000C
            ```

## The Linker Script
* tells the linker where to place code in memory
```
ENTRY(_start) ; Start execution at _start label

MEMORY {
    FLASH : ORIGIN = 0x80000, LENGTH = 32M ; bootloader starts at address 0x80000, memory region called FLASH starting at 0x80000 and extends 32 MB
}

SECTIONS {
    .text.boot : { ; bootloader (.text.boot) code goes first, put all .text.boot sections from the object file in FLASH memory
        *(.text.boot) 
    } > FLASH

    .text : { ; all other (.text) code goes after
        *(.text) 
    } > FLASH

    .bss : { ; define __bss_start__ and __bss_end__ symbols, bootloader uses these symbols to clear BSS, __bss_start___ is a linker symbol that euals the current address (bootload code does ldr r0, =__bss_start__ and linker replaces it with actual address)
        __bss_start__ = .; 
        *(.bss)
        __bss_end__ = .;
    } > FLASH
}
```

```
*** NOTE: FLASH is a type of memory that is a non-volatile electric computer storage. It retains power even without power. It consists of millions of tiny transisters that trap electrions to represent binary code. FLASH memory stores bootloader code to allow device to boot up safely and launch the primary OS***
```

*  when writing code the __written code__ (bootloader.s, kernel.c) is sent to the __compiler/assembler__ which producesses __object files__ (.o files) and sends them to the __linker__ to produce __executable files__ (kernel.elf, kernel8.img, etc) which are executed by the __CPU__
    * assembler translates code into machine code (CPU instructions) but assembler doesn't know where anything is in memory (ex: assembler sees `bl main` but doesn't know what address `main` is at so assembler adds placeholder)
    * object file (.o file) has machine code with placeholders and a symbol table with labels/functions without addresses yet; include relocation information to fix placeholders later
    * linker stitches object file into one executable
        1. decide where everything goes in memory
        2. fill in placeholders with real addresses
        3. resolve symboles
        4. create final executable 
    * linker script helps decide where `ORIGIN` is (where bootloader goes) which helps fill in placeholders
    * linker script tells linker how to arrange code in memory (`SECTIONS`)
* BSS = BLock Stated by Symbol
    * region that stores uninitialized global variables (does not have initial value)
    * when you compile, compiler doesn't store the values for the variables in the executable but it reserves space in memory for the variable, it contains garbage data when program initially loads
    * bootloader needs to zero out the garbage so programmer doesn't need to

## Objectives

- [x] Start with .section and .global directives
- [x] Create _start: label
- [x] Switch to Thumb mode using ldr and bx
- [x] Disable interrupts using cpsid
- [x] Load stack pointer to 0x80010000
- [x] Clear BSS section with a loop:
    - [x] Load __bss_start__ into r0
    - [x] Load __bss_end__ into r1
    - [x] Move 0 into r2
    - [x] Create a loop label
    - [x] Compare r0 and r1
    - [x] Branch if equal to done label
    - [x] Store r2 at address in r0
    - [x] Add 4 to r0
    - [x] Branch back to loop
    - [x] Create done label
- [x] Call main() using bl
- [x] Infinite loop at the end using b .