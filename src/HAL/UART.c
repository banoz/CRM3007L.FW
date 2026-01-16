#include "OB38R16T1.h"
#include "UART.h"

//===============================================================
//DEFINITIONs
//===============================================================
#define d_S0RELH        0x03
#define d_S0RELL        0xFE // 250000 baud

//===============================================================
//GLOBAL VARIABLE
//===============================================================
bit bU0TX = 0;

// Ring buffer for RX: 16 bytes, circular FIFO
#define RX_BUF_SIZE 16
unsigned char rx_buf[RX_BUF_SIZE];
unsigned char rx_head = 0;  // Write index (ISR updates)
unsigned char rx_tail = 0;  // Read index (main/app reads)

//=========================================================================
void init_UART(void)
{
    S0RELH   = d_S0RELH;
    S0RELL   = d_S0RELL;
    AUX     |= 0x80 | 0x10;     // BRS=1, SICS[1:0]=01
    PCON    |= 0x80;            // SMOD=1
    IEN0    |= 0x90;            // EA=1, ES0=1, interrupt enable
    S0CON    = 0x50;            // mode 1, SM20=0, Revice enable
    TI       = 0;
}

// Check if data available in RX buffer
bit uart_rx_ready(void) {
    return (rx_head != rx_tail);
}

// Get one byte from RX buffer (call only if ready; blocks not, assumes checked)
unsigned char uart_getchar(void) {
    unsigned char data1 = rx_buf[rx_tail];
    rx_tail = (rx_tail + 1) % RX_BUF_SIZE;
    return data1;
}

void UART_ISR(void) interrupt d_UART_Vector
{
    unsigned char RXBUF0;
		unsigned char next_head;
	
    if (RI)                     // INT_RX
    {
			  RI = 0;                 // RI clear
        RXBUF0 = S0BUF;         // Read BUF
			
				next_head = (rx_head + 1) % RX_BUF_SIZE;
        
				if (next_head != rx_tail) {  // Buffer not full
            rx_buf[rx_head] = RXBUF0; // Store received byte
            rx_head = next_head;     // Advance head
        }
    }
    else                        // INT_TX
    {
        bU0TX  = 0;
        TI     = 0;             // TI clear
    }
}

void UART_TX(unsigned char n_temp0)
{
    bU0TX = 1;
    S0BUF = n_temp0;
    while(bU0TX);
}

//=========================================================================
