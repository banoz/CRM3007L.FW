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
#define H_STEAM P1_7
#define H_COFFEE P1_6

#define K1_3WV P3_2
#define K2_STEAM_VALVE P3_1
#define K3_COFFEE_VALVE P3_0

// Temperature safety limits (in decidegrees C: value * 10)
#define COFFEE_TEMP_MAX (1200) // 120°C - maximum safe temperature for coffee boiler
#define STEAM_TEMP_MAX (1800)  // 180°C - maximum safe temperature for steam boiler

// Port configuration constants (for board_initialize)
#define P0_MODE1_CONFIG 0x71  // P0M1: 0b01110001 - P0.0,P0.4,P0.5,P0.6 as input
#define P0_MODE0_CONFIG 0x8E  // P0M0: 0b10001110 - P0.1,P0.2,P0.3,P0.7 as output
#define P1_MODE1_CONFIG 0xC2  // P1M1: 0b11000010 - P1.1,P1.6,P1.7 push-pull
#define P1_MODE0_CONFIG 0xC2  // P1M0: 0b11000010
#define P2_MODE1_CONFIG 0x00  // P2M1: 0x00
#define P2_MODE0_CONFIG 0x08  // P2M0: 0b00001000 - P2.3 as output
#define P3_MODE1_CONFIG 0x00  // P3M1: 0x00
#define P3_MODE0_CONFIG 0x0F  // P3M0: 0b00001111 - P3.0-P3.3 as output

// PSM (Pulse Skip Modulation) constants
#define PSM_RANGE_MAX 0x7F    // PSM range: 0-127 (7-bit control for pump power)

SystemState system_state;

/// Pulse Skip Modulation (PSM) for pump control
// PSM is used to control pump speed by skipping AC half-cycles
// psm_value: desired pump power (0 to psm_range)
// psm_range: maximum range (0x7F = 127)
// psm_counter: counts active pump cycles for monitoring

unsigned char psm_range = PSM_RANGE_MAX; // PSM range: 0-127 (7-bit control)
unsigned char psm_value = 0;             // Current PSM power setting
unsigned int psm_counter = 0;            // Count of pump activation cycles

unsigned int psm_a = 0;		   // PSM accumulator
volatile bit zero_crossed = 0; // Flag set by INT0 on zero-crossing detection

char str_buf[16];

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
	// Configure port modes using named constants
	P0M1 = P0_MODE1_CONFIG;
	P0M0 = P0_MODE0_CONFIG;
	P1M1 = P1_MODE1_CONFIG;
	P1M0 = P1_MODE0_CONFIG;
	P2M1 = P2_MODE1_CONFIG;
	P2M0 = P2_MODE0_CONFIG;
	P3M1 = P3_MODE1_CONFIG;
	P3M0 = P3_MODE0_CONFIG;

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

	PWM_initialize();

	// INT0 init
	IT0 = 1;
	EX0 = 1;

	EA = 1;

	ADC_Start(2); // poll the buttons first

	PWM_Enable();
}

/**
 * @brief Main control loop tick function - updates sensors and controls outputs
 * @note Called every TICK_INTERVAL_MS (30ms) from main loop
 * @note Reads temperatures, updates I2C registers, and applies control commands
 */
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
		psm_counter = 0;
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
	if (control_value > psm_range)
	{
		control_value = psm_range;
	}

	psm_value = control_value;
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

	PWM_Output2(control_value); // 0~99 range
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

	PWM_Output3(control_value); // 0~99 range
}

/// PSM (Pulse Skip Modulation) algorithm
// Implements Bresenham-like algorithm to determine if pump should run this cycle
// Accumulates psm_value each cycle; when it exceeds psm_range, pump activates

/**
 * @brief Calculate whether to skip the current pump cycle
 * @note Called on each AC zero-crossing to implement PSM control
 */
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

/**
 * @brief Check and handle AC zero-crossing events
 * @note Called from main loop to process zero_crossed flag set by INT0 ISR
 * @note Triggers ADC polling and PSM calculation on each zero-crossing
 * @note Uses atomic operation to prevent race condition with ISR
 */
void check_zc(void)
{
	// Atomic check and clear of zero_crossed flag to prevent race condition
	EA = 0; // Disable interrupts
	if (zero_crossed)
	{
		zero_crossed = 0;
		EA = 1; // Re-enable interrupts
		
		ADC_poll();
		calculateSkip();
	}
	else
	{
		EA = 1; // Re-enable interrupts
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
	IE0 = 0;
}

/**
 * @brief Emergency shutdown - disable all heaters, pump, and valves
 * @note Call this function on critical errors (sensor faults, overtemperature, etc.)
 * @note Disables all outputs to safe state
 */
void board_emergency_shutdown(void)
{
	// Disable all heaters
	PWM_Output2(0);  // Coffee heater off
	PWM_Output3(0);  // Steam heater off
	
	// Disable pump
	PUMP = 1;  // Pump off (assuming active low)
	
	// Close all valves
	K3_COFFEE_VALVE = 0;
	K2_STEAM_VALVE = 0;
	K1_3WV = 0;
	
	// Turn off buzzer
	BUZZER = 0;
	
	// Optional: Set status LED to indicate error state
	STATUS_LED = 1;  // Indicate error
}
