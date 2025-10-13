#include "OB38R16T1.h"
#include "WDT.h"

#define WDT_VECTOR  17          //WDT Interrupt Vector
#define d_WDTIE     1           //WDT Enable Bit
#define d_WDTIM     0x07        //WDTM[3:0] WDT Clock Source Divider Bit (0~F)
#define d_IEWDT     1           //WDT interrupt enable if set 0: Disable WDT interrupt, set 1: Enable WDT interrupt


void WDTI_Init(void)            //Initialize WDT
{
    EA    = 0;                         //for WDT Interrupt 
    TAKEY = 0x55;                      //**************************************//
    TAKEY = 0xAA;                      //Set To Enable The WDTC Write Attribute//
    TAKEY = 0x5A;                      //**************************************//
    WDTIC = (d_WDTIM) | (d_WDTIE<<5);  //Set WDT Reset Time and Enable WDT and select RST/Interrupt
    IEN2  = (d_IEWDT<<1);              //for WDT Interrupt 
    EA    = 1;                         //for WDT Interrupt 
}

void WDTI_CountClear(void)
{
    WDTK = 0xAA;                       //WDT Timer Clear To Zero
}

void WDTI_Disable(void)
{
    TAKEY = 0x55;                      //**************************************//
    TAKEY = 0xAA;                      //Set To Enable The WDTC Write Attribute//
    TAKEY = 0x5A;                      //**************************************//
    WDTIC = 0x00;                      //Disable WDT Function
}

void WDT_ISR(void) interrupt WDT_VECTOR //WDT Interrupt Subroutine
{
    P0 = ~P0;                          //When Into interrupt subroutine WDTIF(IRCON2&0x02) will automatic clear 
}
