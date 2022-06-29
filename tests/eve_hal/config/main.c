
#include <stdio.h>
#include <EVE_Hal.h>

static EVE_HalContext s_Host;
static bool s_Running = true;

static uint64_t s_Millis = 0;
static uint8_t s_LastTag = 0;
static uint8_t s_HoldTag = 0;

void setRefreshRate(EVE_HalContext *phost, uint32_t refreshRate)
{
	EVE_ConfigParameters config;
	const uint32_t width = EVE_Hal_rd16(phost, REG_HSIZE);
	const uint32_t height = EVE_Hal_rd16(phost, REG_VSIZE);
#ifdef EVE_SUPPORT_HSF
	const uint32_t hsfWidth = EVE_CHIPID >= EVE_BT817 ? EVE_Hal_rd16(phost, REG_HSF_HSIZE) : 0;
#else
	const uint32_t hsfWidth = 0;
#endif
	EVE_Util_configDefaultsEx(phost, &config, width, height, refreshRate, hsfWidth);
	EVE_Util_config(phost, &config);
}

bool loop(EVE_HalContext *phost)
{
	uint8_t tag;
	uint32_t frames = EVE_Hal_rd32(phost, REG_FRAMES);
	bool noDrag = (s_LastTag == s_HoldTag);
	s_Millis = EVE_millis64();

	EVE_CoCmd_dlStart(phost);

	EVE_CoCmd_dl(phost, CLEAR_TAG(255));
	EVE_CoCmd_dl(phost, CLEAR_COLOR_RGB(0, 0, 0));
	EVE_CoCmd_dl(phost, CLEAR(1, 1, 1));

	EVE_CoCmd_clock(phost, phost->Width >> 1, phost->Height >> 1,
		(min(phost->Height, phost->Width) >> 1) - 32, 0, /* FIXME: OPT_3D is in the documentation as value 0 */
		(s_Millis / 1000 / 60 / 60) % 24, (s_Millis / 1000 / 60) % 60,
		(s_Millis / 1000) % 60, s_Millis % 1000);

	EVE_CoCmd_progress(phost, 15, phost->Height - 20, phost->Width - 30, 10, 0, frames & 0xFF, 0xFF);

	EVE_CoCmd_dl(phost, TAG(30));
	EVE_CoCmd_button(phost, 10, 10, 120, 30, 27, (noDrag && s_HoldTag == 30) ? OPT_FLAT : 0, "30 Hz");

	EVE_CoCmd_dl(phost, TAG(50));
	EVE_CoCmd_button(phost, 10, 50, 120, 30, 27, (noDrag && s_HoldTag == 50) ? OPT_FLAT : 0, "50 Hz");

	EVE_CoCmd_dl(phost, TAG(60));
	EVE_CoCmd_button(phost, 10, 90, 120, 30, 27, (noDrag && s_HoldTag == 60) ? OPT_FLAT : 0, "60 Hz");

	EVE_CoCmd_dl(phost, TAG(75));
	EVE_CoCmd_button(phost, 10, 130, 120, 30, 27, (noDrag && s_HoldTag == 75) ? OPT_FLAT : 0, "75 Hz");

	EVE_CoCmd_dl(phost, TAG(90));
	EVE_CoCmd_button(phost, 10, 170, 120, 30, 27, (noDrag && s_HoldTag == 90) ? OPT_FLAT : 0, "90 Hz");

	EVE_CoCmd_dl(phost, TAG(120));
	EVE_CoCmd_button(phost, 10, 210, 120, 30, 27, (noDrag && s_HoldTag == 120) ? OPT_FLAT : 0, "120 Hz");

	EVE_CoCmd_dl(phost, DISPLAY());
	EVE_CoCmd_swap(phost);
	EVE_Cmd_waitFlush(phost);

	if (phost->CmdFault)
		EVE_Util_resetCoprocessor(phost);

	tag = EVE_Hal_rd8(phost, REG_TOUCH_TAG);
	if (tag != s_LastTag)
	{
		eve_printf_debug("Tag changed from %d to %d\n", (unsigned int)s_LastTag, (unsigned int)tag);

		if (tag == 0 && s_LastTag == s_HoldTag)
		{
			/* tag up */
			eve_printf_debug("Button up\n");
			switch (s_LastTag)
			{
			case 30:
			case 50:
			case 60:
			case 75:
			case 90:
			case 120:
				setRefreshRate(phost, s_LastTag);
				break;
			}
			s_LastTag = 0;
			s_HoldTag = 0;
		}
		else if (tag == 0)
		{
			/* drag drop */
			s_LastTag = 0;
			s_HoldTag = 0;
		}
		else if (s_LastTag == 0)
		{
			/* tag down */
			s_LastTag = tag;
			s_HoldTag = tag;
		}
		else if (s_LastTag == s_HoldTag)
		{
			/* start drag */
			s_LastTag = tag;
		}
		else
		{
			/* drag continue */
			s_LastTag = tag;
		}

		s_LastTag = tag;
	}

	return true;
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
			/* Rotate 90 degrees */
			if (EVE_CHIPID >= EVE_FT810)
				EVE_CoCmd_setRotate(phost, 0x2);

			/* Calibrate */
			EVE_CoCmd_dlStart(phost);
			EVE_CoCmd_dl(phost, CLEAR_COLOR_RGB(0, 0, 0));
			EVE_CoCmd_dl(phost, CLEAR(1, 1, 1));
			if (!EVE_CoCmd_calibrate(phost))
				printf("Calibration failed\n");

			/* Run */
			while (s_Running && phost->Status != EVE_STATUS_CLOSED) // TODO: Deal with emulator closing (EVE_STATUS_CLOSED/EVE_STATUS_ERROR?)
			{
				if (!loop(phost))
					s_Running = false;
			}
			if (phost->Status == EVE_STATUS_CLOSED)
			{
				printf("Device closed unexpectedly\n");
			}
			EVE_Util_shutdown(phost);
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
