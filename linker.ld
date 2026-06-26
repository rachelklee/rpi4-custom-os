/* linker.ld - Memory layout for Pi 4 */

ENTRY(_start)

MEMORY {
    FLASH (rx) : ORIGIN = 0x80000, LENGTH = 0x2000000
    RAM (rwx) : ORIGIN = 0x80000000, LENGTH = 0x100000000
}

SECTIONS {
    .text : {
        *(.text.boot)
        *(.text)
        *(.rodata)
    } > FLASH
    
    .data : {
        *(.data)
    } > FLASH
    
    .bss : {
        __bss_start__ = .;
        *(.bss)
        *(COMMON)
        __bss_end__ = .;
    } > FLASH
}
