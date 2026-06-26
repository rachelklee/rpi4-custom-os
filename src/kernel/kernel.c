/* kernel.c - Kernel main entry point */

void uart_init(void);
void uart_puts(const char *s);

int main(void) {
    uart_init();
    uart_puts("Hello from ARM!\n");
    
    while (1) {
        ;
    }
    
    return 0;
}

void uart_init(void) {
    /* TODO: Initialize UART */
}

void uart_puts(const char *s) {
    /* TODO: Send string to UART */
    (void)s;
}
