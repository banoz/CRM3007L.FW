#include "OB38R16T1.h"
#include "HAL\timer.h"
#include "HAL\UART.h"
#include "HAL\WDT.h"
#include "board.h"

#define TICK_INTERVAL_MS 30  // Main control loop interval in milliseconds

void delay(unsigned long cnt)
{
	while (cnt--)
		;
}

unsigned long nextTick = 0;
unsigned long lastTick = 0;

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

			WDTR_CountClear();
		}

		if (lastTick < getTick())
		{
			lastTick = getTick();
			check_zc();
		}

		delay(1UL);
	}
}