#include <math.h>
#include "OB38R16T1.h"
#include "HAL\ADC.h"
#include "board.h"
#include "sensors.h"

unsigned int adc_values[3] = {0, 0, 0}; // Store readings for CH0, CH1, CH2

extern unsigned int n_data;
extern bit ADCfinish;

void ADC_initialize(void)
{
	ADCCAL = 0x00; // Set ADCCAL=0x00;
	ADCCAL = 0x03; // Set ADCCAL=0x03;
	while ((ADCCAL & 0x01) == 0x01)
		; // End wait for ADC calibration

	ADCC1 = d_ADCnEN0;				   // Set ADC channel
	ADCCS = d_ADCnEN1 | d_ADC_CLK_Sel; // Set ADC channel & Select ADC clock
	IEADC = 1;						   // Enable ADC interrupt.
}

void ADC_poll(void)
{
	if (ADCfinish)
	{
		unsigned char adcChannel = ADCC2 & 0x0F;

		adc_values[adcChannel] = n_data;
		ADCfinish = 0;

		if (adcChannel++ > 2)
		{
			adcChannel = 0;
		}

		ADC_Start(adcChannel);
	}
}

MultiSwitchState get_multi_switch(unsigned int value)
{
	if (value > 3300)
	{
		return SWITCH_PWR; // 3723
	}

	if (value > 1700)
	{
		return SWITCH_S1; // 2048
	}

	if (value > 700)
	{
		return SWITCH_S2; // 1024
	}

	return SWITCH_NONE;
}

void sensors_update(SystemState *sys)
{
	sys->coffee.ntc_value = (unsigned int)(((-638) * log((float)(unsigned int)adc_values[0]) + 5674)); // ADC0: P0.4
	sys->steam.ntc_value = (unsigned int)(((-441) * log((float)(unsigned int)adc_values[1]) + 4164));  // ADC1: P0.5
	sys->multi_switch = get_multi_switch(adc_values[2]);											   // ADC2: P0.6
	sys->steam_switch = P0_0;
}
