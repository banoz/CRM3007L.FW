/*
 * OB38R16T1 MCU Configuration Header
 */

#ifndef CONFIG_H
#define CONFIG_H

// MCU specifications
#define MCU_NAME "OB38R16T1"
#define FLASH_SIZE 16384    // 16KB Flash
#define IRAM_SIZE 256       // 256 bytes internal RAM
#define XRAM_SIZE 512       // 512 bytes SRAM

// Clock configuration
#define FOSC 16000000UL     // 16 MHz RC oscillator

// UART configuration
#define BAUD_RATE 9600

// Timer configuration
#define TIMER0_MODE 1       // 16-bit timer mode

#endif // CONFIG_H
