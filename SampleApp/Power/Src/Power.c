/*
 * Copyright (c) Bridgetek Pte Ltd
 *
 * THIS SOFTWARE IS PROVIDED BY BRIDGETEK PTE LTD "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL BRIDGETEK PTE LTD BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES LOSS OF USE, DATA, OR PROFITS OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. BRIDGETEK DRIVERS MAY BE USED ONLY
 * IN CONJUNCTION WITH PRODUCTS BASED ON BRIDGETEK PARTS. BRIDGETEK DRIVERS MAY BE DISTRIBUTED
 * IN ANY FORM AS LONG AS LICENSE INFORMATION IS NOT MODIFIED. IF A CUSTOM VENDOR ID AND/OR PRODUCT
 * ID OR DESCRIPTION STRING ARE USED, IT IS THE RESPONSIBILITY OF THE PRODUCT MANUFACTURER TO
 * MAINTAIN ANY CHANGES AND SUBSEQUENT WHQL RE-CERTIFICATION AS A RESULT OF MAKING THESE CHANGES.
 */

 /* Abstract: Application to demonstrate function of EVE.
  * Author : Bridgetek
  */

#include "Common.h"
#include "Platform.h"
#include "EVE_CoCmd.h"
#include "Power.h"

#define SAMAPP_INFO_TEXT(str)  Draw_TextColor(s_pHalContext, str, (uint8_t[]) { 0x77, 0x77, 0x77 }, (uint8_t[]) { 255, 255, 255 })
#define SAMAPP_INFO_START      Display_StartColor(s_pHalContext,  (uint8_t[]) { 0x77, 0x77, 0x77 }, (uint8_t[]) { 255, 255, 255 })
#define SAMAPP_INFO_END        Display_End(s_pHalContext);
#define SAMAPP_DELAY_NEXT      EVE_sleep(2000);

static EVE_HalContext s_halContext;
static EVE_HalContext* s_pHalContext;
void SAMAPP_Power();

/**
 * @brief Main function to initialize EVE, do calibrate and start application
 * 
 * @param argc Unuse
 * @param argv Unuse
 * @return int 0
 */
int main(int argc, char* argv[])
{
    s_pHalContext = &s_halContext;
    Gpu_Init(s_pHalContext);

    // read and store calibration setting
#if !defined(BT8XXEMU_PLATFORM) && GET_CALIBRATION == 1
    Esd_Calibrate(s_pHalContext);
    Calibration_Save(s_pHalContext);
#endif

    EVE_Util_clearScreen(s_pHalContext);

    char *info[] =
    {  "EVE Sample Application",
        "This sample demonstrate the power control", 
        "",
        ""
    }; 

    while (TRUE) {
        WelcomeScreen(s_pHalContext, info);

        SAMAPP_Power();

        EVE_Util_clearScreen(s_pHalContext);

        EVE_Hal_close(s_pHalContext);
        EVE_Hal_release();

        /* Init HW Hal for next loop*/
        Gpu_Init(s_pHalContext);
#if !defined(BT8XXEMU_PLATFORM) && GET_CALIBRATION == 1
        Calibration_Restore(s_pHalContext);
#endif
    }
    return 0;
}

/**
* @brief Reinit SPI connection
* Power commands should be executred in SPI single channel mode
*/
static void helperReinitSPI()
{
#if defined(FT81X_ENABLE)                                     \
    && (defined(FT900_PLATFORM) || defined(FT93X_PLATFORM))   \
    && (defined(ENABLE_SPI_QUAD) || defined(ENABLE_SPI_DUAL))
    /* api to set quad and numbe of dummy bytes */
    /* Set EVE in single channel mode */
    EVE_Hal_setSPI(s_pHalContext, GPU_SPI_SINGLE_CHANNEL, GPU_SPI_ONEDUMMY);

    /* Set FT900 to single channel mode */
    spi_init(SPIM, spi_dir_master, spi_mode_0, 4);
    spi_option(SPIM, spi_option_fifo_size, 64);
    spi_option(SPIM, spi_option_fifo, 1);
    spi_option(SPIM, spi_option_fifo_receive_trigger, 1);
    spi_option(SPIM, spi_option_bus_width, 1);
#endif

#if defined(FT4222_PLATFORM)
    /* All host commands of FT81x (except system reset) should be executed
    * in Single channel mode
    */
    if (s_pHalContext->spichannel == SPI_IO_QUAD || s_pHalContext->spichannel == SPI_IO_DUAL)
        EVE_Hal_setSPI(s_pHalContext, GPU_SPI_SINGLE_CHANNEL, GPU_SPI_ONEDUMMY);
#endif
}

/**
* @brief Change the clock frequency for FT81X series chips
*
*/
void SAMAPP_Power_changeFreq()
{
#if defined(FT81X_ENABLE) // FT81X only
    Draw_Text(s_pHalContext, "Example for: Change the clock frequency for FT81X series chips");

    fadeout(s_pHalContext);
    Play_MuteSound(s_pHalContext); //Play mute sound to avoid pop sound
    EVE_Host_powerModeSwitch(s_pHalContext, GPU_SLEEP_M); //put device in sleep mode
    EVE_sleep(50);

    //change the clock frequency to 60mhz during sleep mode to avoid any behaviors during the clock transition period.
    EVE_Host_selectSysClk(s_pHalContext, GPU_SYSCLK_48M); //change system frequency to 48mhz

    EVE_Host_powerModeSwitch(s_pHalContext, GPU_ACTIVE_M); //put device in active mode
    EVE_sleep(50);
    SAMAPP_INFO_TEXT("System clock is now at 48mhz.");
    fadein(s_pHalContext);
    EVE_sleep(1000);

    fadeout(s_pHalContext);
    Play_MuteSound(s_pHalContext); //Play mute sound to avoid pop sound
    EVE_Host_powerModeSwitch(s_pHalContext, GPU_SLEEP_M);
    EVE_sleep(50);

    //change the clock frequency to 60mhz during sleep mode to avoid any behaviors during the clock transition period.
    EVE_Host_selectSysClk(s_pHalContext, GPU_SYSCLK_60M); //change system frequency to 60mhz

    EVE_Host_powerModeSwitch(s_pHalContext, GPU_ACTIVE_M);
    EVE_sleep(50);
    SAMAPP_INFO_TEXT("System clock is now at 60mhz.");
    fadein(s_pHalContext);
    SAMAPP_DELAY_NEXT;
#endif // FT81X only
}

/**
* @brief When some ROMs and ADC are not being used they can be powered off to save power
*
*  Application should retain the ROMs and ADCs setting on its own as the state of 
*  the ROMs and ADCs are not readable from the chip.
*
*/
void SAMAPP_Power_powerOffComponents()
{
#if defined(FT81X_ENABLE) // FT81X only
    Draw_Text(s_pHalContext, "Example for: Power off unused ROMs and ADC Components");

    SAMAPP_INFO_TEXT("Disabling Audio component");
    GPU_81X_PowerOffComponents(s_pHalContext, 0x20 | 0x10); //disable audio
    EVE_sleep(1000);

    // Power on all ROMs and ADC
    SAMAPP_INFO_TEXT("Power on all ROMs and ADC component");
    GPU_81X_PowerOffComponents(s_pHalContext, 0x00);

    SAMAPP_DELAY_NEXT;
#endif // Win32 FT81X only
}

/**
* @brief The current of the GPIOs and IOs can be increased upto 20 milliamps or decrease down to 5 milliamps, by default the GPIOs and IOs specified in the GPU_81X_GPIO_GROUP_T table are outputing 5 milliamps.
*
*/
void SAMAPP_Power_padDriveStrength()
{
#if defined(FT81X_ENABLE) // FT81X only
    Draw_Text(s_pHalContext, "Example for: GPIOs drive strenght");

    fadeout(s_pHalContext);
    Play_MuteSound(s_pHalContext); //Play mute sound to avoid pop sound
    GPU_81X_PadDriveStrength(s_pHalContext, GPU_10MA, GPU_GPIO0); //change GPIO0

    SAMAPP_INFO_TEXT("GPIO0 is now driving 10 milliamps.");
    fadein(s_pHalContext);
    SAMAPP_DELAY_NEXT;
#endif // FT81X only
}

/**
* @brief demonstration to hold the device during the system reset.
*
*/
void SAMAPP_Power_holdResetState()
{
#if defined(FT81X_ENABLE) // FT81X only
    Draw_Text(s_pHalContext, "Example for: Hold the device during the system reset");

    fadeout(s_pHalContext);
    Play_MuteSound(s_pHalContext); //Play mute sound to avoid pop sound
    SAMAPP_INFO_TEXT( "Reset state hold.");
    EVE_Host_coreReset(s_pHalContext); //perform a device reset.
    EVE_Host_resetActive(s_pHalContext); //hold the device in reset state
    EVE_sleep(1000);

    //during the device reset holding period, the application can perform operations that require the device to be non-operational or pre-operational status.
    EVE_Host_resetRemoval(s_pHalContext); //exit reset state, the device will power on and enter into its default state.
    BootupConfig(s_pHalContext); //reconfigure the display
    SAMAPP_INFO_TEXT( "Reset state released.");
    fadein(s_pHalContext);
    SAMAPP_DELAY_NEXT;
#endif // FT81X only
}

/**
* @brief Switching power mode
*
*/
void SAMAPP_Power_powerMode()
{
    Draw_Text(s_pHalContext, "Example for: Switching power mode");

    /*************************************************
    Senario1:  Transition from Active mode to Standby mode.
    Transition from Standby mode to Active Mode
    **************************************************/
    //Switch FT800 from Active to Standby mode
    fadeout(s_pHalContext);
    Play_MuteSound(s_pHalContext); //Play mute sound to avoid pop sound
    helperReinitSPI();

    EVE_Host_powerModeSwitch(s_pHalContext, GPU_STANDBY_M);
    //Wake up from Standby first before accessing FT800 registers.
    EVE_Host_powerModeSwitch(s_pHalContext, GPU_ACTIVE_M);
    SAMAPP_INFO_TEXT( "Power Scenario 1");
    fadein(s_pHalContext);
    EVE_sleep(1000);

    /*************************************************
    Senario2:  Transition from Active mode to Sleep mode.
    Transition from Sleep mode to Active Mode
    **************************************************/
    //Switch FT800 from Active to Sleep mode
    fadeout(s_pHalContext);
    Play_MuteSound(s_pHalContext); //Play mute sound to avoid pop sound
    EVE_Host_powerModeSwitch(s_pHalContext, GPU_SLEEP_M);
    EVE_sleep(50);
    //Wake up from Sleep
    EVE_Host_powerModeSwitch(s_pHalContext, GPU_ACTIVE_M);
    EVE_sleep(50);
    SAMAPP_INFO_TEXT( "Power Scenario 2");
    fadein(s_pHalContext);
    EVE_sleep(1000);

    /*************************************************
    Senario3:  Transition from Active mode to PowerDown mode.
    Transition from PowerDown mode to Active Mode via Standby mode.
    **************************************************/
    //Switch FT800 from Active to PowerDown mode by sending command
    fadeout(s_pHalContext);
    Play_MuteSound(s_pHalContext); //Play mute sound to avoid pop sound
    helperReinitSPI();

    EVE_Host_powerModeSwitch(s_pHalContext, GPU_POWERDOWN_M);
    EVE_sleep(50);
    BootupConfig(s_pHalContext);
    //Need download display list again because power down mode lost all registers and memory
    SAMAPP_INFO_TEXT( "Power Scenario 3");
    fadein(s_pHalContext);
    EVE_sleep(1000);

    /*************************************************
    Senario4:  Transition from Active mode to PowerDown mode by toggling PDN from high to low.
    Transition from PowerDown mode to Active Mode via Standby mode.
    **************************************************/
    //Switch FT800 from Active to PowerDown mode by toggling PDN
    fadeout(s_pHalContext);
    Play_MuteSound(s_pHalContext); //Play mute sound to avoid pop sound

    /* Toggling PD_N pin power cycles FT81X and all register contents are lost and reset
    to default when the chip is next switched on. This puts FT81X SPI to single mode
    leaving FT4222 in multi channel if ENABLE_SPI_QUAD/ENABLE_SPI_DUAL enabled.
    Hence explicilty switching the communication to single before power cycle.
    */
    helperReinitSPI();

    EVE_Hal_powerCycle(s_pHalContext, FALSE);
    BootupConfig(s_pHalContext);
    //Need download display list again because power down mode lost all registers and memory
    SAMAPP_INFO_TEXT( "Power Scenario 4");
    fadein(s_pHalContext);
    EVE_sleep(1000);

    /*************************************************
    Senario5:  Transition from Active mode to PowerDown mode via Standby mode.
    Transition from PowerDown mode to Active mode via Standby mode.
    **************************************************/
    //Switch FT800 from Active to standby mode
    fadeout(s_pHalContext);
    Play_MuteSound(s_pHalContext); //Play mute sound to avoid pop sound
    helperReinitSPI();

    EVE_Host_powerModeSwitch(s_pHalContext, GPU_STANDBY_M);
    EVE_Hal_powerCycle(s_pHalContext, FALSE);

    BootupConfig(s_pHalContext);
    //Need download display list again because power down mode lost all registers and memory
    Draw_Text(s_pHalContext, "Power Scenario 5");
    fadein(s_pHalContext);
    EVE_sleep(1000);

    /*************************************************
    Senario6:  Transition from Active mode to PowerDown mode via Sleep mode.
    Transition from PowerDown mode to Active mode via Standby mode.
    **************************************************/
    //Switch FT800 from Active to standby mode
    fadeout(s_pHalContext);
    Play_MuteSound(s_pHalContext); //Play mute sound to avoid pop sound
    helperReinitSPI();

    EVE_Host_powerModeSwitch(s_pHalContext, GPU_SLEEP_M);
    EVE_Hal_powerCycle(s_pHalContext, FALSE); //go to powerdown mode

    BootupConfig(s_pHalContext);
    //Need download display list again because power down mode lost all registers and memory
    Draw_Text(s_pHalContext, "Power Scenario 6");
    fadein(s_pHalContext);
    SAMAPP_DELAY_NEXT;
}

void SAMAPP_Power() {
    SAMAPP_Power_changeFreq();
    SAMAPP_Power_powerOffComponents();
    SAMAPP_Power_padDriveStrength();
    SAMAPP_Power_holdResetState();
}


