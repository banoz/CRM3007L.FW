# SDCC OB38R16T1 MCU Project

This project uses SDCC (Small Device C Compiler) to develop firmware for the OB38R16T1 microcontroller.

## Target Specifications
- **MCU**: OB38R16T1 (8051-compatible)
- **Flash**: 16KB (16384 bytes)
- **Internal RAM**: 256 bytes
- **SRAM**: 512 bytes

## Project Structure
- `src/` - Source files
- `build/` - Build output directory
- `CMakeLists.txt` - CMake configuration
- `cmake/` - CMake toolchain files

## Coding Guidelines
- Use SDCC-specific keywords and extensions for OB38R16T1 MCU
- Follow embedded C best practices
- Keep interrupt handlers short and efficient
- Use `__at` keyword for absolute addressing when needed
- Use `__interrupt` keyword for interrupt service routines
- Be mindful of memory constraints: 16KB Flash, 512B SRAM
- Optimize code for size when working near Flash limits

## Build Instructions
1. Configure: `cmake -B build`
2. Build: `cmake --build build`
3. Output: `.ihx` file in `build/` directory
