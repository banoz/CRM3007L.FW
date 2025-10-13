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

//bit uart_rx_overflow(void);        // Flag for buffer overflow (optional handling)

bit uart_rx_ready(void);
// Get one byte from RX buffer (call only if ready; blocks not, assumes checked)
unsigned char uart_getchar(void);

void UART_TX(unsigned char n_temp0);
void init_UART(void);

#endif
