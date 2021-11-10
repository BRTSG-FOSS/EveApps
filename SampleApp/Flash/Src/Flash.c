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
#include "Flash.h"

static EVE_HalContext s_halContext;
static EVE_HalContext* s_pHalContext;
void SAMAPP_Flash();

int main(int argc, char* argv[])
{
    s_pHalContext = &s_halContext;
    Gpu_Init(s_pHalContext);

    // read and store calibration setting
#if !defined(BT8XXEMU_PLATFORM) && GET_CALIBRATION == 1
    Esd_Calibrate(s_pHalContext);
    Calibration_Save(s_pHalContext);
#endif

    Flash_Init(s_pHalContext, TEST_DIR "/Flash/BT81X_Flash.bin", "BT81X_Flash.bin");
    EVE_Util_clearScreen(s_pHalContext);

    char *info[] =
    {  "EVE Sample Application",
        "This sample demonstrate the using of flash", 
        "",
        ""
    }; 

    while (TRUE) {
        WelcomeScreen(s_pHalContext, info);

        SAMAPP_Flash();

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

char* flashMessage[50];
int messageIndex = 0;

void healperAppendMessage(char *s, int num)
{
    flashMessage[messageIndex] = (char*)malloc(num);
    if (!flashMessage[messageIndex]) {
        APP_ERR("Malloc failed");
        return;
    }

    memcpy(flashMessage[messageIndex], s, num);
    

    EVE_CoCmd_dlStart(s_pHalContext);
    EVE_Cmd_wr32(s_pHalContext, CLEAR_COLOR_RGB(0xcc, 0xcc, 0xcc));
    EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));
    EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));
    EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0, 0, 0));

    int y = 0;
    int x = 20;
    for (int i = 0; i <= messageIndex; i++) {
        EVE_CoCmd_text(s_pHalContext, x, y, 23, 0, flashMessage[i]);
        y += 30;
    }
    Display_End(s_pHalContext);

    messageIndex++;
    EVE_sleep(500);
}

void healperClearMessage() {
    for (int i = 0; i < messageIndex; i++) {
        free(flashMessage[i]);
    }
    messageIndex = 0;
}


/**
* @brief API to demonstrate flash states
*
*/
void SAMAPP_Flash_state() {
    const char* flash_status[4] = { "INIT", "DETACHED", "BASIC", "FULL" };
    char mes[50];
    int mesLen = 50;

    Draw_Text(s_pHalContext, "Example for: Flash states");

    printf("\n\nFLASH STATES\n");
    uint8_t status = EVE_Hal_rd8(s_pHalContext, REG_FLASH_STATUS);
    sprintf(mes, "Flash State: %s (%d)\n", flash_status[status], status);
    healperAppendMessage(mes, mesLen);
    EVE_sleep(1000);

    sprintf(mes, "CMD_FLASHDETACH");
    healperAppendMessage(mes, mesLen);

    EVE_CoCmd_flashDetach(s_pHalContext);
    EVE_Cmd_waitFlush(s_pHalContext);

    status = EVE_Hal_rd8(s_pHalContext, REG_FLASH_STATUS);
    if (status != FLASH_STATUS_DETACHED)
    {
        sprintf(mes, "Flash is not able to detach");
        healperAppendMessage(mes, mesLen);
    }
    else
    {
        sprintf(mes, "Flash is detached");
        healperAppendMessage(mes, mesLen);
    }
    sprintf(mes, "Flash State: %s (%d)\n", flash_status[status], status);
    healperAppendMessage(mes, mesLen);
    EVE_sleep(1000);

    sprintf(mes, "CMD_FLASHATTACH");
    healperAppendMessage(mes, mesLen);

    EVE_CoCmd_flashAttach(s_pHalContext);
    EVE_Cmd_waitFlush(s_pHalContext);

    status = EVE_Hal_rd8(s_pHalContext, REG_FLASH_STATUS);
    if (status != FLASH_STATUS_BASIC)
    {
        sprintf(mes, "Flash is not able to attach");
        healperAppendMessage(mes, mesLen);
    }
    else
    {
        sprintf(mes, "Flash is attached");
        healperAppendMessage(mes, mesLen);
    }
    sprintf(mes, "Flash State: %s (%d)\n", flash_status[status], status);
    healperAppendMessage(mes, mesLen);
    EVE_sleep(1000);

    sprintf(mes, "CMD_FLASHFAST");
    healperAppendMessage(mes, mesLen);

    EVE_CoCmd_flashFast(s_pHalContext, 0);
    EVE_Cmd_waitFlush(s_pHalContext);
    status = EVE_Hal_rd8(s_pHalContext, REG_FLASH_STATUS);

    if (status != FLASH_STATUS_FULL)
    {
        sprintf(mes, "Flash is not able to get into full mode");
        healperAppendMessage(mes, mesLen);
    }
    else
    {
        sprintf(mes, "Flash gets into full mode");
        healperAppendMessage(mes, mesLen);
    }
    sprintf(mes, "Flash State: %s (%d)\n", flash_status[status], status);
    healperAppendMessage(mes, mesLen);
    EVE_sleep(1000); 

    sprintf(mes, "Check Flash Size");
    healperAppendMessage(mes, mesLen);

    status = EVE_Hal_rd8(s_pHalContext, REG_FLASH_SIZE);
    sprintf(mes, "Flash size : %d MBytes", status);
    healperAppendMessage(mes, mesLen);

    healperClearMessage();
    EVE_sleep(2000);
}

/**
* @brief API to demonstrate flash programming
*
*/
void SAMAPP_Flash_program() {
    char mes[50];
    int mesLen = 50;

    Draw_Text(s_pHalContext, "Example for: Flash program");

    /* Erase the flash */
    FlashHelper_SwitchState(s_pHalContext, FLASH_STATUS_FULL);
    EVE_Cmd_waitFlush(s_pHalContext);

    sprintf(mes, "Erasing flash");
    healperAppendMessage(mes, mesLen);
    EVE_CoCmd_flashErase(s_pHalContext);
    EVE_CoCmd_nop(s_pHalContext);
    EVE_Cmd_waitFlush(s_pHalContext);

    sprintf(mes, "Flash is erased");
    healperAppendMessage(mes, mesLen);

    sprintf(mes, "Programming flash....");
    healperAppendMessage(mes, mesLen);
    EVE_sleep(500);

    // write flash
    Ftf_Write_File_To_Flash_With_Progressbar(s_pHalContext, 
        TEST_DIR "/Flash/BT81X_Flash.bin", "BT81X_Flash.bin", 0);
    
    healperClearMessage();    
    EVE_sleep(2000);
}

void SAMAPP_Flash() {
    SAMAPP_Flash_state();
    SAMAPP_Flash_program();
}


