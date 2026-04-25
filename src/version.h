#ifndef __VERSION_H__
#define __VERSION_H__

// Firmware version information
#define FW_VERSION_MAJOR 1
#define FW_VERSION_MINOR 0
#define FW_VERSION_PATCH 0

// Build information (automatically set by compiler)
#define FW_BUILD_DATE __DATE__
#define FW_BUILD_TIME __TIME__

// Version string helper macros
#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

#define FW_VERSION_STRING \
    TOSTRING(FW_VERSION_MAJOR) "." \
    TOSTRING(FW_VERSION_MINOR) "." \
    TOSTRING(FW_VERSION_PATCH)

#endif // __VERSION_H__
