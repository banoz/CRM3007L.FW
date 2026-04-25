#include <stdint.h>
#include "OB38R16T1.h"
#include "HAL/ADC.h"
#include "board.h"
#include "sensors.h"

/* Number of entries in the lookup table */
#define NTC_TABLE_SIZE  12
 
/* Lookup table: {coffee_adc, steam_adc, temp_x10} */
/* temp_x10 is temperature in tenths of a degree (e.g. 860 = 86.0°C) */
static const __code uint16_t ntc_table[NTC_TABLE_SIZE][3] = {
    {  3157,  3167,   500 },  /*  50 deg C */
    {  2844,  2858,   600 },  /*  60 deg C */
    {  2501,  2518,   700 },  /*  70 deg C */
    {  2147,  2165,   800 },  /*  80 deg C */
    {  1939,  1960,   860 },  /*  86 deg C */
    {  1872,  1892,   880 },  /*  88 deg C */
    {  1806,  1826,   900 },  /*  90 deg C */
    {  1743,  1761,   920 },  /*  92 deg C */
    {  1678,  1698,   940 },  /*  94 deg C */
    {  1616,  1635,   960 },  /*  96 deg C */
    {  1497,  1511,  1000 },  /* 100 deg C */
    {  1225,  1227,  1100 }  /* 110 deg C */
};

volatile unsigned int adc_values[3] = {0, 0, 0}; // Store readings for CH0, CH1, CH2

extern unsigned int n_data;
extern bit ADCfinish;

static unsigned int interpolate_ntc_temperature(unsigned int adc_value, unsigned char adc_column)
{
	int32_t adc0;
	int32_t adc1;
	int32_t temp0;
	int32_t temp1;
	int32_t denominator;
	int32_t numerator;
	int32_t temp;
	unsigned char i;

	if (adc_value >= ntc_table[0][adc_column])
	{
		adc0 = (int32_t)ntc_table[0][adc_column];
		adc1 = (int32_t)ntc_table[1][adc_column];
		temp0 = (int32_t)ntc_table[0][2];
		temp1 = (int32_t)ntc_table[1][2];
		denominator = adc1 - adc0;
		numerator = ((int32_t)adc_value - adc0) * (temp1 - temp0);
		temp = temp0 + (numerator / denominator);

		if (temp < 0)
		{
			return 0;
		}
		if (temp > 2500)
		{
			return 2500;
		}

		return (unsigned int)temp;
	}

	if (adc_value <= ntc_table[NTC_TABLE_SIZE - 1][adc_column])
	{
		adc0 = (int32_t)ntc_table[NTC_TABLE_SIZE - 2][adc_column];
		adc1 = (int32_t)ntc_table[NTC_TABLE_SIZE - 1][adc_column];
		temp0 = (int32_t)ntc_table[NTC_TABLE_SIZE - 2][2];
		temp1 = (int32_t)ntc_table[NTC_TABLE_SIZE - 1][2];
		denominator = adc1 - adc0;
		numerator = ((int32_t)adc_value - adc0) * (temp1 - temp0);
		temp = temp0 + (numerator / denominator);

		if (temp < 0)
		{
			return 0;
		}
		if (temp > 2500)
		{
			return 2500;
		}

		return (unsigned int)temp;
	}

	for (i = 0; i < (NTC_TABLE_SIZE - 1); i++)
	{
		unsigned int adc_high = ntc_table[i][adc_column];
		unsigned int adc_low = ntc_table[i + 1][adc_column];

		if ((adc_value <= adc_high) && (adc_value >= adc_low))
		{
			adc0 = (int32_t)adc_high;
			adc1 = (int32_t)adc_low;
			temp0 = (int32_t)ntc_table[i][2];
			temp1 = (int32_t)ntc_table[i + 1][2];
			denominator = adc1 - adc0;
			numerator = ((int32_t)adc_value - adc0) * (temp1 - temp0);
			temp = temp0 + (numerator / denominator);

			if (temp < 0)
			{
				return 0;
			}
			if (temp > 2500)
			{
				return 2500;
			}

			return (unsigned int)temp;
		}
	}

	return 0;
}

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
	// Sensor fault detection - check for out-of-range ADC values
	if (adc_value < ADC_MIN_VALID || adc_value > ADC_MAX_VALID)
	{
		return TEMP_ERROR_VALUE; // Sensor fault (open/short circuit)
	}

	return interpolate_ntc_temperature(adc_value, 0);
}

unsigned int map_steam_boiler_temperature(unsigned int adc_value)
{
	// Sensor fault detection - check for out-of-range ADC values
	if (adc_value < ADC_MIN_VALID || adc_value > ADC_MAX_VALID)
	{
		return TEMP_ERROR_VALUE; // Sensor fault (open/short circuit)
	}

	return interpolate_ntc_temperature(adc_value, 1);
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
