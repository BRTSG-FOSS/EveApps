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
 * Abstract: Initialize EVE and start sample SETs
 * Author : Bridgetek
 *
 * Revision History:
 * 1.0 - date 2018.07.30 - Initial for BT816
 */

#include "Platform.h"
#include "Common.h"
#include "App.h"
#include "eab.h"

Ft_Gpu_HalInit_t halInit;
EVE_HalContext ph, *phost;
static bool s_Running = true;


bool loop(EVE_HalContext *phost)
{
	//EVE_Hal_displayMessageZ(phost, "EVE Hal Test");

	eab_test_feature(phost);

	return false;
}

int32_t main(int32_t argc, char8_t *argv[])
{
	phost = &ph;

	/* Initialize HAL */
	EVE_Hal_initialize();

	/* Open the device interactively */
	// if (EVE_Util_openDeviceInteractive(phost, L"Flash.bin"))
	if (!EVE_Util_openDeviceInteractive(phost, NULL))
	{
		printf("Failed to open HAL\n");
	}
	else
	{
		if (EVE_Util_bootupConfigInteractive(phost, EVE_DISPLAY_DEFAULT))
		{
			while (s_Running && phost->Status != EVE_STATUS_CLOSED) // TODO: Deal with emulator closing (EVE_STATUS_CLOSED/EVE_STATUS_ERROR?)
			{
				if (!loop(phost))
					s_Running = false;
			}
			if (phost->Status == EVE_STATUS_CLOSED)
			{
				printf("Device closed unexpectedly\n");
			}
		}
		else
		{
			printf("Failed to bootup the device\n");
		}

		EVE_Hal_close(phost);
	}

	EVE_Hal_release();
	return 0;
}
