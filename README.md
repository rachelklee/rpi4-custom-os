# rpi4-custom-os

## Hardware

- **Target:** Raspberry Pi 4 (ARM Cortex-A72, 4GB RAM)
- **Toolchain:** arm-none-eabi-gcc (cross-compiler)
- **Serial debugging:** USB-to-UART adapter (CP2102)

## Build Instructions

```bash
make clean
make
```

This produces `kernel8.img` — the bootloader + kernel binary.

## Load onto Pi 4

1. Insert microSD card with Raspberry Pi OS into Mac
2. Copy `kernel8.img` to `/boot/` partition
3. Eject and insert into Pi 4
4. Connect UART adapter (TX → GPIO 14, RX → GPIO 15, GND → GND)
5. Run serial monitor at 115200 baud
6. Power on Pi

## Project Phases

- [x] **Phase 0:** Bootloader + minimal kernel
- [ ] **Phase 1:** UART driver + serial output
- [ ] **Phase 2:** Task scheduler + multitasking
- [ ] **Phase 3:** BLE protocol handler
- [ ] **Phase 4:** MCP server for Claude
- [ ] **Phase 5:** Integration + documentation

## Files

- `bootloader.s` — ARM assembly entry point
- `kernel.c` — C kernel entry point
- `linker.ld` — Memory layout (bootloader at 0x80000)
- `Makefile` — Build automation