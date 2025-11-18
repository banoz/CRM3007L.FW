# Code Review Report - CRM3007L.FW

**Date:** 2025-11-18  
**Reviewer:** GitHub Copilot Code Review Agent  
**Project:** CRM3007L Coffee Machine Controller Firmware  
**MCU:** OB38R16T1W28SP

---

## Executive Summary

This firmware controls a coffee machine with two boilers (coffee and steam), multiple valves, pump control with PSM (Pulse Skip Modulation), and temperature sensing via NTC thermistors. The code is functional but has several areas requiring attention for improved safety, maintainability, and robustness.

**Overall Assessment:** The code is operational but needs improvements in safety mechanisms, error handling, and code clarity.

---

## Critical Issues (High Priority)

### 1. **Unsafe Temperature Fail-Safe Logic**

**Location:** `board.c`, lines 148-165

**Issue:**
```c
void set_coffee_power(unsigned char control_value, unsigned int current_temp)
{
    if (current_temp > 1200) // cap at 120C
    {
        control_value = 0;
    }
    PWM_Output(0, 0, control_value, 0); // 0~99 range
}
```

**Problems:**
- No validation of temperature sensor readings before use
- If NTC sensor fails (open/short circuit), `current_temp` could be invalid
- No over-temperature emergency shutdown mechanism
- Temperature limits are hardcoded without hysteresis
- Missing redundant safety checks

**Recommendation:**
- Add sensor fault detection (check for out-of-range ADC values)
- Implement hysteresis for temperature control
- Add emergency shutdown for sensor failures
- Log temperature anomalies
- Add multiple safety thresholds (warning, critical, emergency)

---

### 2. **Missing ADC Value Validation**

**Location:** `sensors.c`, lines 43-51

**Issue:**
```c
unsigned int map_coffee_boiler_temperature(unsigned int adc_value)
{
    return (unsigned int)(COFFEE_NTC_A + (COFFEE_NTC_B * log((float)(unsigned int)adc_value)));
}
```

**Problems:**
- `log(0)` would cause undefined behavior or return -infinity
- No validation that `adc_value` is within valid range (0-4095 for 12-bit ADC)
- Integer underflow possible if result is negative
- No sensor fault detection

**Recommendation:**
```c
unsigned int map_coffee_boiler_temperature(unsigned int adc_value)
{
    // Sensor fault detection
    if (adc_value < 10 || adc_value > 4085) {
        return 0xFFFF; // Error value
    }
    
    float temp = COFFEE_NTC_A + (COFFEE_NTC_B * log((float)adc_value));
    
    // Clamp to valid temperature range
    if (temp < 0) return 0;
    if (temp > 2500) return 2500; // 250°C max
    
    return (unsigned int)temp;
}
```

---

### 3. **Watchdog Timer Disabled**

**Location:** `main.c`, line 20

**Issue:**
```c
// WDTR_Init(); // TODO this breaks ADC, fix later
```

**Problems:**
- Watchdog disabled with a TODO comment suggests unresolved issue
- System can hang without recovery mechanism
- No failsafe if main loop stalls
- Critical safety feature disabled

**Recommendation:**
- Debug and fix the ADC/WDT conflict immediately
- This is a safety-critical system that should have watchdog protection
- Consider alternative timing/initialization sequence

---

### 4. **Race Condition in Zero-Crossing Detection**

**Location:** `board.c`, lines 169-212

**Issue:**
```c
volatile bit zero_crossed = 0;

void check_zc(void)
{
    if (zero_crossed)
    {
        ADC_poll();
        calculateSkip();
        zero_crossed = 0;  // Not atomic with interrupt
    }
}

void INT0Interrupt(void) interrupt d_INT0_Vector
{
    zero_crossed = 1;
    IE0 = 0;
}
```

**Problems:**
- `zero_crossed` access is not atomic
- Potential race condition between ISR and main loop
- Could miss zero-crossing events
- No interrupt re-enabling logic visible

**Recommendation:**
- Use atomic operations or disable interrupts during flag check/clear
- Consider using a counter instead of boolean flag to catch missed events
- Add logic to re-enable INT0 interrupt

---

## High Priority Issues

### 5. **Magic Numbers Throughout Code**

**Locations:** Multiple files

**Examples:**
- `board.c`: `0x71, 0x8E, 0xC2` for port configurations
- `sensors.c`: `3300, 1700, 700` for switch thresholds
- `main.c`: `30` for tick interval
- `HAL/timer.c`: `1310` for reload value

**Problems:**
- Reduces code readability
- Makes maintenance difficult
- No explanation of where values come from
- Hard to adjust configuration

**Recommendation:**
```c
// Example improvements:
#define TICK_INTERVAL_MS        30
#define SWITCH_PWR_THRESHOLD    3300  // ADC value for 1K resistor
#define SWITCH_S1_THRESHOLD     1700  // ADC value for 10K resistor
#define SWITCH_S2_THRESHOLD     700   // ADC value for 30K resistor

// Port configuration
#define P0_MODE1_CONFIG         0x71  // P0.0,P0.4,P0.5,P0.6 as input
#define P0_MODE0_CONFIG         0x8E  // P0.1,P0.2,P0.3,P0.7 as output
```

---

### 6. **No Error Handling for I2C Communication**

**Location:** `HAL/IIC.c`

**Issue:**
- I2C slave implementation has no error handling
- No timeout mechanisms
- No bus error detection
- No retry logic

**Problems:**
- Bus lockup could hang the system
- Communication errors go undetected
- No recovery from invalid states

**Recommendation:**
- Add timeout detection
- Implement error counters
- Add bus reset capability
- Validate data integrity

---

### 7. **Buffer Overflow Risk in UART**

**Location:** `HAL/UART.c`, lines 55-60

**Issue:**
```c
if (next_head != rx_tail) {  // Buffer not full
    rx_buf[rx_head] = RXBUF0;
    rx_head = next_head;
}
```

**Problems:**
- Silently drops data when buffer full
- No notification of buffer overflow
- Could lose critical commands

**Recommendation:**
- Add overflow flag/counter
- Log or signal buffer overflow events
- Consider increasing buffer size
- Implement flow control if possible

---

## Medium Priority Issues

### 8. **Inconsistent Naming Conventions**

**Examples:**
- `n_DAT`, `n_data`, `n_Next_Step` (Hungarian notation inconsistently applied)
- `getTick()` vs `check_zc()` (camelCase vs snake_case)
- `PWM_initialize()` vs `board_tick()` (mixed conventions)

**Recommendation:**
- Adopt consistent naming convention (suggest snake_case for C)
- Update style guide documentation
- Apply consistently across all files

---

### 9. **Missing Function Documentation**

**Issue:** Most functions lack documentation explaining:
- Purpose
- Parameters
- Return values
- Side effects
- Preconditions

**Recommendation:**
```c
/**
 * @brief Set coffee boiler heater power with temperature safety limit
 * @param control_value Desired power level (0-99)
 * @param current_temp Current temperature in decidegrees C (e.g., 950 = 95.0°C)
 * @note Automatically disables heater if temperature exceeds 120°C
 */
void set_coffee_power(unsigned char control_value, unsigned int current_temp)
```

---

### 10. **Unused/Commented Code**

**Locations:**
- `HAL/ADC.c`: Lines 13-99 (large block of commented code)
- Variables in PWM.c that are commented out

**Problems:**
- Clutters codebase
- Confusing for maintenance
- Unclear if code might be needed

**Recommendation:**
- Remove commented code (version control preserves history)
- If keeping for reference, add clear explanation why

---

### 11. **PSM Algorithm Lacks Documentation**

**Location:** `board.c`, lines 168-200

**Issue:**
```c
void calculateSkip(void)
{
    bit psm_skip = 0;
    psm_a += psm_value;
    if (psm_a >= psm_range) {
        psm_a -= psm_range;
        psm_skip = 0;
    } else {
        psm_skip = 1;
    }
    // ... more logic
}
```

**Problems:**
- PSM (Pulse Skip Modulation) algorithm not explained
- No comments on why specific logic is used
- Variable names not descriptive (`psm_a`, `psm_range`)
- Magic number `0x7F` for psm_range

**Recommendation:**
- Add detailed comments explaining PSM algorithm
- Rename variables for clarity: `psm_accumulator`, `psm_divisor`
- Document the modulation strategy

---

### 12. **Temperature Formula Constants Need Documentation**

**Location:** `sensors.h`, lines 4-7

**Issue:**
```c
#define COFFEE_NTC_A (5674)
#define COFFEE_NTC_B (-638)
#define STEAM_NTC_A (4164)
#define STEAM_NTC_B (-441)
```

**Problems:**
- No explanation of where these values come from
- README.md has different formulas with floating-point constants
- Units not specified (are these in decidegrees?)
- No reference to NTC datasheet or calibration procedure

**Recommendation:**
```c
// NTC 10kΩ @ 25°C temperature mapping constants
// Formula: temp(°C × 10) = A + (B × ln(adc_value))
// Calibrated on YYYY-MM-DD using [method]
// Based on: [datasheet/calibration reference]
#define COFFEE_NTC_A (5674)  // Offset constant
#define COFFEE_NTC_B (-638)  // Scaling factor (× 10 for decidegrees)
```

---

## Low Priority Issues

### 13. **Delay Function is Inefficient**

**Location:** `main.c`, lines 7-11

**Issue:**
```c
void delay(unsigned long cnt)
{
    while (cnt--)
        ;
}
```

**Problems:**
- Compiler may optimize away empty loop
- Not calibrated to any actual time
- Wastes CPU cycles
- Should use timer-based delay

**Recommendation:**
- Use `volatile` or timer-based delays
- Consider sleep/idle mode for power efficiency
- Document actual delay achieved

---

### 14. **No Version Information**

**Issue:** No firmware version tracking in code

**Recommendation:**
```c
#define FW_VERSION_MAJOR 1
#define FW_VERSION_MINOR 0
#define FW_VERSION_PATCH 0
#define FW_BUILD_DATE __DATE__
#define FW_BUILD_TIME __TIME__
```

---

### 15. **Limited I2C Memory Map Documentation**

**Location:** `board.c`, lines 100-114

**Issue:**
```c
n_DAT[0] = system_state.multi_switch | (system_state.steam_switch << 4);
n_DAT[1] = coffee_temp & 0xFF;
// ... more assignments
```

**Problems:**
- Memory map not documented
- No explanation of register layout
- Hard to understand I2C interface

**Recommendation:**
Create a register map in header file or documentation:
```c
// I2C Register Map
#define REG_SWITCHES        0   // [7:4] steam switch, [3:0] multi switch
#define REG_COFFEE_TEMP_L   1   // Coffee temp low byte
#define REG_COFFEE_TEMP_H   2   // Coffee temp high byte
// ... etc
```

---

## Code Quality Observations

### Positive Aspects:
1. ✅ Clean separation of concerns (HAL, board, sensors, main)
2. ✅ Use of hardware abstraction layer
3. ✅ Interrupt-driven design for responsive control
4. ✅ Reasonable modularization

### Areas for Improvement:
1. ❌ Lack of defensive programming practices
2. ❌ Missing input validation
3. ❌ No error handling strategy
4. ❌ Insufficient documentation
5. ❌ Inconsistent code style
6. ❌ Magic numbers reduce maintainability

---

## Security Considerations

### Potential Vulnerabilities:
1. **I2C Slave Access**: No authentication or validation of I2C commands
2. **Buffer Handling**: UART buffer could be exploited if data patterns are predictable
3. **Temperature Control**: Malicious I2C commands could bypass temperature limits

### Recommendations:
1. Add command validation for I2C writes
2. Implement safety limits in firmware that cannot be overridden
3. Add checksums or CRC for critical data
4. Consider watchdog as mandatory safety feature

---

## Testing Recommendations

### Current State:
- No visible test infrastructure
- Manual testing likely relied upon

### Recommendations:
1. **Unit Tests**: For temperature conversion functions, PSM algorithm
2. **Integration Tests**: Sensor reading pipeline, control loop timing
3. **Hardware-in-Loop Testing**: With actual hardware for safety validation
4. **Boundary Testing**: Sensor failure modes, extreme temperatures
5. **Regression Testing**: Ensure fixes don't break existing functionality

---

## Compliance & Standards

### Considerations:
- Coffee machines are consumer appliances with safety requirements
- Consider applicable standards:
  - IEC 60335-2-15 (Appliances for heating liquids)
  - UL 1082 (Household Electric Coffee Makers)

### Requirements Often Include:
- Temperature limit enforcement (✅ partial)
- Fail-safe modes (❌ missing)
- Watchdog timer (❌ disabled)
- Redundant safety measures (❌ limited)

---

## Recommended Action Plan

### Immediate (Safety Critical):
1. Fix watchdog timer / ADC conflict
2. Add sensor fault detection to temperature functions
3. Implement emergency shutdown logic
4. Add ADC range validation

### Short Term:
1. Replace magic numbers with named constants
2. Add function documentation
3. Fix race condition in zero-crossing detection
4. Add error handling for I2C communication

### Medium Term:
1. Implement comprehensive error logging
2. Add version tracking
3. Create detailed I2C register documentation
4. Establish coding standards

### Long Term:
1. Develop test infrastructure
2. Consider formal safety certification
3. Implement firmware update mechanism
4. Add diagnostic capabilities

---

## Conclusion

The firmware demonstrates good structural organization but requires immediate attention to safety-critical issues, particularly around sensor validation and failsafe mechanisms. The code would benefit significantly from defensive programming practices, comprehensive documentation, and consistent coding standards.

**Priority:** Address the critical issues before deploying to production, especially watchdog functionality and temperature sensor validation.

