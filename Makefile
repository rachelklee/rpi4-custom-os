# Makefile - Compile bootloader + kernel for Pi 4

CROSS_COMPILE = arm-none-eabi-
CC = $(CROSS_COMPILE)gcc
LD = $(CROSS_COMPILE)ld
OBJCOPY = $(CROSS_COMPILE)objcopy
OBJDUMP = $(CROSS_COMPILE)objdump

CFLAGS = -mcpu=cortex-a72 -fpic -ffreestanding -Wall -Wextra -O2
LDFLAGS = -T linker.ld -Map kernel.map

# Output
all: kernel8.img

# Link bootloader + kernel
kernel.elf: src/bootloader/bootloader.o src/kernel/kernel.o
	$(LD) $(LDFLAGS) -o kernel.elf src/bootloader/bootloader.o src/kernel/kernel.o

# Convert to binary image
kernel8.img: kernel.elf
	$(OBJCOPY) kernel.elf -O binary kernel8.img
	@echo "✓ Built kernel8.img"

# Compile bootloader
src/bootloader/bootloader.o: src/bootloader/bootloader.s
	$(CC) $(CFLAGS) -c src/bootloader/bootloader.s -o src/bootloader/bootloader.o

# Compile kernel
src/kernel/kernel.o: src/kernel/kernel.c
	$(CC) $(CFLAGS) -c src/kernel/kernel.c -o src/kernel/kernel.o

# Debug: show disassembly
disasm: kernel.elf
	$(OBJDUMP) -d kernel.elf | head -100

# Debug: show symbols
symbols: kernel.elf
	$(CROSS_COMPILE)nm kernel.elf | grep -E "main|_start|bss"

# Clean
clean:
	rm -f kernel.elf kernel8.img kernel.map
	rm -f src/bootloader/*.o src/kernel/*.o
	@echo "✓ Cleaned"

.PHONY: all clean disasm symbols
