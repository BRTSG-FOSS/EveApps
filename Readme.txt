================================================================================
 Copyright (c) Bridgetek Pte Ltd.                                               
                                                                                
 THIS SOFTWARE IS PROVIDED BY BRIDGETEK PTE LTD. ``AS IS'' AND ANY EXPRESS OR   
 IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF   
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO     
 EVENT SHALL BRIDGETEK PTE LTD BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,  
 SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,   
 PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES, LOSS OF USE, DATA, OR PROFITS OR  
 BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER  
 IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)     
 ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE     
 POSSIBILITY OF SUCH DAMAGE.                                                    
================================================================================

1. INTRODUCTION
    EveApps is a set of applications based on Eve Series chip (FT80X, FT81X, BT815/6, BT817/8) for users to refer to.
    
    It contains 26 demo application and 1 sample application. They are all written in ANSI C code and full source code is enclosed.
    Demo application is for demo purpose and fair complex, which is closed to real-life project. 
    Sample application is for the tutorial purpose which is simpler and easier to understand each display list or command.   
    For beginners, please always start from SampleApp project first.
    
    Please note that some of the applications are developed for certain Eve Series chips or host platforms.    
    Please check the Readme.txt of application directory for more details.
    
    All the applications here depend on a set of common files under "common" folder. 
    Each application has one subfolder "project" which contains the project file used to build it up from toolchain.
    For FT4222/MPSSE or Eve Emulator platform, the project file is for Microsoft Visual Studio 2017 IDE.
    For FT90X/FT930 platform, the project is for Eclipse IDE from Bridgetek. 
	
    Users are assumed to be familiar with the programming guide and data sheet of Eve Series chips. 
    
2. FOLDER INTRODUCTION
    ├───common                  
    │   ├───app_common          | Application's common functions
    │   ├───eve_flash           | Blob binary for flash programming
    │   └───eve_hal             | Hardware abstraction layer to bridge different platforms
    ├───EveApps                  
    │    ├───DemoCircleArea      | Image viewer demo
    │    ├───DemoEvChargePoint   | Electric charging station demo
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
    │    ├───DemoMetaballs       | Balls demo
    │    ├───DemoMeterDemo       | Meter demo 
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
    ├───EveApps_Configure        | Configure EveApps with a GUI
    
3. SUPPORT PLATFORMS

    ┌─────────────────────┬────────────────────────────────────────────────────┬─────────────────────────┬──────────────────────────────────────┐
    │ Application         │ MPSSE  FT4222  FT900  FT930  Emulator  MM2040EV    │  FT800  BT815  BT817    │    QVGA  WQVGA  WVGA   WSVGA  WXGA   │ 
    ├─────────────────────┼────────────────────────────────────────────────────┼─────────────────────────┼──────────────────────────────────────┤
    │ DemoAudioPlayback   │ .      .      .      .      .          .           │  x      .      .        │    .     .      .      x      x      │ 
    │ DemoCircleView      │ .      .      .      .      .          .           │  x      .      .        │    .     .      .      .      x      │ 
    │ DemoEvChargePoint   │ .      .      .      x      .          .           │  x      .      .        │    x     x      x      x      x      │ 
    │ DemoFlashBitbang    │ .      .      .      .      .          .           │  x      .      .        │    .     .      .      .      .      │ 
    │ DemoGauges          │ .      .      .      .      .          .           │  .      .      .        │    .     .      .      x      x      │ 
    │ DemoGradient        │ .      .      .      .      .          .           │  .      .      .        │    .     .      .      x      x      │ 
    │ DemoGraph           │ .      .      .      .      .          .           │  .      .      .        │    .     .      .      x      x      │ 
    │ DemoHDPictureViewer │ .      .      .      .      .          .           │  x      .      .        │    .     .      .      x      x      │ 
    │ DemoImageViewer     │ .      .      .      .      .          .           │  .      .      .        │    .     .      .      x      x      │ 
    │ DemoImageViewer2    │ .      .      .      x      .          .           │  x      .      .        │    .     .      .      x      x      │ 
    │ DemoInstrument      │ .      .      x      x      .          .           │  x      .      .        │    .     .      .      x      x      │ 
    │ DemoJackpot         │ .      .      .      .      .          .           │  .      .      .        │    .     .      .      x      x      │ 
    │ DemoKeyboard        │ .      .      .      .      .          .           │  .      .      .        │    .     .      .      .      .      │ 
    │ DemoLift            │ .      .      .      x      .          .           │  .      .      .        │    .     .      .      x      x      │ 
    │ DemoLift2           │ .      .      x      x      .          x           │  x      .      .        │    .     .      .      x      x      │ 
    │ DemoMainmenu        │ .      .      .      .      .          .           │  .      .      .        │    .     .      .      x      x      │ 
    │ DemoMediaPlayer     │ .      .      x      x      .          x           │  x      .      .        │    .     .      .      .      .      │ 
    │ DemoMetaballs       │ .      .      .      .      .          .           │  .      .      .        │    .     .      .      .      .      │ 
    │ DemoMeter           │ .      .      .      x      .          .           │  .      .      .        │    .     .      .      x      x      │ 
    │ DemoRefrigerator    │ .      .      .      .      .          x           │  .      .      .        │    x     .      .      .      .      │ 
    │ DemoRotaryDial      │ .      .      .      .      .          .           │  .      .      .        │    .     .      .      x      x      │ 
    │ DemoRunningBar      │ .      .      .      .      .          .           │  x      .      .        │    .     .      .      x      x      │ 
    │ DemoSignals         │ .      .      .      .      .          .           │  .      .      .        │    .     .      .      x      x      │ 
    │ DemoSignature       │ .      .      .      .      .          .           │  .      .      .        │    .     .      .      x      x      │ 
    │ DemoSketch          │ .      .      .      .      .          .           │  .      .      .        │    .     .      .      x      x      │ 
    │ DemoUnicode         │ .      .      .      .      .          .           │  x      .      .        │    .     .      .      x      x      │ 
    │ DemoUnicodeRuntime  │ .      .      x      x      .          .           │  x      .      .        │    .     .      .      x      x      │ 
    │ DemoWashingMachine  │ .      .      .      .      .          .           │  .      .      .        │    x     .      .      x      x      │ 
    │ SampleApp           │ .      .      .      .      .          .           │  .      .      .        │    .     .      .      x      x      │ 
    └─────────────────────┴────────────────────────────────────────────────────┴─────────────────────────┴──────────────────────────────────────┘
    x = unsupport
    . = support
    * Pico platform is built with cmake and nmake, example:
       $mkdir build && cd build
       $cmake -G "NMake Makefiles" -DEVE_APPS_PLATFORM=RP2040 -DEVE_APPS_GRAPHICS=EVE_GRAPHICS_BT817 -DEVE_DISPLAY=EVE_DISPLAY_WVGA ..
       $nmake SampleApp
        
4. THE API SETS OF SAMPLE APPLICATION
    ● Set 00: Sample usage of drawing primitives: 
	                     points, lines, rectangles, bitmap, bitmap palette, bitmap palette background, fonts, text 8x8,
                      	 text vga, bar graph, linestrips, edgestrips, scissor, polygon, cube, ball stencil, street map, 
						 additive blend text, macro usage, additive blend points.
    ● Set 01: Sample for CRC check, logo, widgets: clock, gauge, gradient, keys, 
	                     progressbar, scroll, slider, dial, toggle, spinner, touch, powermode.
    ● Set 02: Sample for inflate, loadimage mono, loadimage fullcolor.
    ● Set 03: Sample for cmd_setfont.
    ● Set 04: Sample for widgets: Text, number, button, screensaver, snapshot, sketch, matrix, track, music player streaming.
    ● Set 05: Sample for custom font application: Chinese Font.
    ● Set 06: Sample for multi touch on screen with audio.
    ● Set 07: Sample for multi touch on screen with bouncing squares, bouncing circles, bouncing points and moving points. 
    ● Set 08: Sample for multi tracker, number bases, screen orientation, higher resolution bitmap, FT81x paletted bitmap, bitmap paletted8, 
	                     dxt1 compressed image, simple video playback, video playback via media fifo, video playback frame by frame, 
						 video playback via command buffer, video playback from flash, video playback frame by frame from flash.
    ● Set 09: Sample for changing frequency, FT81x power off components, FT81x change pad drives strength.
    ● Set 10: Sample for ASTC layout on RAM_G, astc layout on flash, ASTC layout for a cell, flash state, program flash, 
	                     mediafifo, cmdinflate, cmd_gradienta, cmd_fillwidth, rotate, animation, extended format font, reset font, 
						 OTP_FORMAT, coprocessor faults and restore.
    ● Set 11: Sample for BULK transfer using reg_cmdb_write/reg_cmdb_space.
    ● Set 12: Sample for image rotate by RotateAround, image rotate by Rotate and Translate.
    ● Set 13: Sample for Unicode string with re-index glyph data.
    ● Set 14: Sample usage of BT817/8 specific commands. 

Note:
    For Eve registers and commands/instructions definition, you can find it at the file common/eve_hal/EVE_GpuDef.h and common/eve_hal/Eve_CoCmd.h.
	For Eve Emulator, it is for windows platform only and located at common/eve_hal/Bin/Simulation and common/eve_hal/Hdr。 
    For flash blob file used to access the eve connected flash chip, you can find it at common/eve_flash.        
            
5. VERSION
    v1.4-rc2
        - Support Raspberry Pico RP2040 MCU
        - Change test folder location in the SD to EveApps/[application name]/Test
        - Fix QSPI connection issue of MM930LITE and MM817EV
        - Code clean up and minor bug fix
        
6. RELEASE LOG
    v1.4-rc3
        - Fix bug: DemoGauges's UI broken on LCD WXGA
        - Add RST_PULSE at boot up (supported GD3X)
    v1.4-rc2
        - Minor bug fix
        - single header (eve.h) is excluded 
    v1.4-rc1
        - Add single header eve.h for the whole common folder
        - Port every Demo to use single header version (eve.h) 
        - Trigger the system reset (at bootup) for some platform which has no Power Down pin toggled (such as GD3X)
        - Add Pico RP2040 platform support         
    v1.3-rc2:
        - Fix bug: Broken image in portrait mode of DemoLift
        - Fix bug: Compile error on DemoFlashBitbang and DemoHDPictureviewer
        - Fix bug: Crashing in DemoFlashBitBang
    v1.3-rc1:
        - Add 3 demo:
            + DemoAudioPlayback
            + DemoFlashBitbang
            + DemoMediaPlayer
        - Minor bugs fix
    v1.2-rc3:
        - Resolve SonarQube code smells and bugs in common folder
        - Add Yes/No dialog to get user approvement before program flash at initialize
    v1.2-rc2:
        - Resolve SonarQube code smells and bugs in SampleApp
    v1.2-rc1:
        - Fix issue: Bad image and font on DemoEvChargePoint Emulator
        - Set default for every project: EVE_GRAPHICS_ME817EV, EVE_PLATFORM_FT4222, EVE_DISPLAY_WVGA, MM900EV1B and MM930LITE
        - Add macro ENABLE_FAST_MOVE: Set to reduce floor transition time
        - Fix issue: Text strikethrough on rom font 32 to 34 (SampleApp)
    v1.1-rc2: 
        -  Fix issue: Incorrect UI of DemoLift
        -  Add a touch calibration when booting up in EvChargePoint demo
    v1.1-rc1: 
        - Add DemoInstrument
        - Remove Eve_EmulatorMain.c/h
        - Add star moving arround in DemoSignature
        - REG_CYA_CONFIG renamed to REG_TOUCH_CONFIG
        - Fix bug: sd card not initializing after reset
        - Fix bug: compile error on FT801, FT810, FT812 and FT813
        - Fix bug: scroll down issue on DemoJacpot
    v1.0-rc3
        - Bug fix
    v1.0-rc2:
        - Bug fix
    v1.0-rc1:
        - Add SampleApp and 24 demo application
        - Support Eve Emulator, FT90X/FT930 and FT4222/MPSSE
     
                                   【END】
