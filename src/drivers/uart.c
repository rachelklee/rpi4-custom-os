#include <stdint.h>
#include "uart.h"

// define register addresses
// use volatile when accessing hardware registers to prevent optimizations

// bases
#define UART_BASE 0x3F201000
#define GPIO_BASE 0x3F200000

// gpio registers
#define GPIO_PUP_PDN_CNTRL_REG0 (GPIO_BASE + 0xE4)
#define GPFSEL1 (GPIO_BASE + 0x04)

// uart registers
#define UARTIMSC (UART_BASE + 0x38)

#define UART_BAUD_RATE 115200
#define UARTIBRD (UART_BASE + 0x24)
#define UARTFBRD (UART_BASE + 0x28)

#define UARTLCR_H (UART_BASE + 0x2C)
#define UARTICR (UART_BASE + 0x30)

// implement uart_init, uart_putc, and uart_puts functions

// 1. disable pull-up/pull-down on GPIO 14/15
// 2. configure GPIO 14/15 as UART
// 3. disable interrupts
// 4. set baud rate
// 5. configure line control register
// 6. enable UART (control register)
void uart_init(void) {
    // cast to a volatile pointer (inside parenthesis), dereference pointer (outside asterisk)
    volatile uint32_t pupPdnCntrlRegVal = *(volatile uint32_t *)GPIO_PUP_PDN_CNTRL_REG0;
    pupPdnCntrlRegVal &= 0b00001111111111111111111111111111;
    *(volatile uint32_t *)GPIO_PUP_PDN_CNTRL_REG0 = pupPdnCntrlRegVal;

    volatile uint32_t gpfsel1Val = *(volatile uint32_t *)GPFSEL1;
    gpfsel1Val &= 0b11111111111111000000111111111111;
    gpfsel1Val |= 0b00000000000000100100000000000000;
    *(volatile uint32_t *)GPFSEL1 = gpfsel1Val;

    *(volatile uint32_t *)UARTIMSC = 0x0;

    *(volatile uint32_t *)UARTIBRD = 26;
    *(volatile uint32_t *)UARTFBRD = 3;

    *(volatile uint32_t *)UARTLCR_H = 0b01110000;
    *(volatile uint32_t *)UARTICR = 0b1100000001;
}

// send one character
// 1. check if UART is ready (UARTFR bit 6)
// 2. wait if not ready
// 3. send character when ready (write to UARTDR)
void uart_putc(char c) {

} 


void uart_puts(const char *s) {

}