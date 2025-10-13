#ifndef __BOARD_H__
#define __BOARD_H__

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
    Boiler coffee;                    // Coffee boiler: NTC P0.4 (I), Heater P1.7 (O), Valve P3.0 (O)
    Boiler steam;                     // Steam boiler: NTC P0.5 (I), Heater P1.6 (O), Valve P3.1 (O)
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

#endif