#include "OB38R16T1.h"
#include "board.h"
#include "sensors.h"
#include "HAL\ADC.h"
#include "HAL\IIC.h"
#include "HAL\timer.h"
#include <stdlib.h> // For atoi, _itoa, etc.
#include <stdio.h>	// For sprintf

#define STATUS_LED P0_1
#define PUMP P1_1
#define H_STEAM = P1_6
#define H_COFFEE = P1_7

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
	P0_7 = 0;	 // LED common off
	// Set P1.1,1.6,1.7 (pump, heaters off)
	P1_1 = 1;
	P1_6 = 1;
	P1_7 = 1;
	P2_3 = 0;  // PWR LED off
	P3 = 0x00; // Valves, buzzer off

	// INT0 init
	IT0 = 1;
	EX0 = 1;

	// Timer init
	TIMER0_initialize();
	TR0 = 1;

	// ADC init
	ADC_initialize();

	// Initialize UART
	// init_UART();

	IIC_init_slave();

	EA = 1;

	ADC_Start(2); // poll the buttons first
}

SystemState system_state;

unsigned int psm_value = 0;
unsigned int psm_counter = 0;

char str_buf[16];

extern unsigned char n_DAT[];

void board_tick(void)
{
	char *str_ptr;

	sensors_update(&system_state);

	n_DAT[1] = system_state.coffee.ntc_value & 0xFF;
	n_DAT[2] = system_state.coffee.ntc_value >> 8;
	n_DAT[3] = system_state.steam.ntc_value & 0xFF;
	n_DAT[4] = system_state.steam.ntc_value >> 8;

	STATUS_LED = !STATUS_LED;
}

/// PSM

unsigned int psm_a = 0;
unsigned int psm_range = 100;

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

void on_zc(void)
{
	ADC_poll();

	calculateSkip();
}

void INT0Interrupt(void) interrupt d_INT0_Vector
{
	IE0 = 0;
	on_zc();
}
