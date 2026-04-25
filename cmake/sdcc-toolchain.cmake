# SDCC Toolchain file for OB38R16T1 MCU
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR OB38R16T1)

# SDCC compiler settings
set(CMAKE_C_COMPILER sdcc)
set(CMAKE_AR sdcclib)
set(CMAKE_RANLIB :)

# Target processor
set(MCU_ARCH mcs51)

# Compiler flags for OB38R16T1
# 16KB Flash, 256B internal RAM, 512B SRAM
set(CMAKE_C_FLAGS_INIT "-mmcs51 --model-small --stack-auto --xram-size 512 --code-size 16384 --iram-size 256")
set(CMAKE_EXE_LINKER_FLAGS_INIT "-mmcs51 --out-fmt-ihx")

# File extensions
set(CMAKE_EXECUTABLE_SUFFIX ".ihx")
set(CMAKE_C_OUTPUT_EXTENSION ".rel")

# Skip compiler tests
set(CMAKE_C_COMPILER_WORKS 1)
set(CMAKE_CXX_COMPILER_WORKS 1)
