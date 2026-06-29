# Phase 1: UART Driver

### What is UART?
* UART = Universal Asynchronous Receiver/Transmitter
* UART is for debugging the OS
    * connects Pi to Mac
    * prints debug messages to terminal 
    * allows visibility into what OS is doing
* protocol for sending/receiving data one byte at a time over two wires
    * TX (transmit) --> send data from Pi to MAC
    * RX (receive) --> receive dat from Pi to MAC
* for Pi 4: UART0 is main serial port, TX on GPIO 14 (pin 8), RX on GPIO 15 (pin 10), baud rate is 115200 bits/second
    * baud rate is the speed of communication, how many bits per second you ucan send over the wire
    * example:  
        Send the character 'H' which is `01001000` in binary (8 bits)
        at $115200$ baud  
        each bit takes $1/115200=0.0000087$ seconds  
        the whole byte takes $8*(1/115200) = 0.000069$ seconds
```
Pi 4 boots
    ↓
Bootloader initializese CPU
    ↓
Kernel calls main()
    ↓
main() does stuff
    ↓
Pi 4 UART Controller
    ↓
    Sends bytes to TX wire
    ↓
USB-to-UART adapter
    ↓
Your Mac serial console
    ↓
"Hello from ARM!"
```

### How does memory-mapped I/O work?
* hardware devices (UART, GPIO) controlled by registers
* example:
    ```
    uint32_t *uart_data = (uint32_t *)0x3F201000;
        // declare a pointer: uart_data is a pointer to a 32 bit unsigned integer
        // *uart_data is a pointer that stores the memory address 0x3F201000, address is cased to a pointer to a 32 bit unsigned int (so the address isn't just a number)
        // uart_data is a pointer that points to memory address 0x3F201000
    *uart_data = 'H'; // send the character 'H' to UART
        // dereference (*) the pointer and write 'H' to the memory address
    ```
* GPIO controller base: `0x3F200000`
* UART0 controller base: `0x3F201000`
* hardware devices (peripherals like UART, GPIO, Timer, Bluetooth, USB, etc) are accessed through memory addresses, each is a separte chip/circuit inside or attached to the Pi
    * CPU tells peripheral to turn pins to HIGH/LOW using memory-mapped I/O by writing to specific memory addresses
    * memory-mapped because peripherals are "mapped" into address space at `0x3F000000-0x3F010000`

### What registers need to be configured
* To implement UART
    1. configure GPIO 14 and GPIO 15 as UART pins: in GPIO mode they are general-purpose input/output but can be configured for alternate functions (Alternate Function 0 for UART mode)
    2. configure the UART controller: set baud rate to 115200 bps, word size to 8 bits, enable UART transmit
    3. baud rate calculation: values for IBRD and FBRD for 115200 baud at 48MHz clock (CM2711 ARM Peripherals PDF, ARM PL011 UART Technical Reference)