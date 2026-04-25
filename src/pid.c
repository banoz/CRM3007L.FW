#include <stdint.h>
#include "OB38R16T1.h"
#include "pid.h"
#include "board.h"
#include "sensors.h"

#define PID_OUTPUT_MIN (0)
#define PID_OUTPUT_MAX (COFFEE_POWER_MAX)
#define PID_INTEGRAL_LIMIT (20000L)

/* 16-bit PID gain register map in n_DAT (little-endian). */
#define PID_REG_KP_L (14)
#define PID_REG_KP_H (15)
#define PID_REG_KI_L (16)
#define PID_REG_KI_H (17)
#define PID_REG_KD_L (18)
#define PID_REG_KD_H (19)

/* Separate gain scaling to represent values like [7.7, 0.008, 555]. */
#define PID_KP_SCALE (10L)
#define PID_KI_SCALE (1000L)
#define PID_KD_SCALE (1L)

/* Compile-time overflow guard: max ki (65535) * integral limit must fit in long. */
_Static_assert(65535UL * (unsigned long)PID_INTEGRAL_LIMIT <= 2147483647UL,
	"ki * pid_integral can overflow long — reduce PID_INTEGRAL_LIMIT");

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

static void pid_read_gains(uint16_t *kp, uint16_t *ki, uint16_t *kd)
{
	bit previous_ea = EA;

	EA = 0;
	*kp = (uint16_t)n_DAT[PID_REG_KP_L] | ((uint16_t)n_DAT[PID_REG_KP_H] << 8);
	*ki = (uint16_t)n_DAT[PID_REG_KI_L] | ((uint16_t)n_DAT[PID_REG_KI_H] << 8);
	*kd = (uint16_t)n_DAT[PID_REG_KD_L] | ((uint16_t)n_DAT[PID_REG_KD_H] << 8);
	EA = previous_ea;
}

void pid_initialize(void)
{
	n_DAT[REG_COFFEE_SETPOINT] = (unsigned char)PID_DEFAULT_SETPOINT;
	n_DAT[PID_REG_KP_L] = (unsigned char)(PID_DEFAULT_KP & 0xFF);
	n_DAT[PID_REG_KP_H] = (unsigned char)((PID_DEFAULT_KP >> 8) & 0xFF);
	n_DAT[PID_REG_KI_L] = (unsigned char)(PID_DEFAULT_KI & 0xFF);
	n_DAT[PID_REG_KI_H] = (unsigned char)((PID_DEFAULT_KI >> 8) & 0xFF);
	n_DAT[PID_REG_KD_L] = (unsigned char)(PID_DEFAULT_KD & 0xFF);
	n_DAT[PID_REG_KD_H] = (unsigned char)((PID_DEFAULT_KD >> 8) & 0xFF);
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
	uint16_t kp;
	uint16_t ki;
	uint16_t kd;

	if (current_temp == TEMP_ERROR_VALUE)
	{
		// TEMP_ERROR_VALUE (0xFFFF) indicates a sensor fault.
		pid_reset();
		return 0;
	}

	if (setpoint > 32767u)
	{
		setpoint = 32767u;
	}
	if (current_temp > 32767u)
	{
		current_temp = 32767u;
	}
	error = (int)setpoint - (int)current_temp;

	derivative = error - pid_last_error;
	pid_last_error = error;

	pid_read_gains(&kp, &ki, &kd);
	p_term = ((long)kp * (long)error) / PID_KP_SCALE;
	i_term = ((long)ki * pid_integral) / PID_KI_SCALE;
	d_term = ((long)kd * (long)derivative) / PID_KD_SCALE;
	output_sum = p_term + i_term + d_term;
	saturated_low = (output_sum <= PID_OUTPUT_MIN);
	saturated_high = (output_sum >= PID_OUTPUT_MAX);

	// Avoid integral windup when output is saturated in the direction of the error.
	integrate = !((saturated_low && error < 0) || (saturated_high && error > 0));
	if (integrate)
	{
		pid_integral = clamp_long(pid_integral + error, -PID_INTEGRAL_LIMIT, PID_INTEGRAL_LIMIT);
		i_term = ((long)ki * pid_integral) / PID_KI_SCALE;
		output_sum = p_term + i_term + d_term;
	}

	output = output_sum;
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
