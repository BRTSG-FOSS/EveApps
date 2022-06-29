
#include <stdio.h>
#include <EVE_Hal.h>

static EVE_HalContext s_Host;
static bool s_Running = true;

bool loop(EVE_HalContext *phost)
{
	// EVE_Hal_displayMessageZ(phost, "EVE Hal Test");
	EVE_Cmd_wr32(phost, CMD_LOGO);
	EVE_Cmd_waitLogo(phost);

	printf("Logo finished, bye!");
	return false;
}

int main()
{
	EVE_HalContext *phost = &s_Host;

	/* Initialize HAL */
	EVE_Hal_initialize();

	/* Open the device interactively */
	// if (EVE_Util_openDeviceInteractive(phost, L"Flash.bin"))
	if (EVE_Util_openDeviceInteractive(phost, NULL))
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
	else
	{
		printf("Failed to open HAL\n");
	}
	EVE_Hal_release();
}

/* end of file */
