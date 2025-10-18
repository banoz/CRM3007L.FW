#include "OB38R16T1.h"
#include "HAL\timer.h"
#include "HAL\UART.h"
#include "HAL\WDT.h"
#include "board.h"

void delay(unsigned long cnt)
{
	while (cnt--)
		;
}

unsigned long nextTick = 0;

void main(void)
{
	board_initialize();

	WDTI_Init();

	nextTick = getTick();

	while (1)
	{
		if (nextTick < getTick())
		{
			nextTick = getTick() + 30;

			board_tick();
		}

		delay(1UL);
		
		WDTI_CountClear();
	}
}