#include "OB38R16T1.h"
#include "board.h"
#include "sensors.h"
#include "HAL\ADC.h"
#include "HAL\IIC.h"
#include "HAL\PWM.h"
#include "HAL\timer.h"
#include "HAL\UART.h"
#include <stdlib.h> // For atoi, _itoa, etc.
#include <stdio.h>	// For sprintf

#define STATUS_LED P0_1
#define S1_LED P0_2
#define S2_LED P0_3
#define PWR_LED P2_3
#define BUZZER P3_3

#define PUMP P1_1
#define H_STEAM P1_7
#define H_COFFEE P1_6

#define K1_3WV P3_2
#define K2_STEAM_VALVE P3_1
#define K3_COFFEE_VALVE P3_0

// Temperature safety limits (in decidegrees C: value * 10)
#define COFFEE_TEMP_MAX (1200) // 120°C - maximum safe temperature for coffee boiler
#define STEAM_TEMP_MAX (1800)  // 180°C - maximum safe temperature for steam boiler

typedef struct psm_state
{
	unsigned int psm_a;		  // PSM accumulator
	unsigned char psm_range;  // PSM range: 0-127 (7-bit control)
	unsigned char psm_value;  // Current PSM power setting
	unsigned int psm_counter; // Count of pump activation cycles
} psm_state;

__xdata SystemState system_state;

/// Pulse Skip Modulation (PSM) for pump control
// PSM is used to control pump speed by skipping AC half-cycles
// psm_value: desired pump power (0 to psm_range)
// psm_range: maximum range (0x7F = 127)
// psm_counter: counts active pump cycles for monitoring

__xdata psm_state pump_psm = {0, 0x7F, 0, 0};		   // Pump PSM (0-127)
__xdata psm_state coffee_boiler_psm = {0, 0x7F, 0, 0}; // Coffee boiler PSM (0-127)

volatile bit zero_crossed = 0; // Flag set by INT0 on zero-crossing detection

extern unsigned char n_DAT[];

void set_controls(unsigned char);
void set_valves(unsigned char);
void set_pump_power(unsigned char);
void set_coffee_power(unsigned char, unsigned int);
void set_steam_power(unsigned char, unsigned int);
unsigned int map_coffee_boiler_temperature(unsigned int);
unsigned int map_steam_boiler_temperature(unsigned int);

/**
 * @brief Initialize board hardware and peripherals
 * @note Configures GPIO, timers, ADC, I2C, PWM, and external interrupt
 * @note Sets all outputs to safe initial state (off)
 */
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

	P0_2 = 1;
	P0_3 = 1;
	P2_3 = 1;
	if (P0_6)
	{
		goToISP();
	}
	P0_2 = 0;
	P0_3 = 0;
	P2_3 = 0;

	// Timer init
	TIMER0_initialize();
	TR0 = 1;

	// Peripherals init
	ADC_initialize();

	IIC_init_slave();

	// PWM_initialize();

	// init_UART();

	// INT0 init
	IT0 = 1;	   // Edge triggered
	ENHIT |= 0x02; // Both rising and falling edge enable for INT0
	EX0 = 1;

	EA = 1;

	ADC_Start(2); // poll the buttons first

	// PWM_Enable();
}

/**
 * @brief Main control loop tick function - updates sensors and controls outputs
 * @note Called every TICK_INTERVAL_MS (30ms) from main loop
 * @note Reads temperatures, updates I2C registers, and applies control commands
 */
void board_tick(void)
{
	sensors_update();

	unsigned int coffee_temp = 0; // degree C * 10
	unsigned int steam_temp = 0;  // degree C * 10

	coffee_temp = map_coffee_boiler_temperature(system_state.coffee.ntc_value);
	steam_temp = map_steam_boiler_temperature(system_state.steam.ntc_value);

	// TODO consider implementing IIC mutex

	n_DAT[0] = system_state.multi_switch | (system_state.steam_switch << 4);
	n_DAT[1] = coffee_temp & 0xFF;
	n_DAT[2] = coffee_temp >> 8;
	n_DAT[3] = steam_temp & 0xFF;
	n_DAT[4] = steam_temp >> 8;

	n_DAT[6] = pump_psm.psm_counter & 0xFF;

	set_controls(n_DAT[8]);					  // n_DAT[8]
	set_valves(n_DAT[9]);					  // n_DAT[9]
	set_pump_power(n_DAT[10]);				  // n_DAT[10]
	set_coffee_power(n_DAT[11], coffee_temp); // n_DAT[11]
	set_steam_power(n_DAT[12], steam_temp);	  // n_DAT[12]
}

/**
 * @brief Set control LEDs and status indicators
 * @param control_value Bit-packed control byte from I2C register n_DAT[8]
 *                      [0]: STATUS_LED, [1]: PWR_LED, [2]: S1_LED, [3]: S2_LED
 *                      [7]: PSM counter reset flag
 */
void set_controls(unsigned char control_value) // n_DAT[8]
{
	STATUS_LED = (control_value >> 0) & 0x01;
	PWR_LED = (control_value >> 1) & 0x01;
	S1_LED = (control_value >> 2) & 0x01;
	S2_LED = (control_value >> 3) & 0x01;

	if ((control_value >> 7) & 0x01)
	{
		pump_psm.psm_counter = 0;
	}
}

/**
 * @brief Set valve states
 * @param control_value Bit-packed control byte from I2C register n_DAT[9]
 *                      [0]: K3_COFFEE_VALVE, [1]: K2_STEAM_VALVE, [2]: K1_3WV
 */
void set_valves(unsigned char control_value) // n_DAT[9]
{
	K3_COFFEE_VALVE = (control_value >> 0) & 0x01;
	K2_STEAM_VALVE = (control_value >> 1) & 0x01;
	K1_3WV = (control_value >> 2) & 0x01;
}

/**
 * @brief Set pump power using PSM (Pulse Skip Modulation)
 * @param control_value Desired power level from I2C register n_DAT[10] (0 to psm_range)
 * @note Value is clamped to psm_range (0x7F) if it exceeds maximum
 */
void set_pump_power(unsigned char control_value) // n_DAT[10]
{
	if (control_value > pump_psm.psm_range)
	{
		control_value = pump_psm.psm_range;
	}

	pump_psm.psm_value = control_value;
}

/**
 * @brief Set coffee boiler heater power with temperature safety limits
 * @param control_value Desired power level from I2C register n_DAT[11] (0-99)
 * @param current_temp Current temperature in decidegrees C (e.g., 950 = 95.0°C)
 * @note Automatically disables heater if temperature exceeds 120°C or sensor fault detected
 */
void set_coffee_power(unsigned char control_value, unsigned int current_temp) // n_DAT[11]
{
	// Safety check: disable heater if temperature exceeds safe limit
	if (current_temp > COFFEE_TEMP_MAX)
	{
		control_value = 0;
	}

	// Safety check: disable heater if sensor fault detected
	if (current_temp == TEMP_ERROR_VALUE)
	{
		control_value = 0;
	}

	coffee_boiler_psm.psm_value = control_value; // 0~127 range
}

/**
 * @brief Set steam boiler heater power with temperature safety limits
 * @param control_value Desired power level from I2C register n_DAT[12] (0-99)
 * @param current_temp Current temperature in decidegrees C (e.g., 1500 = 150.0°C)
 * @note Automatically disables heater if temperature exceeds 180°C or sensor fault detected
 */
void set_steam_power(unsigned char control_value, unsigned int current_temp) // n_DAT[12]
{
	// Safety check: disable heater if temperature exceeds safe limit
	if (current_temp > STEAM_TEMP_MAX)
	{
		control_value = 0;
	}

	// Safety check: disable heater if sensor fault detected
	if (current_temp == TEMP_ERROR_VALUE)
	{
		control_value = 0;
	}

	// PWM_Output3(control_value); // 0~99 range
}

/// PSM (Pulse Skip Modulation) algorithm
// Implements Bresenham-like algorithm to determine if pump should run this cycle
// Accumulates psm_value each cycle; when it exceeds psm_range, pump activates

/**
 * @brief Calculate whether to skip the current pump cycle
 * @note Called on each AC zero-crossing to implement PSM control
 */
bit calculateSkip(psm_state *psm)
{
	bit psm_skip = 0;

	psm->psm_a += psm->psm_value;

	if (psm->psm_a >= psm->psm_range)
	{
		psm->psm_a -= psm->psm_range;
		psm_skip = 0;
	}
	else
	{
		psm_skip = 1;
	}

	if (psm->psm_a > psm->psm_range)
	{
		psm->psm_a = 0;
		psm_skip = 0;
	}

	if (!psm_skip)
	{
		psm->psm_counter++;
	}

	return psm_skip;
}

/**
 * @brief Check and handle AC zero-crossing events
 * @note Called from main loop to process zero_crossed flag set by INT0 ISR
 * @note Triggers ADC polling and PSM calculation on each zero-crossing
 */
void check_zc(void)
{
	if (zero_crossed)
	{
		ADC_poll();

		PUMP = calculateSkip(&pump_psm);
		H_COFFEE = calculateSkip(&coffee_boiler_psm);

		zero_crossed = 0;
	}
}

/**
 * @brief INT0 Interrupt Service Routine - AC Zero-Crossing Detection
 * @note Sets zero_crossed flag to signal main loop that zero-crossing occurred
 * @note Triggered by P1.3 (Z-C input) on falling edge
 */
void INT0_ISR(void) interrupt d_INT0_Vector
{
	zero_crossed = 1;
}
