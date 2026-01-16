#include "OB38R16T1.h"

#define d_T0MOD        0x01 //TMOD[3:0] TIMER0 Mode setting
#define d_T1MOD        0x00 //TMOD[7:4] TIMER1 Mode setting

#define d_MODEVALUE_T0      65536     //MODE1 16bit 65536
#define d_RELOADVALUE_T0    1310      //Reload value to get 1 ms tick

unsigned long halTick = 0;

unsigned long getTick(void) 
{
	return halTick;
}

void TIMER0_initialize(void)  //Initialize TIMER0
{
    ET0 = 1;
    TMOD  = (d_T1MOD<<4)|(d_T0MOD);
    TH0   = (d_MODEVALUE_T0-d_RELOADVALUE_T0)/256;
    TL0   = (d_MODEVALUE_T0-d_RELOADVALUE_T0)%256;
}

void TIMER0_ISR(void) interrupt d_T0_Vector
{
    TH0 = (d_MODEVALUE_T0-d_RELOADVALUE_T0)/256;    //MODE1 16bit 65536
    TL0 = (d_MODEVALUE_T0-d_RELOADVALUE_T0)%256;
    
    halTick++;    
}