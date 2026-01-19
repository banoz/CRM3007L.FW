#include "OB38R16T1.h"
#include "IIC.h"

//=====================================================================
#define d_IIC_Adress_1      (0x10 << 1)
#define d_IIC_Adress_2      (0x12 << 1)

#define d_ACK               0
#define d_NACK              1
#define d_Write             0
#define d_Read              1
#define d_null              0
#define d_CMD_RW            0x40
#define d_SAVE_ADDR         10
#define d_SAVE_DATA         20
//=====================================================================

volatile unsigned char n_RW          = d_null;
volatile unsigned char n_Addr        = d_null;
volatile unsigned char n_Next_Step   = d_null;
volatile unsigned char n_DAT[16] = {0};

// I2C error tracking
volatile unsigned int iic_rx_count = 0;    // Count of receive interrupts
volatile unsigned int iic_tx_count = 0;    // Count of transmit interrupts
volatile unsigned int iic_nack_count = 0;  // Count of NACK received

//=====================================================================

void IIC_ISR(void) interrupt d_IIC_Vector
{	
	if(RXIF)
	{
		RXIF = 0;
		iic_rx_count++;  // Track RX activity
		
		if((IICA1 & 0x01) | (IICA2 & 0x01))    //match
		{
			n_RW = RW;
			
			if (n_RW == d_Write)
			{
				n_Next_Step = d_SAVE_ADDR;
				n_Addr = 0x00;
			}
			else
			{
				IICRWD = n_DAT[n_Addr];
				n_Addr++;
			}	
		}
		else if(n_RW == d_Write)
		{
			if (n_Next_Step == d_SAVE_ADDR)
			{ 
				n_Addr = IICRWD;
				n_Next_Step = d_SAVE_DATA;						
			}
			else if (n_Next_Step == d_SAVE_DATA)
			{
				n_DAT[n_Addr++] = IICRWD;
				n_Next_Step = d_SAVE_DATA;					
			}
		}
		IICEBT = d_CMD_RW;            // IIC bus ready			
	}
	if (TXIF)	//data Transmit Interrupt Flag
	{
		TXIF = 0;
		iic_tx_count++;  // Track TX activity
		
		if(RXAK == 0)
		{
			IICRWD = n_DAT[n_Addr];
			n_Addr++;
		}
		else
		{
			iic_nack_count++;  // Track NACK responses
		}
		IICEBT = d_CMD_RW;            // IIC bus ready
	}

}

void IIC_init_slave(void)
{
    IICS    = 0x00;             // init IICS
    IRCON   = 0x00;             // init IRCON
    IICEBT  = d_CMD_RW;         // IIC bus ready
    IICA1   = d_IIC_Adress_1;   // Control Byte 1
    IICA2   = d_IIC_Adress_2;   // Control Byte 2
    IEN1   |= 0x20;             // Enable interrupt IIC
    IEN0   |= 0x80;             // Enable interrupt All
    IICCTL  = 0x80;             // Enable IIC module, slave mode, use IICA1
    
    // Initialize error counters
    iic_rx_count = 0;
    iic_tx_count = 0;
    iic_nack_count = 0;
}

/**
 * @brief Get I2C communication statistics
 * @param rx_count Pointer to store RX interrupt count
 * @param tx_count Pointer to store TX interrupt count
 * @param nack_count Pointer to store NACK count
 */
void IIC_get_stats(unsigned int *rx_count, unsigned int *tx_count, unsigned int *nack_count)
{
    if (rx_count) *rx_count = iic_rx_count;
    if (tx_count) *tx_count = iic_tx_count;
    if (nack_count) *nack_count = iic_nack_count;
}

