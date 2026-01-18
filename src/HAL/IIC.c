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

//=====================================================================

void IIC_ISR(void) interrupt d_IIC_Vector
{	
	if(RXIF)
	{
		RXIF = 0;
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
		if(RXAK == 0)
		{
			IICRWD = n_DAT[n_Addr];
			n_Addr++;
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
}



