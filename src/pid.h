#pragma once

#define PID_COEFF_SCALE (100)
#define PID_DEFAULT_SETPOINT (0)
#define PID_DEFAULT_KP (0)
#define PID_DEFAULT_KI (0)
#define PID_DEFAULT_KD (0)

void pid_initialize(void);
void pid_reset(void);
unsigned char pid_tick(unsigned int current_temp, unsigned int setpoint);
