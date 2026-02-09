#ifndef __BOARD_H__
#define __BOARD_H__

#define goToISP() __asm__("ljmp 0x3c80")

// I2C Register Map (n_DAT array indices)
// READ Registers (Master reads from slave):
#define REG_SWITCHES         0   // [7:4] steam_switch, [3:0] multi_switch state
#define REG_COFFEE_TEMP_L    1   // Coffee temperature low byte (decidegrees C)
#define REG_COFFEE_TEMP_H    2   // Coffee temperature high byte
#define REG_STEAM_TEMP_L     3   // Steam temperature low byte (decidegrees C)
#define REG_STEAM_TEMP_H     4   // Steam temperature high byte
// Register 5: Reserved
#define REG_PSM_COUNTER      6   // Pump cycle counter low byte
// Register 7: Reserved
// WRITE Registers (Master writes to slave):
#define REG_CONTROLS         8   // [7]=PSM_RST [3]=S2_LED [2]=S1_LED [1]=PWR_LED [0]=STATUS_LED
#define REG_VALVES           9   // [2]=K1_3WV [1]=K2_STEAM [0]=K3_COFFEE
#define REG_PUMP_POWER       10  // Pump power (0-127)
#define REG_COFFEE_POWER     11  // Manual coffee heater power (0-99) when setpoint is 0
#define REG_STEAM_POWER      12  // Steam heater power (0-99)
#define REG_COFFEE_SETPOINT_L 13 // Coffee boiler setpoint low byte (decidegrees C)
#define REG_COFFEE_SETPOINT_H 14 // Coffee boiler setpoint high byte
#define REG_PID_KP           15  // Coffee PID proportional gain (scaled)
#define REG_PID_KI           16  // Coffee PID integral gain (scaled)
#define REG_PID_KD           17  // Coffee PID derivative gain (scaled)

// Enum for multi-switch detection (based on resistor values for analog read)
typedef enum
{
    SWITCH_NONE = 0,
    SWITCH_PWR = 1 << 1, // 1K resistor
    SWITCH_S1 = 1 << 2,  // 10K resistor
    SWITCH_S2 = 1 << 3,  // 30K resistor
} MultiSwitchState;

// Struct representing the coffee/steam boiler components
typedef struct
{
    unsigned int ntc_value;  // Raw ADC value from NTC (temperature sensor)
    unsigned int heater : 1; // Heater control (on/off)
    unsigned int valve : 1;  // Valve control (on/off)
} Boiler;

// Main system struct modeling the CRM3007L hardware
typedef struct
{
    Boiler coffee;                    // Coffee boiler: NTC P0.4 (I), Heater P1.7 (O, PWM3), Valve P3.0 (O)
    Boiler steam;                     // Steam boiler: NTC P0.5 (I), Heater P1.6 (O, PWM2), Valve P3.1 (O)
    unsigned int pump : 1;            // Pump control: P1.1 (O)
    unsigned int three_way_valve : 1; // 3-way valve: P3.2 (O)
    unsigned int buzzer : 1;          // Buzzer: P3.3 (O)
    unsigned int steam_switch : 1;    // Steam switch: P0.0 (I)
    MultiSwitchState multi_switch;    // Multi-switch (PWR/S1/S2) via analog: P0.6 (I)
    unsigned int status_led : 1;      // General LED: P0.1 (O)
    unsigned int pwr_led : 1;         // Power LED: P2.3 (O)
    unsigned int s1_led : 1;          // S1 LED: P0.2 (O)
    unsigned int s2_led : 1;          // S2 LED: P0.3 (O)
    unsigned int led_common : 1;      // LED common: P0.7 (O)
} SystemState;

void board_initialize(void);

void board_tick(void);

void check_zc(void);

extern __xdata SystemState system_state;

#endif
