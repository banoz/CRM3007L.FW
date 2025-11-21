#ifndef __PWM_H__
#define __PWM_H__

// Example Code
/*
#include "PWM.h"
#include "OB38R16T1.h"

void main(void) //Main Function Start
{
    PWM_initialize();         //Call PWM Initial Subroutine
    PWM_Output(g_PWMD0, g_PWMD1, g_PWMD2, g_PWMD3);
    while(1)
    {
        ;
    }
    //PWM_Disable();
}
*/

//extern unsigned int g_PWMD0;        //PWMD0[9:0]=Duty (PWM Channel 0 Data Register 0~1023)
//extern unsigned int g_PWMD1;        //PWMD1[9:0]=Duty (PWM Channel 1 Data Register 0~1023)
//extern unsigned int g_PWMD2;        //PWMD2[9:0]=Duty (PWM Channel 2 Data Register 0~1023)
//extern unsigned int g_PWMD3;        //PWMD3[9:0]=Duty (PWM Channel 3 Data Register 0~1023)

void PWM_initialize(void);
void PWM_Output(unsigned int PWMD0, unsigned int PWMD1,
                unsigned int PWMD2, unsigned int PWMD3);

void PWM_Output2(unsigned int PWMD2);
void PWM_Output3(unsigned int PWMD3);

void PWM_Enable(void);
void PWM_Disable(void);
void PWM_ISR(void);

#endif
