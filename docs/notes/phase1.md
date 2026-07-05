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

### UART
* UART driver structure
    ```
    uart.h (header file)
    |-- uart_init()
    |-- uart_putc(char) --> put char
    |-- uart_puts(const char *s) --> put string
    |-- uart_getc() [optional for input]

    uart.c (implementation)
    |-- register definitions (addresses)
    |-- uart_init() implementation
    |-- uart_putc() implementation
    |-- uart_puts() implementation
    ```
* kernel.c calls uart_iit() at tartup, uses uart_puts() for all kernel related outputs
* BCM2711 ARM Peripherals Datasheet
    * GPIO
        * ![GPIO Block Diagram](GPIO_block_diagram.png)
            * this diagram shows how a single GPIO pin works and how software can control a pin/what features are available
            * what can a pin do:
                * be an input (read voltage from external circuit)
                * be an output (drive voltage to external circuit)
                * use alternate functions
                * generate interrupts by notifying CPU when pin state changes
                * have pull-up/pull-down resisters
                * detect edges or levels
            * LEFT (pull-up/pull-down control)
                * when a GPIO pin is configured as input it is floating (undefined voltage) if nothing is connected. Pull-up resistor connects pin to 3.3V through a resistor if nothing drives then the pin reads HIGH. Pull-down resister connects the pin to GND (ground) through a resistor if nothing drives then the pin reads LOW.
                * This is controlled using registers
                    ```
                    PULL_UP_ENABLE = 1    // Pull pin toward 3.3V
                    PULL_UP_ENABLE = 0    // No pull-up
                    PULL_DN_ENABLE = 1    // Pull pin toward GND
                    PULL_DN_ENABLE = 0    // No pull-down
                    ```
            * TOP (Pin Direction Register)
                * direction register tells the pin if it is an input or output
                    ```
                    DIRECTION = 0 // Input
                    DIRECTION = 1 // Output
                    ```
            * TOP RIGHT, red lines (function selection registers)
                * pins can be GPIO mode (controlled by GPIO output register and pin direction) or alternate function mode (connected to something else like UART, SPI, I2C, etc)
                    ```
                    FUNCTION_SELECT = 0 // GPIO mode
                    FUNCTION_SELECT = 4 // alternate function 0
                    FUNCTION SELECT = 5 // alternate function 1
                    ``` 
            * CENTER RIGHT (output control)
                * write to output registers to control the pin voltage
                    ```
                    OUTPUT_SET = 1 // drive pin HIGH (3.3V)
                    OUTPUT_CLEAR = 1 // drive pin LOW (0V)
                * if alternate function used, the alternate hardware controls output signal (for example, UART)
            * CENTER LEFT (input path)
                * if the pin is set to be input or alternate function input voltage on pin is read to determine if pin is HIGH or LOW
            * BOTTOM (detection logic)
                * level detection
                    * is the pin currently HIGH or LOW (continuous, synchronous to clock)
                    ``` 
                    LEVEL_DETECT = HIGH // interrupt CPU if pin is HIGH
                    LEVEL_DETECT = LOW // interrupt CPU if pin is LOW
                    ```
                * edge detection
                    * rising edge means pin transitioned from LOW to HIGH
                    * falling edge means pin transitioned from HIGH to LOW
                    ```
                    RISING_EDGE_ENABLE = 1 // interrupt CPU on rising edge
                    FALLING_EDGE_ENABLE = 1 // interrupt CPU on falling edge
                    ```
                * async detection
                    * async detection is faster than regular edge detection because it doesn't wait for clock, use for fast response
            * RIGHT (interrupts)
                * when a detected event (from the bottom) occurs an interrupt flag is set
                * CPU interrupt handler reads this flag and then clears the flag
                ```
                if (RISING_EDGE_DETECTED == 1) {
                    // interrupt
                    RISING_EDGE_DETECTED = 0; // clear flag
                }
                ```
        * GPIO 14 and 15 are set for UART
            ```
            GPFSEL1[14:12] = 1 // GPIO 14, alternate function (UART TX, transmit)
            GPFSEL1[17:15] = 1 // GPIO 15, alternate function (UART RX, receive)
            ```
            * GPIO 14 automatically outputs UART TX signal
            * GPIO 15 automatically reads UART RX signal
            * UART hardware takes over (control output state and input value)
        
        * GPIO base address for this project is `0x3F200000` 
        * GPIO controller manages all 54 GPIO pins (GPIO 0-53)
            | Register | Offset | Controls|
            | :--- | :---: | ---: |
            | GPFSEL0 | 0x00 | GPIO 0-9 (3 bits each) |
            | GFSEL1 | 0x04 | GPIO 10-19 (3 bits each) | 
            | GFSEL2 | 0x08 | GPIO 20-29 (3 bits each) |
            | GFSEL3 | 0x0C | GPIO 30-39 (3 bits each) |
            | GFSEL4 | 0x10 | GPIO 40-49 (3 bits each) |
            | GFSEL5 | 0x14 | GPIO 50-53 (3 bits each) |
            * each GPIO pin uses 3 bits in its GPFSEL register
            ```
            000 = GPIO input
            001 = GPIO output
            100 = Alternate function 0 (ALT0)
            101 = Alternate function 1 (ALT1)
            110 = Alternate function 2 (ALT2)
            111 = Alternate function 3 (ALT3)
            010 = Alternate function 4 (ALT5)
            ```
            * EXAMPLE: GPIO 14 is controlled by bits [14:12] of GPFSEL1, `GPFSEL1[14:12] = 100 (bin) = 4 (dec) = ALT0 (UART TX)`
            


    * UART


### What registers need to be configured
* To implement UART
    1. configure GPIO 14 and GPIO 15 as UART pins: in GPIO mode they are general-purpose input/output but can be configured for alternate functions (Alternate Function 0 for UART mode)
    2. configure the UART controller: set baud rate to 115200 bps, word size to 8 bits, enable UART transmit
    3. baud rate calculation: values for IBRD and FBRD for 115200 baud at 48MHz clock (CM2711 ARM Peripherals PDF, ARM PL011 UART Technical Reference)