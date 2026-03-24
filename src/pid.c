#include "OB38R16T1.h"
#include "pid.h"
#include "board.h"
#include "sensors.h"

#define PID_OUTPUT_MIN (0)
#define PID_OUTPUT_MAX (COFFEE_POWER_MAX)
#define PID_INTEGRAL_LIMIT (20000L)

// PID coefficients are sourced from the I2C register bank to avoid extra copies.
extern volatile unsigned char n_DAT[];

// Store PID state in xdata to preserve limited internal RAM.
static __xdata long pid_integral = 0;
static __xdata int pid_last_error = 0;
static __xdata unsigned char pid_output = 0;

static long clamp_long(long value, long min_value, long max_value)
{
	if (value < min_value)
	{
		return min_value;
	}
	if (value > max_value)
	{
		return max_value;
	}
	return value;
}

static void pid_read_gains(unsigned char *kp, unsigned char *ki, unsigned char *kd)
{
	bit previous_ea = EA;

	EA = 0;
	*kp = n_DAT[REG_PID_KP];
	*ki = n_DAT[REG_PID_KI];
	*kd = n_DAT[REG_PID_KD];
	EA = previous_ea;
}

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
	unsigned char kp;
	unsigned char ki;
	unsigned char kd;

	if (current_temp == TEMP_ERROR_VALUE)
	{
		// TEMP_ERROR_VALUE (0xFFFF) indicates a sensor fault.
		pid_reset();
		return 0;
	}

	error = (int)setpoint - (int)current_temp;

	derivative = error - pid_last_error;
	pid_last_error = error;

	pid_read_gains(&kp, &ki, &kd);
	p_term = (long)kp * (long)error;
	i_term = (long)ki * pid_integral;
	d_term = (long)kd * (long)derivative;
	output_sum = p_term + i_term + d_term;
	saturated_low = (output_sum <= ((long)PID_OUTPUT_MIN * PID_COEFF_SCALE));
	saturated_high = (output_sum >= ((long)PID_OUTPUT_MAX * PID_COEFF_SCALE));

	// Avoid integral windup when output is saturated in the direction of the error.
	integrate = !((saturated_low && error < 0) || (saturated_high && error > 0));
	if (integrate)
	{
		pid_integral = clamp_long(pid_integral + error, -PID_INTEGRAL_LIMIT, PID_INTEGRAL_LIMIT);
		i_term = (long)ki * pid_integral;
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
