#include <math.h>
#include "OB38R16T1.h"
#include "HAL\ADC.h"
#include "board.h"
#include "sensors.h"

volatile unsigned int adc_values[3] = {0, 0, 0}; // Store readings for CH0, CH1, CH2

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

		if (++adcChannel > 2)
		{
			adcChannel = 0;
		}

		ADC_Start(adcChannel);

		ADCfinish = 0;
	}
}

unsigned int map_coffee_boiler_temperature(unsigned int adc_value)
{
	float temp;
	
	// Sensor fault detection - check for out-of-range ADC values
	if (adc_value < ADC_MIN_VALID || adc_value > ADC_MAX_VALID)
	{
		return TEMP_ERROR_VALUE; // Sensor fault (open/short circuit)
	}
	
	// Calculate temperature using NTC formula
	temp = COFFEE_NTC_A + (COFFEE_NTC_B * logf((float)adc_value));
	
	// Validate temperature range (0°C to 250°C)
	if (temp < 0)
		return 0;
	if (temp > 2500)
		return 2500;
	
	return (unsigned int)temp;
}

unsigned int map_steam_boiler_temperature(unsigned int adc_value)
{
	float temp;
	
	// Sensor fault detection - check for out-of-range ADC values
	if (adc_value < ADC_MIN_VALID || adc_value > ADC_MAX_VALID)
	{
		return TEMP_ERROR_VALUE; // Sensor fault (open/short circuit)
	}
	
	// Calculate temperature using NTC formula
	temp = STEAM_NTC_A + (STEAM_NTC_B * logf((float)adc_value));
	
	// Validate temperature range (0°C to 250°C)
	if (temp < 0)
		return 0;
	if (temp > 2500)
		return 2500;
	
	return (unsigned int)temp;
}

MultiSwitchState get_multi_switch(unsigned int value)
{
	if (value > SWITCH_PWR_THRESHOLD)
	{
		return SWITCH_PWR;
	}

	if (value > SWITCH_S1_THRESHOLD)
	{
		return SWITCH_S1;
	}

	if (value > SWITCH_S2_THRESHOLD)
	{
		return SWITCH_S2;
	}

	return SWITCH_NONE;
}

/**
 * @brief Update all sensor readings into system_state structure
 * @note Reads ADC values and converts to appropriate units/states
 * @note Updates coffee/steam NTC values, multi-switch state, and steam switch state
 */
void sensors_update(void)
{
	system_state.coffee.ntc_value = adc_values[0];				 // ADC0: P0.4
	system_state.steam.ntc_value = adc_values[1];				 // ADC1: P0.5
	system_state.multi_switch = get_multi_switch(adc_values[2]); // ADC2: P0.6
	system_state.steam_switch = !P0_0;
}
