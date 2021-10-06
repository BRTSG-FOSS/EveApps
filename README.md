# EveApps

EveApps is a set of applications based on Eve Series chip (FT80X, FT81X, BT815/6, BT817/8) for users to refer to.

It contains 28 demo application and 1 sample application. They are all written in ANSI C code and full source code is enclosed.
Demo application is for demo purpose and fair complex, which is closed to real-life project. 
Sample application is for the tutorial purpose which is simpler and easier to understand each display list or command.   
For beginners, please always start from SampleApp project first.

All the applications here depend on a set of common files under "common" folder. 
Each application has one subfolder "project" which contains the project file used to build it up from toolchain.
For FT4222/MPSSE or Eve Emulator platform, the project file is for Microsoft Visual Studio 2017 IDE.
For FT90X/FT930 platform, the project is for Eclipse IDE from Bridgetek. 

Users are assumed to be familiar with the programming guide and data sheet of Eve Series chips. 

Please note that some of the applications are developed for certain Eve Series chips or host platforms (see ["Support platforms"](#support-platforms)).

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

## Folder introduction

    ├───common                  
    │   ├───app_common          | Application's common functions
    │   ├───eve_flash           | Blob binary for flash programming
    │   └───eve_hal             | Hardware abstraction layer to bridge different platforms
    ├───EveApps                  
    │    ├───DemoCircleView      | Image viewer demo
    │    ├───DemoEvChargePoint   | Electric charging station demo
    │    ├───DemoFlashBitbang    | Reading flash content in low-level mode
    │    ├───DemoGauges          | Gauges demo
    │    ├───DemoGradient        | Color gradient demo
    │    ├───DemoGraph           | Graph demo
    │    ├───DemoHDPictureViewer | 4K image viewing demo
    │    ├───DemoImageviewer     | Image viewer demo
    │    ├───DemoImageviewer2    | Image viewer demo
    │    ├───DemoInstrument      | Instrument demo
    │    ├───DemoJackpot         | Jackpot game demo
    │    ├───DemoKeyboard        | Keyboard demo
    │    ├───DemoLift            | Lift demo
    │    ├───DemoLift2           | Lift with video background demo
    │    ├───DemoMainmenu        | Main menu demo
    │    ├───DemoMediaPlayer     | Manage and play media content on hard disk/SD card
    │    ├───DemoMetaballs       | Balls demo
    │    ├───DemoMeter           | Meter demo 
    │    ├───DemoRefrigerator    | Refrigerator demo
    │    ├───DemoRotaryDial      | Rotary dial demo
    │    ├───DemoRunningBar      | Street light moving demo 
    │    ├───DemoSignals         | Wave signals demo
    │    ├───DemoSignature       | Signature demo
    │    ├───DemoSketch          | Sketch demo
    │    ├───DemoUnicode         | Unicode demo
    │    ├───DemoUnicodeRuntime  | Unicode demo with runtime determined characters
    │    ├───DemoWashingMachine  | Washing machine UI demo
    │    └───SampleApp           | Sample application for tutorial purpose
    │            
    ├───Tools/EveApps_Configure  | An .NET GUI tool to help user select hardware for an EveApps application

## Support platforms

    ┌────────────────────┬────────────────────────────────────────────────┬─────────────────────┬────────────────────────────────┐
    │ Application        │ MPSSE  FT4222  FT900  FT930  Emulator  MM2040EV│  FT800  BT815  BT817│ QVGA  WQVGA  WVGA   WSVGA  WXGA│ 
    ├────────────────────┼────────────────────────────────────────────────┼─────────────────────┼────────────────────────────────┤
    │ DemoAudioPlayback  │ .      .      .      .      .          .       │  x      .      .    │ .     .      .      x      x   │ 
    │ DemoCircleView     │ .      .      .      .      .          .       │  x      .      .    │ .     .      .      .      x   │ 
    │ DemoEvChargePoint  │ .      .      .      x      .          .       │  x      .      .    │ x     x      x      x      x   │ 
    │ DemoFlashBitbang   │ .      .      .      .      .          .       │  x      .      .    │ .     .      .      .      .   │ 
    │ DemoGauges         │ .      .      .      .      .          .       │  .      .      .    │ .     .      .      x      x   │ 
    │ DemoGradient       │ .      .      .      .      .          .       │  .      .      .    │ .     .      .      x      x   │ 
    │ DemoGraph          │ .      .      .      .      .          .       │  .      .      .    │ .     .      .      x      x   │ 
    │ DemoHDPictureViewer│ .      .      .      .      .          .       │  x      .      .    │ .     .      .      x      x   │ 
    │ DemoImageViewer    │ .      .      .      .      .          .       │  .      .      .    │ .     .      .      x      x   │ 
    │ DemoImageViewer2   │ .      .      .      x      .          .       │  x      .      .    │ .     .      .      x      x   │ 
    │ DemoInstrument     │ .      .      x      x      .          .       │  x      .      .    │ .     .      .      x      x   │ 
    │ DemoJackpot        │ .      .      .      .      .          .       │  .      .      .    │ .     .      .      x      x   │ 
    │ DemoKeyboard       │ .      .      .      .      .          .       │  .      .      .    │ .     .      .      .      .   │ 
    │ DemoLift           │ .      .      .      x      .          .       │  .      .      .    │ .     .      .      x      x   │ 
    │ DemoLift2          │ .      .      x      x      .          x       │  x      .      .    │ .     .      .      x      x   │ 
    │ DemoMainmenu       │ .      .      .      .      .          .       │  .      .      .    │ .     .      .      x      x   │ 
    │ DemoMediaPlayer    │ .      .      x      x      .          x       │  x      .      .    │ .     .      .      .      .   │ 
    │ DemoMetaballs      │ .      .      .      .      .          .       │  .      .      .    │ .     .      .      .      .   │ 
    │ DemoMeter          │ .      .      .      x      .          .       │  .      .      .    │ .     .      .      x      x   │ 
    │ DemoRefrigerator   │ .      .      .      .      .          x       │  .      .      .    │ x     .      .      .      .   │ 
    │ DemoRotaryDial     │ .      .      .      .      .          .       │  .      .      .    │ .     .      .      x      x   │ 
    │ DemoRunningBar     │ .      .      .      .      .          .       │  x      .      .    │ .     .      .      x      x   │ 
    │ DemoSignals        │ .      .      .      .      .          .       │  .      .      .    │ .     .      .      x      x   │ 
    │ DemoSignature      │ .      .      .      .      .          .       │  .      .      .    │ .     .      .      x      x   │ 
    │ DemoSketch         │ .      .      .      .      .          .       │  .      .      .    │ .     .      .      x      x   │ 
    │ DemoUnicode        │ .      .      .      .      .          .       │  x      .      .    │ .     .      .      x      x   │ 
    │ DemoUnicodeRuntime │ .      .      x      x      .          .       │  x      .      .    │ .     .      .      x      x   │ 
    │ DemoWashingMachine │ .      .      .      .      .          .       │  .      .      .    │ x     .      .      x      x   │ 
    │ SampleApp          │ .      .      .      .      .          .       │  .      .      .    │ .     .      .      x      x   │ 
    └────────────────────┴────────────────────────────────────────────────┴─────────────────────┴────────────────────────────────┘
    x = unsupport
    . = support
    
## The api sets of sample application

● Set 00: Sample usage of drawing primitives: points, lines, rectangles, bitmap, bitmap palette, bitmap palette background, fonts, text 8x8,text vga, bar graph, linestrips, edgestrips, scissor, polygon, cube, ball stencil, street map, additive blend text, macro usage, additive blend points.

● Set 01: Sample for CRC check, logo, widgets: clock, gauge, gradient, keys, progressbar, scroll, slider, dial, toggle, spinner, touch, powermode.

● Set 02: Sample for inflate, loadimage mono, loadimage fullcolor.

● Set 03: Sample for cmd_setfont.

● Set 04: Sample for widgets: Text, number, button, screensaver, snapshot, sketch, matrix, track, music player streaming.

● Set 05: Sample for custom font application: Chinese Font.

● Set 06: Sample for multi touch on screen with audio.

● Set 07: Sample for multi touch on screen with bouncing squares, bouncing circles, bouncing points and moving points. 

● Set 08: Sample for multi tracker, number bases, screen orientation, higher resolution bitmap, FT81x paletted bitmap, bitmap paletted8, dxt1 compressed image, simple video playback, video playback via media fifo, video playback frame by frame, video playback via command buffer, video playback from flash, video playback frame by frame from flash.

● Set 09: Sample for changing frequency, FT81x power off components, FT81x change pad drives strength.

● Set 10: Sample for ASTC layout on RAM_G, astc layout on flash, ASTC layout for a cell, flash state, program flash, mediafifo, cmdinflate, cmd_gradienta, cmd_fillwidth, rotate, animation, extended format font, reset font, OTP_FORMAT, coprocessor faults and restore.

● Set 11: Sample for BULK transfer using reg_cmdb_write/reg_cmdb_space.

● Set 12: Sample for image rotate by RotateAround, image rotate by Rotate and Translate.

● Set 13: Sample for Unicode string with re-index glyph data.

● Set 14: Sample usage of BT817/8 specific commands. 

Note:
    For Eve registers and commands/instructions definition, you can find it at the file common/eve_hal/EVE_GpuDef.h and common/eve_hal/Eve_CoCmd.h.
	  For Eve Emulator, it is for windows platform only and located at common/eve_hal/Bin/Simulation and common/eve_hal/Hdr。 
    For flash blob file used to access the eve connected flash chip, you can find it at common/eve_flash.  
    
## Version
    This version is v1.4.0-rc3
        
## Release log
    v1.4.0-rc3:
        - Fix minor issue: DemoGauges's UI broken on LCD WXGA
    v1.4.0-rc2
        - Fix QSPI connection issue of MM930LITE and MM817EV
        - Code clean up and minor bug fix     
    v1.4.0-rc1
        - Add 28 demo application and 1 sample application
        - Add Pico RP2040 platform support
        - Trigger the system reset (at bootup) for some platform which has no Power Down pin toggled (such as GD3X)
        
## License
Copyright (c) 2012-2021 Scott Chacon and others

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
