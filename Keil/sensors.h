#ifndef __SENSORS_H__
#define __SENSORS_H__

void ADC_initialize(void);

void ADC_poll(void);

void sensors_update(SystemState *);

#endif