cmake_minimum_required(VERSION 3.20)

# Set toolchain before project
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/cmake/sdcc-toolchain.cmake)

project(CRM3007L C)

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/HAL
)

# Source files
set(SOURCES
    src/main.c
    src/board.c
    src/pid.c
    src/sensors.c
    src/HAL/ADC.c
    src/HAL/IIC.c
    src/HAL/PWM.c
    src/HAL/timer.c
    src/HAL/UART.c
    src/HAL/WDT.c
)

# Create executable
add_executable(firmware ${SOURCES})

# Additional SDCC-specific flags
target_compile_options(firmware PRIVATE
    -mmcs51
    --model-small
    --stack-auto
)

# Compiler definitions
target_compile_definitions(firmware PRIVATE
    MCS51REG_ENABLE_WARNINGS
    MICROCONTROLLER_DEFINED
)

target_link_options(firmware PRIVATE
    -mmcs51
    --out-fmt-ihx
)

# Print build information
message(STATUS "Building for OB38R16T1 MCU using SDCC")
message(STATUS "Target: 16KB Flash, 256B RAM, 512B SRAM")
message(STATUS "Output: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/firmware.ihx")
