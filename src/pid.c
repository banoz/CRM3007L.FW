#include "pid.h"
#include "board.h"
#include "sensors.h"

#define PID_OUTPUT_MIN (0)
#define PID_OUTPUT_MAX (0x7F)
#define PID_INTEGRAL_LIMIT (20000L)
#define CLAMP_LONG(value, min, max) ((value) < (min) ? (min) : ((value) > (max) ? (max) : (value)))

extern volatile unsigned char n_DAT[];

static __xdata long pid_integral = 0;
static __xdata int pid_last_error = 0;
static __xdata unsigned char pid_output = 0;

void pid_initialize(void)
{
	n_DAT[REG_COFFEE_SETPOINT_L] = (unsigned char)(PID_DEFAULT_SETPOINT & 0xFF);
	n_DAT[REG_COFFEE_SETPOINT_H] = (unsigned char)(PID_DEFAULT_SETPOINT >> 8);
	n_DAT[REG_PID_KP] = PID_DEFAULT_KP;
	n_DAT[REG_PID_KI] = PID_DEFAULT_KI;
	n_DAT[REG_PID_KD] = PID_DEFAULT_KD;
	pid_reset();
}

void pid_reset(void)
{
	pid_integral = 0;
	pid_last_error = 0;
	pid_output = 0;
}

unsigned char pid_tick(unsigned int current_temp, unsigned int setpoint)
{
	int error = 0;
	int derivative = 0;
	long output = 0;
	long integral_candidate = 0;
	long p_term = 0;
	long i_term = 0;
	long d_term = 0;

	if (current_temp == TEMP_ERROR_VALUE)
	{
		pid_reset();
		return pid_output;
	}

	error = (int)setpoint - (int)current_temp;
	integral_candidate = CLAMP_LONG(pid_integral + error, -PID_INTEGRAL_LIMIT, PID_INTEGRAL_LIMIT);

	derivative = error - pid_last_error;
	pid_last_error = error;

	p_term = (long)n_DAT[REG_PID_KP] * (long)error;
	i_term = (long)n_DAT[REG_PID_KI] * integral_candidate;
	d_term = (long)n_DAT[REG_PID_KD] * (long)derivative;

	output = (p_term + i_term + d_term) / PID_COEFF_SCALE;

	if ((output <= PID_OUTPUT_MIN && error < 0) ||
		(output >= PID_OUTPUT_MAX && error > 0))
	{
		integral_candidate = pid_integral;
		i_term = (long)n_DAT[REG_PID_KI] * integral_candidate;
		output = (p_term + i_term + d_term) / PID_COEFF_SCALE;
	}

	if (output < PID_OUTPUT_MIN)
	{
		output = PID_OUTPUT_MIN;
	}
	else if (output > PID_OUTPUT_MAX)
	{
		output = PID_OUTPUT_MAX;
	}

	pid_integral = integral_candidate;
	pid_output = (unsigned char)output;
	return pid_output;
}
