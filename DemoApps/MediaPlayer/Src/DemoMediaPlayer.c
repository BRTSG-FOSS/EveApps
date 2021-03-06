/*
 * Copyright (c) Bridgetek Pte Ltd
 *
 * THIS SOFTWARE IS PROVIDED BY BRIDGETEK PTE LTD "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 * BRIDGETEK PTE LTD BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES LOSS OF USE, DATA, OR PROFITS OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
 * TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * BRIDGETEK DRIVERS MAY BE USED ONLY IN CONJUNCTION WITH PRODUCTS BASED ON BRIDGETEK PARTS.
 *
 * BRIDGETEK DRIVERS MAY BE DISTRIBUTED IN ANY FORM AS LONG AS LICENSE INFORMATION IS NOT MODIFIED.
 *
 * IF A CUSTOM VENDOR ID AND/OR PRODUCT ID OR DESCRIPTION STRING ARE USED, IT IS THE
 * RESPONSIBILITY OF THE PRODUCT MANUFACTURER TO MAINTAIN ANY CHANGES AND SUBSEQUENT WHQL
 * RE-CERTIFICATION AS A RESULT OF MAKING THESE CHANGES.
 *
 * Abstract: Application to demonstrate function of EVE.
 * Author : Bridgetek
 */

 /*
  * Description: Demo application for EVE platform
  */

#include "Platform.h"

#include "Common.h"
#include "App.h"
#include "FT_Util.h"

static EVE_HalContext *s_pHalContext;

ft_int32_t MultimediaExplorer_Main(Ft_Gpu_Hal_Context_t* pHalContext, GuiManager* CleO, int command, void* _data);
ft_void_t Info(Ft_Gpu_Hal_Context_t* pHalContext);

void DemoMediaPlayer(EVE_HalContext* pHalContext) {
#if !defined(EVE_FLASH_AVAILABLE)
    #error Platform not supported
#endif
	s_pHalContext = pHalContext;
	FlashHelper_SwitchFullMode(s_pHalContext);

    init_guiobs(s_pHalContext);

    while(1) {
        MultimediaExplorer_Main(s_pHalContext, getGuiInstance(), 0, NULL);
    }
}

