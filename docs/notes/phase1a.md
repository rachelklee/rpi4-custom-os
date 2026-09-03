# Phase 1a: Changing ISA

_NOTE: After building the OS and developing the UART drivers I proceeded to test the UART functionality. I was receiving no output from UART SERIAL and realized that there was a config mismatch caused by the bootloader being written in ARM32 (AArch 32) instead of ARM64 (AArch 64)_

### What is ARM32 (AArch32) vs ARM64 (AArch64)?
* both are ISAs designed by ARM
* ARM32/AArch32
    * Registers are 32 bits wide
        * R0-R15 (16 total registers), R15 doubles as program counter, R13 is the conventional stack pointer, R14 is the conventional link register 
        * R13-15 are not dedicated registers architecturally, this is just convention
    * 2 instruction encodings (standard 32 bit ARM, Thumb which is 16 bit instruction per word mode)
    * processor modes
        * User, IRQ, FIQ, Supervisor, Abort, Undefined System
    * 
