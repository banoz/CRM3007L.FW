#ifndef __SENSORS_H__
#define __SENSORS_H__

#include "board.h"

// Temperature mapping uses lookup-table linear interpolation in sensors.c
// Units: decidegrees Celsius (e.g., 950 = 95.0°C)

// Sensor fault detection thresholds
#define ADC_MIN_VALID (10)    // Minimum valid ADC reading (open circuit detection)
#define ADC_MAX_VALID (4085)  // Maximum valid ADC reading (short circuit detection)
#define TEMP_ERROR_VALUE (0xFFFF)  // Return value indicating sensor error

void ADC_initialize(void);

void ADC_poll(void);

void sensors_update(void);

// Temperature mapping functions (return TEMP_ERROR_VALUE on sensor fault)
unsigned int map_coffee_boiler_temperature(unsigned int adc_value);
unsigned int map_steam_boiler_temperature(unsigned int adc_value);

// Multi-switch state detection based on analog voltage divider
MultiSwitchState get_multi_switch(unsigned int value);

// ADC value thresholds for multi-switch detection
#define SWITCH_PWR_THRESHOLD  (3300)  // ADC value for 1K resistor (3723 typical)
#define SWITCH_S1_THRESHOLD   (1700)  // ADC value for 10K resistor (2048 typical)
#define SWITCH_S2_THRESHOLD   (700)   // ADC value for 30K resistor (1024 typical)

#endif