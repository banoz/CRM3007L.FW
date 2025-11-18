# Code Review: CRM3007L Firmware Documentation

**Date**: November 18, 2025  
**Reviewer**: GitHub Copilot  
**Repository**: banoz/CRM3007L.FW  
**Commit Reviewed**: 8e2ba21 - "Update temperature calculation for steam coil"

## Executive Summary

This repository contains documentation for a CRM3007L coffee machine controller firmware project using the OB38R16T1W28SP microcontroller. The current state includes hardware pin mappings, NTC thermistor specifications, and temperature calculation formulas. No source code implementation is present yet.

## Review Findings

### 1. Temperature Calculation Formulas ⚠️

**Issue**: The NTC temperature formulas in README.md use ambiguous notation:

```
float t = (-63.82) * ln(r) + 567.4   // for coffee boiler
float t = (-44.08) * ln(r) + 416.42  // for steam coil
```

**Problems**:
- `ln(r)` is mathematical notation, but in most programming languages (C/C++), the natural logarithm function is `log()` or `logf()` (not `ln()`)
- Variable `r` is not defined - it should specify what it represents (e.g., resistance in kΩ or Ω)
- Temperature units are not specified (Celsius assumed, but should be explicit)
- The formulas appear to be empirical fits but lack context about their derivation or accuracy range

**Recommendation**:
```c
// NTC Temperature Calculations
// NTC: 10kΩ @ 25°C
// Circuit: NTC with 1kΩ pullup to 5V
// Temperature range: [operating range] °C
// Accuracy: ±[X]°C

// Coffee Boiler Temperature (°C)
// r = NTC resistance in kΩ
float temp_coffee_celsius = (-63.82f) * logf(r) + 567.4f;

// Steam Coil Temperature (°C)  
// r = NTC resistance in kΩ
float temp_steam_celsius = (-44.08f) * logf(r) + 416.42f;
```

### 2. Hardware Documentation ✅

**Good**:
- Comprehensive pin mapping table with PORT, CONN, DESC, SW, I/O, and AF columns
- Clear identification of ADC, PWM, and UART functions
- PCB connector pinouts documented

**Issues**:
- Some pins have empty description fields (P1.0 labeled as PCB-C.4 but no function described)
- PCB-C.2, PCB-C.6, PCB-C.7 lack functional descriptions
- "Z-C" on P1.3 is unclear (Zero-Cross detection? Should be spelled out)

**Recommendations**:
- Complete all pin descriptions
- Add notes about unused/reserved pins
- Clarify abbreviations (Z-C, 3WV, BUZ)
- Add electrical specifications (voltage levels, current limits)

### 3. NTC Thermistor Specifications ⚠️

**Current**:
```
NTC 10kΩ @25°C
```

**Issues**:
- Missing B-value (Beta coefficient), which is critical for NTC calculations
- No manufacturer/part number specified
- Temperature coefficient not documented
- Accuracy/tolerance not specified

**Recommendations**:
```markdown
# NTC Thermistor Specifications

- **Type**: NTC (Negative Temperature Coefficient)
- **Resistance**: 10kΩ ±1% @ 25°C
- **B-Value**: [specify, e.g., 3950K ±1%]
- **Part Number**: [specify manufacturer and part number]
- **Temperature Range**: [specify operating range]
- **Tolerance**: ±1% or ±5%
- **Circuit**: 1kΩ pullup resistor to 5V
```

### 4. Missing Implementation 🔴

**Critical**: No source code is present in the repository.

**Required**:
- Firmware source code (C/C++ for microcontroller)
- Build system configuration (Makefile, platformio.ini, etc.)
- Pin initialization code
- ADC reading and temperature conversion implementation
- PWM control for heaters
- Safety features (overheat protection, etc.)
- State machine for coffee/steam modes

### 5. Missing Documentation 🔴

**Required**:
- Build instructions
- Flashing/programming instructions
- Calibration procedures
- Testing procedures
- Safety considerations
- Electrical schematic or circuit diagram
- PCB layout reference
- Bill of Materials (BOM)

### 6. LICENSE File ✅

**Status**: Correct and appropriate

The repository uses the Unlicense (public domain dedication), which is appropriate for open hardware/firmware projects. The license text is complete and correct.

## Security Considerations

⚠️ **Safety-Critical Application**: This firmware controls heating elements and water in a coffee machine. The following safety features MUST be implemented:

1. **Overheat Protection**
   - Temperature limits with safety margins
   - Failsafe defaults (heaters OFF on error)
   - Watchdog timer to detect firmware hangs

2. **Sensor Validation**
   - NTC open/short circuit detection
   - Range checking on ADC values
   - Fallback behavior on sensor failure

3. **Electrical Safety**
   - Zero-cross detection for AC control (P1.3 Z-C)
   - Proper PWM limits to prevent damage
   - Emergency shutdown capability

4. **Input Validation**
   - Bounds checking on all calculations
   - Overflow/underflow protection in temperature formulas
   - Sanity checks on sensor readings

## Code Quality Standards (for future implementation)

When implementing the firmware, follow these standards:

1. **Coding Style**
   - Use consistent naming conventions
   - Comment all magic numbers
   - Document all functions
   - Use meaningful variable names

2. **Error Handling**
   - Check all return values
   - Implement proper error codes
   - Log errors appropriately
   - Fail safely on all errors

3. **Testing**
   - Unit tests for temperature calculations
   - Integration tests for control logic
   - Hardware-in-the-loop tests
   - Safety feature validation

4. **Version Control**
   - Semantic versioning
   - Changelog maintenance
   - Tag releases appropriately

## Recommendations Summary

### High Priority 🔴
1. Add firmware source code implementation
2. Clarify temperature formula notation and units
3. Add NTC B-value and complete specifications
4. Implement safety features (overheat protection, sensor validation)
5. Add build and programming documentation

### Medium Priority ⚠️
1. Complete pin descriptions in hardware table
2. Add electrical specifications and limits
3. Create schematic and PCB documentation
4. Add calibration procedures
5. Document testing procedures

### Low Priority ℹ️
1. Expand abbreviations (Z-C, 3WV, BUZ)
2. Add photos/diagrams of hardware
3. Create troubleshooting guide
4. Add example usage/operation manual

## Conclusion

The documentation provides a good foundation for understanding the hardware configuration, but the project is incomplete without firmware implementation. The temperature formulas need clarification, and several critical documentation items are missing.

**Overall Status**: 📋 Documentation Only - Implementation Required

**Recommendation**: 
1. Clarify the existing documentation issues identified above
2. Begin firmware implementation with safety features as top priority
3. Add comprehensive testing and calibration documentation
4. Create proper hardware documentation (schematics, BOM)

---

*This review is based on commit 8e2ba21. Future commits should address these findings.*
