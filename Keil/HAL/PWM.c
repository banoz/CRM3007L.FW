#include "OB38R16T1.h"
#include "PWM.h"

#define PWM_VECTOR  8           //PWM Interrupt Vector
#define d_PWMCS     0x06        //PWMCS[2:0] (PWM clock select 0~7) - Timer0 - 1ms
#define d_PWMXEN    0x06        //PWM Channel Enable (0~F)
#define d_PWM0PS    0x00        //PWM0 Channel polarity select
#define d_PWM1PS    0x00        //PWM1 Channel polarity select
#define d_PWM2PS    0x00        //PWM2 Channel polarity select
#define d_PWM3PS    0x00        //PWM3 Channel polarity select
#define d_PWMIE     0x01        //PWM Interrupt Enable bit
#define d_PWMOMS    0x00
#define d_DTP0      0x00
#define d_DT0       0x00
#define d_DTP1      0x00
#define d_DT1       0x00

#define PWMMD       0x0063        //PWMMD[9:0]=Period (PWM Max Data Register 0~99) - 10Hz

void PWM_initialize(void) // Initialize PWM
{
    IEPWM = (d_PWMIE); // Enable PWM Interrupt Function
    PWMMDH = PWMMD >> 8;
    PWMMDL = PWMMD;
    PWMDT0 = (d_DTP0 << 6) | d_DT0;
    PWMDT1 = (d_DTP1 << 6) | d_DT1;
    PWMC2 = (d_PWM3PS << 3) | (d_PWM2PS << 2) | (d_PWM1PS << 1) | (d_PWM0PS);
}

void PWM_Output(unsigned int PWMD0, unsigned int PWMD1,
                unsigned int PWMD2, unsigned int PWMD3) // PWM Output Set
{
    if (PWMD0 > PWMMD)
    {
        PWMD0 = PWMMD;
    }
    if (PWMD1 > PWMMD)
    {
        PWMD1 = PWMMD;
    }
    if (PWMD2 > PWMMD)
    {
        PWMD2 = PWMMD;
    }
    if (PWMD3 > PWMMD)
    {
        PWMD3 = PWMMD;
    }

    PWMD0H = (PWMD0 >> 8);
    PWMD0L = (PWMD0);
    PWMD1H = (PWMD1 >> 8);
    PWMD1L = (PWMD1);
    PWMD2H = (PWMD2 >> 8);
    PWMD2L = (PWMD2);
    PWMD3H = (PWMD3 >> 8);
    PWMD3L = (PWMD3);
    PWMC = PWMC | (d_PWMCS << 5) | (d_PWMOMS << 4) | (d_PWMXEN);
}

void PWM_Disable(void)
{
    IEPWM = 0; // Disable PWM Interrupt
    PWMC = 0;  // Disable PWM Function
}

void PWM_ISR(void) interrupt PWM_VECTOR // PWM Interrupt Subroutine
{
    PWMIF = 0;
}
