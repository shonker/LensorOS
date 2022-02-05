#include "uart.h"

// Global serial driver.
UARTDriver srl;

UARTDriver::UARTDriver() {
    // Disable all interrupts.
    outb(COM1 + 1, 0x00);
    // Enable DLAB (most significant bit)
    outb(COM1 + 3, 0b10000000);
    // Set divisor
    outb(COM1, (u8)BAUD_DIVISOR);
    outb(COM1 + 1, (u8)((u16)BAUD_DIVISOR >> 8));
    // Set Line Control
    outb(COM1 + 3, 0b00000011);
    // Enable FIFO, clear them, 14-byte threshold
    outb(COM1 + 2, 0xc7);
    // IRQs enabled, Data Terminal Ready and Request to Send are set.
    outb(COM1 + 4, 0b00001011);
    // Enable loop-back.
    outb(COM1 + 4, 0b00011110);
    // Loop-back test.
    outb(COM1, 0xae);
    if (inb(COM1) != 0xae) {
        // Error! No good.
        // TODO: Something about it.
    }
    // Enable all interrupts except for loop-back.
    outb(COM1 + 4, 0b00001111);
}

/// Read a byte of data over the serial communications port COM1.
u8 UARTDriver::readb() {
    u16 maxSpins = (u16)1000000;
    while (inb(COM1 + 5) & 0b00000001
           && maxSpins > 0)
    {
        maxSpins--;
    }
    return inb(COM1);
}

/// Write a byte of data over the serial communications port COM1.
void UARTDriver::writeb(u8 data) {
    /// Spin (halt execution) until port is available or maxSpins iterations is reached.
    u16 maxSpins = (u16)1000000;
    while (inb(COM1 + 5) & 0b00100000 && maxSpins > 0) {
        maxSpins--;
    }
    outb(COM1, data);
}

/// Write a C-style null-terminated byte-string to the serial output COM1.
void UARTDriver::writestr(const char* str) {
    // Set current character to beginning of string.
    char* c = (char*)str;
    // Check for null-terminator at current character.
    while (*c != 0) {
#ifdef LENSOR_OS_UART_HIDE_COLOR_CODES
        if (*c == '\33' || *c == '\033' || *c == '\x1b' || *c == '\x1B') {
            // Loop until null terminator or 'm'.
            do { c++; } while (*c != 'm' && *c != 0);
            // Don't read memory past null terminator!
            if (*c == 0)
                return;
            // Skip the 'm'.
            c++;
            if (*c == 0)
                return;
        }
#endif
        writeb((u8)*c);
        c++;
    }
}

/// Write a given number of characters from a given string of characters to serial output COM1.
void UARTDriver::writestr(char* str, u64 numChars) {
    while (numChars > 0) {
#ifdef LENSOR_OS_UART_HIDE_COLOR_CODES
        if (*str == '\33' || *str == '\033' || *str == '\x1b' || *str == '\x1B') {
            // Loop until no more chars or 'm'
            do {
                str++;
                numChars--;
            } while (*str != 'm' && numChars > 0);
            if (*str == 'm') {
                str++;
                numChars--;
            }
            if (numChars == 0)
                return;
        }
#endif
        writeb((u8)*str);
        str++;
        numChars--;    
    }
}
