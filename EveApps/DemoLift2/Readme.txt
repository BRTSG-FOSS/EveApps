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
    This is demo application for EVE platform to draw a lift and a video 
    background on LCD.
    
    Lift can transition up to 5 levels. Transition time may be long, because it
    must play a video in each state below during travelling:
        LIFT_OPENED_STATE
        LIFT_WAIT_TO_CLOSE_STATE
        LIFT_CLOSING_STATE
        LIFT_CLOSED_STATE
        LIFT_WAIT_TO_MOVE_STATE
        LIFT_MOVE_UP_STATE

2. SUPPORTED PLATFORM
    Host platform:
        1. Microsoft Visual C++ platform with FT4222 and MPSSE
        2. Microsoft Visual C++ platform with Emulator

    EVE platform: Only BT81X series

    LCD size: WVGA (800x480)

3. SETUP AND RUN
    3.1 CONNECT HARDWARE
        3.1.1 Microsoft Visual C++ platform with FT4222 and MPSSE
            - Connect PC with EVE platform via FT4222 or MPSSE
            - Connect power to EVE platform
            
        3.1.2 Microsoft Visual C++ platform with Emulator
            - This setup uses window PC only
            
    3.2 BUILD AND RUN
        3.2.1 Microsoft Visual C++ platform with FT4222 and MPSSE
            - Open project in Project\MSVC with Microsoft Visual C++
            - Build (Ctrl + B)
            - Run (F5)
            
        3.2.2 Microsoft Visual C++ platform with Emulator
            - Open project in Project\Msvc_Emulator with Microsoft Visual C++
            - Build (Ctrl + B)
            - Run (F5)

4. CONFIGURATION INSTRUCTIONS
    Host platform: Defined with ESD_TARGET_PLATFORM, such as:
        - Window host: EVE_PLATFORM_FT4222, EVE_PLATFORM_MPSSE
        - FT9X host: MM900EV1A, MM900EV1B...
    
    EVE platform: Defined with ESD_TARGET_GRAPHICS, such as:
        EVE_GRAPHICS_BT815, EVE_GRAPHICS_BT816, EVE_GRAPHICS_BT817, 
        EVE_GRAPHICS_BT818...
    
    Please see common\eve_hal\EVE_Config.h. for more macro.
            
                                   【END】