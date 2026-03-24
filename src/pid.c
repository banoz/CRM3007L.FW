#include "pid.h"
#include "board.h"
#include "sensors.h"

#define PID_OUTPUT_MIN (0)
#define PID_OUTPUT_MAX (COFFEE_POWER_MAX)
#define PID_INTEGRAL_LIMIT (20000L)
#define CLAMP_LONG(value, min, max) ((value) < (min) ? (min) : ((value) > (max) ? (max) : (value)))

extern volatile unsigned char n_DAT[];

static __xdata long pid_integral = 0;
static __xdata int pid_last_error = 0;
static __xdata unsigned char pid_output = 0;

void pid_initialize(void)
{
	n_DAT[REG_COFFEE_SETPOINT] = (unsigned char)PID_DEFAULT_SETPOINT;
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
	int error;
	int derivative;
	long output;
	long output_sum;
	long p_term;
	long i_term;
	long d_term;
	unsigned char saturated_low;
	unsigned char saturated_high;
	unsigned char integrate;

	if (current_temp == TEMP_ERROR_VALUE)
	{
		pid_reset();
		return pid_output;
	}

	error = (int)setpoint - (int)current_temp;

	derivative = error - pid_last_error;
	pid_last_error = error;

	p_term = (long)n_DAT[REG_PID_KP] * (long)error;
	i_term = (long)n_DAT[REG_PID_KI] * pid_integral;
	d_term = (long)n_DAT[REG_PID_KD] * (long)derivative;
	output_sum = p_term + i_term + d_term;
	saturated_low = (output_sum <= ((long)PID_OUTPUT_MIN * PID_COEFF_SCALE));
	saturated_high = (output_sum >= ((long)PID_OUTPUT_MAX * PID_COEFF_SCALE));

	// Avoid integral windup when output is saturated in the direction of the error.
	integrate = !((saturated_low && error < 0) || (saturated_high && error > 0));
	if (integrate)
	{
		pid_integral = CLAMP_LONG(pid_integral + error, -PID_INTEGRAL_LIMIT, PID_INTEGRAL_LIMIT);
		i_term = (long)n_DAT[REG_PID_KI] * pid_integral;
		output_sum = p_term + i_term + d_term;
	}

	output = output_sum / PID_COEFF_SCALE;
	if (output < PID_OUTPUT_MIN)
	{
		output = PID_OUTPUT_MIN;
	}
	else if (output > PID_OUTPUT_MAX)
	{
		output = PID_OUTPUT_MAX;
	}

	pid_output = (unsigned char)output;
	return pid_output;
}
