#include "OB38R16T1.h"
#include "board.h"
#include "sensors.h"
#include "HAL\ADC.h"
#include "HAL\IIC.h"
#include "HAL\PWM.h"
#include "HAL\timer.h"
#include <stdlib.h> // For atoi, _itoa, etc.
#include <stdio.h>	// For sprintf

#define STATUS_LED P0_1
#define S1_LED P0_2
#define S2_LED P0_3
#define PWR_LED P2_3
#define BUZZER P3_3

#define PUMP P1_1
#define H_STEAM P1_6
#define H_COFFEE P1_7

#define K1_3WV P3_2
#define K2_STEAM_VALVE P3_1
#define K3_COFFEE_VALVE P3_0

void board_initialize(void)
{
	// P0M1 = 0b01110001 (0x71), P0M0 = 0b10001110 (0x8E)
	P0M1 = 0x71;
	P0M0 = 0x8E;
	// P1M1 = 0b11000010 (0xC2), P1M0 = 0b11000010 (0xC2)
	P1M1 = 0xC2;
	P1M0 = 0xC2;
	// P2M1 = 0x00, P2M0 = 0b00001000 (0x08)
	P2M1 = 0x00;
	P2M0 = 0x08;
	// P3M1 = 0x00, P3M0 = 0b00001111 (0x0F)
	P3M1 = 0x00;
	P3M0 = 0x0F;

	// Optional: Initialize outputs to safe state (off, assuming active high)
	P0 &= ~0x0E; // Clear P0.1,0.2,0.3 (LEDs off)
	P0_7 = 0;	 // LED common pull-down
	// Set P1.1,1.6,1.7 (pump, heaters off)
	P1_1 = 1;
	P1_6 = 1;
	P1_7 = 1;
	P2_3 = 0;  // PWR LED off
	P3 = 0x00; // Valves, buzzer off

	EA = 0;

	// Timer init
	TIMER0_initialize();
	TR0 = 1;

	// Peripherals init
	ADC_initialize();

	IIC_init_slave();

	PWM_initialize();

	// INT0 init
	IT0 = 1;
	EX0 = 1;

	EA = 1;

	ADC_Start(2); // poll the buttons first
}

SystemState system_state = {0};

unsigned char psm_range = 0x7F;
unsigned char psm_value = 0;
unsigned int psm_counter = 0;

char str_buf[16];

extern unsigned char n_DAT[];

void set_controls(unsigned char);
void set_valves(unsigned char);
void set_pump_power(unsigned char);
void set_coffee_power(unsigned char, unsigned int);
void set_steam_power(unsigned char, unsigned int);
unsigned int map_coffee_boiler_temperature(unsigned int);
unsigned int map_steam_boiler_temperature(unsigned int);

void board_tick(void)
{
	unsigned int coffee_temp = 0; // degree C * 10
	unsigned int steam_temp = 0;  // degree C * 10

	sensors_update();

	coffee_temp = map_coffee_boiler_temperature(system_state.coffee.ntc_value);
	steam_temp = map_steam_boiler_temperature(system_state.steam.ntc_value);

	// TODO consider implementing IIC mutex

	n_DAT[0] = system_state.multi_switch | (system_state.steam_switch << 4);
	n_DAT[1] = coffee_temp & 0xFF;
	n_DAT[2] = coffee_temp >> 8;
	n_DAT[3] = steam_temp & 0xFF;
	n_DAT[4] = steam_temp >> 8;

	n_DAT[6] = psm_counter & 0xFF;

	set_controls(n_DAT[8]);					  // n_DAT[8]
	set_valves(n_DAT[9]);					  // n_DAT[9]
	set_pump_power(n_DAT[10]);				  // n_DAT[10]
	set_coffee_power(n_DAT[11], coffee_temp); // n_DAT[11]
	set_steam_power(n_DAT[12], steam_temp);	  // n_DAT[12]
}

void set_controls(unsigned char control_value) // n_DAT[8]
{
	STATUS_LED = (control_value >> 0) & 0x01;
	PWR_LED = (control_value >> 1) & 0x01;
	S1_LED = (control_value >> 2) & 0x01;
	S2_LED = (control_value >> 3) & 0x01;

	if ((control_value >> 7) & 0x01)
	{
		psm_counter = 0;
	}
}

void set_valves(unsigned char control_value) // n_DAT[9]
{
	K3_COFFEE_VALVE = (control_value >> 0) & 0x01;
	K2_STEAM_VALVE = (control_value >> 1) & 0x01;
	K1_3WV = (control_value >> 2) & 0x01;
}

void set_pump_power(unsigned char control_value) // n_DAT[10]
{
	if (control_value > psm_range)
	{
		control_value = psm_range;
	}

	psm_value = control_value;
}

void set_coffee_power(unsigned char control_value, unsigned int current_temp) // n_DAT[11]
{
	if (current_temp > 1200) // cap at 120C
	{
		control_value = 0;
	}

	PWM_Output(0, 0, control_value, 0); // 0~99 range
}

void set_steam_power(unsigned char control_value, unsigned int current_temp) // n_DAT[12]
{
	if (current_temp > 1800) // cap at 180C
	{
		control_value = 0;
	}

	PWM_Output(0, control_value, 0, 0); // 0~99 range
}

/// PSM

unsigned int psm_a = 0;
volatile bit zero_crossed = 0;

void calculateSkip(void)
{
	bit psm_skip = 0;

	psm_a += psm_value;

	if (psm_a >= psm_range)
	{
		psm_a -= psm_range;
		psm_skip = 0;
	}
	else
	{
		psm_skip = 1;
	}

	if (psm_a > psm_range)
	{
		psm_a = 0;
		psm_skip = 0;
	}

	if (!psm_skip)
	{
		psm_counter++;
	}

	PUMP = psm_skip;
}

void check_zc(void)
{
	if (zero_crossed)
	{
		ADC_poll();

		calculateSkip();

		zero_crossed = 0;
	}
}

void INT0Interrupt(void) interrupt d_INT0_Vector
{
	zero_crossed = 1;
	IE0 = 0;
}
