# Phase 1a: Changing ISA

_NOTE: After building the OS and developing the UART drivers I proceeded to test the UART functionality. I was receiving no output from UART SERIAL and realized that there was a config mismatch caused by the bootloader being written in ARM32 (AArch 32) instead of ARM64 (AArch 64)_

### What is ARM32 (AArch32) vs ARM64 (AArch64)?
* both are ISAs designed by ARM
* ARM32/AArch32
    * Registers are 32 bits wide
        * R0-R15 (16 total registers), R15 doubles as program counter, R13 is the conventional stack pointer, R14 is the conventional link register 
        * R13-15 are not dedicated registers architecturally, this is just convention
    * 2 instruction encodings (standard 32 bit ARM, Thumb which is 16 bit instruction per word mode)
    * processor modes &rarr; ad-hoc, AArch32 modes dont follow a tidy ladder
        * User: normal, unprivileged code execution, no access to sensitive registers/instructions
        * IRQ: "Interrupt Request", entered automatically when a normal hardware interrupt files (eg: timer tick or peripheral saying "I have data for you")
        * FIQ: "Fast Interrupt Request", second interrupt mode meant for one latency critical interrupt source, ARM design with more banked registers than IRQ to make handler faster (less need for save/restore)
        * Supervisor (SVC): where CPU starts up in and OS kernel code typically runs, privileged
        * Abort: entered automatically when a memory access failed
        * Undefined: entered automatically when the CPU tries to execute an instruction it doesn't recognize
        * System: Privileged mode, uses same registers as user mode but with full privileges, mostly convenience mode 
    * Register banking
        * Banking: when CPU switches modes some registers get automatically swapped for mode-specific private copies (eg: IRQ has a private stack pointer `R13` and link register `R14` separate from supervisor mode's `R13`/`R14`)
            * LR (link register): stores the return address for a function or subroutine call
        * Allows interrupt handler to safely use stack without corrupting interrupted code was doing, each mode has its own SP ready to use when CPU switches
    * CPSR (Current Program Status Register)
        * 32 bit
        * holds CPU's current status (dashboard of flags and settings for mode interrupts allowed)
            * current processor mode: bottom 5 bits
            condition flags: N (negative), Z (zero), C (carry), V (overflow), set by instructions like `CMP` and used by conditional branches like `BEQ`, `BNE`
            * Interrupt mask bits: I bit (masks IRQ), F bit (masks FIQ), if set CPU ignores that interrupt type
            * thumb state bit: is CPU interpreting instructions in ARM or Thumb encoding
        * `CPSID IF`
            * sets I and F bits of CPSR, disables inderrupts
            * `CPSIE IF` to re-enable

### ARM64/AArch64
* registers are 64 bit, 31 general purpose registers (`X0-X30`) that are addressable as its bottom 32 bit half (`W0-W30`)
* No thumb, one instruction encoding only (fixed width, 32 bits per instruction)
* hierarchical privilege model (Exception Levels EL0-EL3)
* Interrupt/exception masking uses `DAIF` bits (Debug, Abort/SError, IRQ, FIQ) which are manipulated using `MSR`/`DAIFSet`/`DAIFClr` rather than direct CPSR (Current Program Status Register) bit twiddling

### Exception Levels
* think: concentric rings of privilege
* ELO: least privileged. Ordinary application/userspace code runs here. Can't touch hardware directly, can't configure memory translation
* EL1: Where an OS kernel normally runs. Manages memory (MMU,page tales), handle most exceptions, talk to most peripherals
* EL2: Hypervisor level. Supports virtualization, can host multiple "guest" EL1/EL0 environments
* EL3: Most privileged. Secure monitor level, handles switching between secure and non-secure worlds (ARM TrustZone), firmware level trust bootstrap, etc.

### Bootloader Structure
```
_start:
    [1. mask interrupts]
    [2. check current EL]
    [3. if not EL1, configure and drop to EL1]
    [4. set up stack pointer]
    [5. clear BSS]
    [6. branch to main]    
```
