#include "OB38R16T1.h"
#include "HAL\timer.h"
#include "HAL\UART.h"
#include "HAL\WDT.h"
#include "board.h"

#define TICK_INTERVAL_MS 30  // Main control loop interval in milliseconds
#define COUNTER_MAX 99       // Maximum counter value for periodic operations

void ADC_ISR(void) interrupt d_ADC_Vector;
void IIC_ISR(void) interrupt d_IIC_Vector;
void INT0_ISR(void) interrupt d_INT0_Vector;
void PWM_ISR(void) interrupt d_PWM_Vector;
void TIMER0_ISR(void) interrupt d_T0_Vector;
void UART_ISR(void) interrupt d_UART_Vector;

unsigned long nextTick = 0;
unsigned long lastTick = 0;

unsigned int cntr1 = 0;

void main(void)
{
	board_initialize();

	// WDTR_Init(); // TODO this breaks ADC, fix later

	nextTick = getTick();

	while (1)
	{
		if (nextTick < getTick())
		{
			nextTick = getTick() + TICK_INTERVAL_MS;

			board_tick();

			if (cntr1++ > COUNTER_MAX)
				cntr1 = 0;

			WDTR_CountClear();
		}

		if (lastTick < getTick())
		{
			lastTick = getTick();
			check_zc();
		}
	}

	goToISP();
}