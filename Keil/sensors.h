#ifndef __SENSORS_H__
#define __SENSORS_H__

#define COFFEE_NTC_A (5674)
#define COFFEE_NTC_B (-638)
#define STEAM_NTC_A (4164)
#define STEAM_NTC_B (-441)

void ADC_initialize(void);

void ADC_poll(void);

void sensors_update(void);

#endif