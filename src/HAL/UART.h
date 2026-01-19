#ifndef __UART__
#define __UART__

// Example Code
/*
#include "UART.h"
#include "OB38R16T1.h"

void main(void)
{
    init_UART();
    while(1)
    {
        UART_TX(0xAA);    // 0xAA is data.
    }
}
*/

bit uart_rx_ready(void);
unsigned char uart_getchar(void);
unsigned char uart_get_overflow_count(void);  // Get count of dropped bytes
void uart_clear_overflow_count(void);          // Reset overflow counter

void UART_TX(unsigned char n_temp0);
void init_UART(void);

#endif
