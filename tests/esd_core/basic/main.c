
#include <stdio.h>
#include <Esd_Core.h>

static Esd_Context s_Host;

void Main_Update(void *context)
{

}

void Main_Render(void *context)
{
	EVE_HalContext *phost = Esd_GetHost();
	(void)phost;
	
	Esd_Render_Line(10, 10, 200, 100, 5, ESD_COMPOSE_ARGB8888(255, 127, 0, 255));
}

int main(int argc, char *argv[])
{
	Esd_Initialize();

	{
		Esd_Parameters ep;
		Esd_Defaults(&ep);
		ep.Update = Main_Update;
		ep.Render = Main_Render;
#ifdef ESD_FLASH_FILES
		for (int i = 0; i < ESD_FLASH_NB; ++i)
			ep.FlashFilePaths[i][0] = '\0'; /* Disable flash */
#endif

		if (!Esd_Open(&s_Host, &ep))
			return EXIT_FAILURE;
	}

	Esd_Loop(&s_Host);
	Esd_Close(&s_Host);

	Esd_Release();
	return EXIT_SUCCESS;
}

/* end of file */
