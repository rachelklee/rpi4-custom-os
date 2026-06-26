/* bootloader.s - ARM bootloader for Pi 4 */

.section .text.boot
.global _start

_start:
    /* TODO: Write bootloader code here */
    b .  /* Infinite loop for now */
    
    .end
