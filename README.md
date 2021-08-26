# EveApps
EVE Apps

This repository contains the sampleApp and demo applications for EVE. 

## Build instructions

### Raspberry Pi Pico

Requires the Pico toolchain https://github.com/ndabas/pico-setup-windows to be installed.

The following steps will build for Raspberry Pi Pico.
Replace `X:\source\eve_apps` with your EveApps repository path.
Replace `VM810C` with `VM800C` or `VM816C` if needed.
Replace `Debug` with `Release` if needed.

* Launch the *Developer Command Prompt for Pico*
* `cd X:\source\eve_apps`
* `X:`
* `mkdir build_pico_debug`
* `cd build_pico_debug`
* `cmake -G "NMake Makefiles" -DEVE_APPS_PLATFORM=RP2040 -DEVE_APPS_GRAPHICS=VM810C -DCMAKE_BUILD_TYPE=Debug ..`
* `nmake`

Display resolution is set by flag EVE_DISPLAY, example: -DEVE_DISPLAY=EVE_DISPLAY_WXGA
By default, EVE_DISPLAY_WVGA is set

#### Connections

| RP2040 | EVE | UART | SD |
| --- | --- | --- | --- |
| GP0 (TX) | | RX (debug) | |
| GP1 (RX) | | TX (optional) | |
| GP2 (SPI0 SCK) | SCK | | |
| GP3 (SPI0 MOSI) | MOSI | | |
| GP4 (SPI0 MISO) | MISO | | |
| GP5 (GPIO) | CS | | |
| GP6 (GPIO) | INT | | |
| GP7 (GPIO) | PWD | | |
| 5V | 5V | | |
| GND | GND | | |

### FT4222 and MPSSE

* Run *CMake GUI* on the repository
* Create a new build subdirectory as location to build the binaries
* Push *Configure*
* Select the appropriate `EVE_APPS_PLATFORM` and `EVE_APPS_GRAPHICS` options.
* Push *Generate*
* Push *Open Project*
* Build and run

## Backwards compatibility

Header files under the following paths provide wrapping for compatibility with previous library versions, to allow applications to be upgraded quickly.
These header files should not be included with new projects

- common/esd_core/Compatibility/*.h
- common/eve_hal/Hdr/*.h (except external library headers)

## Style guide

Brief overview of symbol naming styles used.

### EVE Hal

Functions and local variables inside functions are in camelCase, for example `copyToBuffer`.

Public variables inside structures, and other global variables, are in PascalCase, for example `WriteIndex`.

Global functions, and functions used across C files within the libray, are **prefixed** with the namespace in all-caps, and the filename, class or other identifying module in PascalCase, for example `EVE_CoCmd_`. A global function will end up as `EVE_CoCmd_copyToBuffer`, a global variable will end up as `EVE_CoCmd_WriteIndex`, this helps in disambiguating variable and function names. Global functions and variables that are globally applicable may simply be prefixed with `EVE_`, for example `EVE_initialize`.

Static functions (used within a single file) don't need a prefix, just the bare name. Static variables should have an `s_` prefix for clarity, but should be avoided where possible.

Structure names are in PascalCase and preceeded by the `EVE_` prefix, for example `EVE_ConfigParameters`.

### BT8XXEMU C interface

Follows the same style as EVE Hal.

### App code

Application code generally omits any namespace prefix, since it's not necessary. Varies between apps.

### Legacy Hal

Older code, preceeded by either a `FT_` or `Ft_` prefix, uses a mixture of PascalCase and Upper_Snake_Case.

### ESD

ESD follows the same styling as HAL, except the library prefix (`Esd_`) and function names are both in PascalCase.
