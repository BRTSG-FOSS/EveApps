
#include <stdio.h>
#include <EVE_Hal.h>

static EVE_HalContext s_Host[2];
static bool s_Running = true;

static uint64_t s_Millis = 0;

static void displayClock(EVE_HalContext *phost)
{
	EVE_CoCmd_dlStart(phost);
	EVE_CoCmd_dl(phost, CLEAR_COLOR_RGB(0, 0, 0));
	EVE_CoCmd_dl(phost, CLEAR(1, 1, 1));
	EVE_CoCmd_clock(phost, phost->Width >> 1, phost->Height >> 1,
		(min(phost->Height, phost->Width) >> 1) - 32, 0, /* FIXME: OPT_3D is in the documentation as value 0 */
		(s_Millis / 1000 / 60 / 60) % 24, (s_Millis / 1000 / 60) % 60,
		(s_Millis / 1000) % 60, s_Millis % 1000);
	EVE_CoCmd_dl(phost, DISPLAY());
	EVE_CoCmd_swap(phost);
}

static bool loop()
{
	s_Millis = EVE_millis64();

	displayClock(&s_Host[0]);
	displayClock(&s_Host[1]);

	EVE_Cmd_waitFlush(&s_Host[0]);
	EVE_Cmd_waitFlush(&s_Host[1]);

	return true;
}

static bool openDevice(int i)
{
	EVE_HalContext *phost = &s_Host[i];
	if (EVE_Util_openDeviceInteractive(phost, NULL))
	{
		if (EVE_Util_bootupConfigInteractive(phost, EVE_DISPLAY_DEFAULT))
		{
			return true;
		}
		else
		{
			printf("Failed to bootup the device\n");
		}
		EVE_Hal_close(phost);
	}
	else
	{
		printf("Failed to open HAL\n");
	}
	return false;
}

static void closeDevice(int i)
{
	EVE_HalContext *phost = &s_Host[i];

	EVE_Util_clearScreen(phost);
	EVE_Hal_flush(phost);
	EVE_sleep(100); /* TODO: Add EVE_Hal_waitSwap? */

	EVE_Util_shutdown(phost);
	EVE_Hal_close(phost);
}

int main()
{
	/* Initialize HAL */
	EVE_Hal_initialize();
	printf("EVE Hal Test Dual\n");

	printf("\n\nFirst device\n\n");

	while (!openDevice(0));
	EVE_Cmd_wr32(&s_Host[0], CMD_LOGO);
	EVE_Hal_flush(&s_Host[0]);

	printf("\n\nSecond device\n\n");

	while (!openDevice(1));
	EVE_Cmd_wr32(&s_Host[1], CMD_LOGO);
	EVE_Hal_flush(&s_Host[1]);

	EVE_Cmd_waitLogo(&s_Host[0]);
	EVE_Cmd_waitLogo(&s_Host[1]);

	printf("\n\nLogo finished displaying on both devices\n\n");

	while (s_Running
		&& s_Host[0].Status != EVE_STATUS_CLOSED
		&& s_Host[1].Status != EVE_STATUS_CLOSED)
	{
		if (!loop())
			s_Running = false;
	}
	if (s_Host[0].Status == EVE_STATUS_CLOSED)
	{
		printf("Device 0 closed unexpectedly\n");
	}
	if (s_Host[1].Status == EVE_STATUS_CLOSED)
	{
		printf("Device 1 closed unexpectedly\n");
	}

	closeDevice(0);
	closeDevice(1);

	EVE_Hal_release();
}

/* end of file */
