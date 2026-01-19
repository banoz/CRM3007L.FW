#include "OB38R16T1.h"
#include "ADC.h"

unsigned int n_data    = 0x0000;
volatile bit ADCfinish = 0;

void ADC_Start(unsigned char n_ADC_CH)
{  
    ADCC2  = n_ADC_CH; //Set a new channel
    ADCC2 |= 0x80;      //ADC start conversion
}

unsigned int ADC_Read12(void) //12-bit ADC
{
    unsigned int n_ADCD12 = 0x0000;
    n_ADCD12 = (ADCDH<<4) | ADCDL;     //12-bit ADC
    return n_ADCD12;
}
    
void ADC_ISR(void) interrupt d_ADC_Vector // ADC Interrupt Vector Address 0053h => interrupt 10
{
    n_data = ADC_Read12(); //Read ADC
    ADCfinish = 1;
}
