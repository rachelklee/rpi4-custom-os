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
