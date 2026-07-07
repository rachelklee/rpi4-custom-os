#include <stdint.h>
#include "uart.h"

// define register addresses
// use volatile when accessing hardware registers to prevent optimizations
#define GPIO_PUP_PDN_CNTRL_REG0 (volatile uint32_t *)(UART_BASE + 0xE4)
#define GPIO14 14
#define GPIO15 15

#define UARTIMSC (*(volatile uint32_t *)(UART_BASE + 0x38))

#define UART_BASE (volatile uint32_t *)0x3F201000
#define UART_BAUD_RATE 115200
#define UARTIBRD (*(volatile uint32_t *)(UART_BASE + 0x24))
#define UARTFBRD (*(volatile uint32_t *)(UART_BASE + 0x28))

#define UARTLCRH (*(volatile uint32_t *)(UART_BASE + 0x2C))
#define UARTCR (*(volatile uint32_t *)(UART_BASE + 0x30))

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
    *GPIO_PUP_PDN_CNTRL_REG0 = pupPdnCntrlRegVal;

    
}

void uart_putc(char c) {

} 

void uart_puts(const char *s) {

}