# Phase 0

## Notes

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

## The Linker Script
* tells the linker where to place code in memory
```
ENTRY(_start) ; Start execution at _start label

MEMORY {
    FLASH : ORIGIN = 0x80000, LENGTH = 32M ; bootloader starts at address 0x80000
}

SECTIONS {
    .text.boot : { ; bootloader (.text.boot) code goes first
        *(.text.boot) 
    }

    .text : { ; all other (.text) code goes after
        *(.text) 
    }

    .bss : { ; define __bss_start__ and __bss_end__ symbols, bootloader uses these symbols to clear BSS
        __bss_start__ = .; 
        *(.bss)
        __bss_end__ = .;
    }
}
```
