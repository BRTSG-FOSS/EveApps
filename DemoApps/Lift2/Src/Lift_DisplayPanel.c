/*****************************************************************************
* Copyright (c) Bridgetek Pte Ltd.
* Software License Agreement
*
* This code is provided as an example only and is not guaranteed by Bridgetek.
* BRIDGETEK accept no responsibility for any issues resulting from its use.
* The developer of the final application incorporating any parts of this
* sample project is responsible for ensuring its safe and correct operation
* and for any consequences resulting from its use.
*****************************************************************************/
/**
* @file  Lift_DisplayPanel.c
* @brief Lift application to demonstrate EVE primitives and widgets.
*/
#include <Windows.h>
#include "Platform.h"
#include "DemoLift2.h"
#include "Lift_Control.h"
#include "Lift_DisplayPanel.h"

#if defined(EVE_SUPPORT_UNICODE)

#define TEXT_MOVE_COUNT		1
#define START_X_UNICODE_STRING	183

enum {
	CENTER_X_ALIGN,
	CENTER_Y_ALIGN,
	CENTER_ALIGN,
	TOP_LEFT_ALIGN,
};

enum
{
	DISP_FONT_INDEX=0,
	DISP_UP_INDEX,
	DISP_DOWN_INDEX,
	DISP_FLOOR_INDEX,
	DISP_AVI1_INDEX,
};
#define DISP_UP_HANDLE			1
#define DISP_DOWN_HANDLE		2
#define DISP_FLOOR_HANDLE		3
#define DISP_FONT_HANDLE		9

LiftBitmap_t DisplayPanelAssets[] = {
	//layoutStride	//layoutHeight	//dispWidth		//dispHeight	//swizzle		//format		//handle		//addr		//maxnum
	//dispW, dispH = fontW, fontH-2
	{ 16*40/10, 40/8,	40,		40,			585,	(COMPRESSED_RGBA_ASTC_10x8_KHR<<8 | GLFORMAT),	1, FLASHSTARTADD + DISP_FONT_ADDR/32,1 },//font in flash
	{16*(113/4),	113/4,	110,	110,		585,	(COMPRESSED_RGBA_ASTC_4x4_KHR<<8 | GLFORMAT),  	1,	FLASHSTARTADD + DISP_UP_ADDR/32,	1},		//new Up
	{16*(113/4),	113/4,	110,	110,		585,	(COMPRESSED_RGBA_ASTC_4x4_KHR<<8 | GLFORMAT),  	1,	FLASHSTARTADD + DISP_DOWN_ADDR/32,	1},		//new Down
	{16*(203/4),	1203/4,	200,  	1200,		585,	(COMPRESSED_RGBA_ASTC_4x4_KHR<<8 | GLFORMAT),  	1,	FLASHSTARTADD + DISP_FLOOR_ADDR/32,	1},		//floors
	{800*2,		480,	800,	450,		585,	RGB565,  										0,	DISP_AVI1_ADDR,						1},		//avi
};

extern AppxFont_t sappxfont;
 wchar_t Unicode_String2[] = L"Welcome to Bridgetek innovative EVE Solution with BT815";
 wchar_t Unicode_String1[] = {27456,36814,26469,21040,66,114,105,100,103,101,116,101,107,21019,26032,30340,69,86,69,35299,20915,26041,26696,66,84,56,49,53,0};
 wchar_t Unicode_String3[] = {87,105,108,108,107,111,109,109,101,110,32,98,101,105,32,100,101,114,32,105,110,110,111,118,97,116,105,118,101,110,32,69,86,69,45,76,246,115,117,110,103,32,118,111,110,32,66,114,105,100,103,101,116,101,107,32,109,105,116,32,66,84,56,49,53,0};
wchar_t *Unicode_List[] = { Unicode_String1, Unicode_String2, Unicode_String3 };
uint8_t Active_String = 0;
static Gpu_Hal_Context_t *s_pHalContext;

uint8_t Current_Second;
wchar_t Current_Time[10] = L"10:30";

uint32_t Start_Milis;

//Setup current time/date
uint8_t Current_Hour = 9;
uint8_t Current_Minute = 48;
uint8_t Current_Day = 13;
uint8_t Current_Month = 2;
wchar_t Current_Date[20] = L"2018-05-18";
SYSTEMTIME	Time;

//Specify color: Alpha(31~24 bit) + RGB(23~0 bit)
static void Color_ARGB(EVE_HalContext *pHalContext, uint32_t c)
{
	uint32_t rgb = c & 0xFFFFFF;
	App_WrDl_Buffer(s_pHalContext, (4UL << 24) | (rgb));
	App_WrDl_Buffer(s_pHalContext, COLOR_A(c >> 24));
}

void App_DrawRectangleF(EVE_HalContext *pHalContext, int32_t x, int32_t y, int32_t w, int32_t h, int32_t radius, uint32_t color)
{
	int32_t width = (radius + 8) << 4;
	int32_t x0 = (x + radius) << 2;
	int32_t y0 = (y + radius) << 2;
	int32_t x1 = (x + w - 16 - radius) << 2;
	int32_t y1 = (y + h - 16 - radius) << 2;

	App_WrDl_Buffer(s_pHalContext, SAVE_CONTEXT());
	Color_ARGB(s_pHalContext, color);
	App_WrDl_Buffer(s_pHalContext, LINE_WIDTH(16));
	App_WrDl_Buffer(s_pHalContext, BEGIN(RECTS));
#ifdef FT81X_ENABLE
	App_WrDl_Buffer(s_pHalContext, VERTEX_FORMAT(2));
#endif
	App_WrDl_Buffer(s_pHalContext, VERTEX2F(x0, y0));
	App_WrDl_Buffer(s_pHalContext, VERTEX2F(x1, y1));
#if FT_ESD_DL_END
	App_WrDl_Buffer(s_pHalContext, END());
#endif
	App_WrDl_Buffer(s_pHalContext, RESTORE_CONTEXT());
}

void App_DrawBitmap(EVE_HalContext *pHalContext, int32_t handle, int32_t x, int32_t y, int32_t w, int32_t h, uint8_t align)
{
	App_WrDl_Buffer(s_pHalContext, BEGIN(BITMAPS));
	App_WrDl_Buffer(s_pHalContext, BITMAP_HANDLE(handle));
	if (align == CENTER_ALIGN)
	{
		App_WrDl_Buffer(s_pHalContext, VERTEX2F((x - w / 2) * 16, (y - h / 2) * 16));
	}
	else if (align == TOP_LEFT_ALIGN)
	{
		App_WrDl_Buffer(s_pHalContext, VERTEX2F(x * 16, y * 16));
	}
	App_WrDl_Buffer(s_pHalContext, END());
}

void Lift_DispPanel_Init(Gpu_Hal_Context_t *pHalContext)
{
	s_pHalContext = pHalContext;
}

void Lift_DisplayPanel()
{
	static int x_loc = START_X_UNICODE_STRING;
	static uint8_t Text_Run_Count = 0;
	uint8_t floor_index;
	SYSTEMTIME time;
	char current_time[10];
	char current_date[20];
	
	GetLocalTime(&time);
	Current_Hour = time.wHour;
	Current_Minute = time.wMinute;
	Current_Day = time.wDay;
	Current_Month = time.wMonth;

	sprintf(current_date, "%02d", time.wDay);
	current_date[2] = '-';
	sprintf(current_date + 3, "%02d", time.wMonth);
	strcpy(current_date + 5, "-2018");
	mbstowcs(Current_Date, current_date, strlen(current_date));
	Current_Date[10] = 0;

	sprintf(current_time, "%02d", Current_Hour);
	current_time[2] = ':';
	sprintf(current_time + 3, "%02d", Current_Minute);
	if (Current_Hour > 12)
	{
		strcpy(current_time + 5, " PM");
	}
	else
	{
		strcpy(current_time + 5, " AM");
	}
	mbstowcs(Current_Time, current_time, strlen(current_time));

	do
	{
		//Update location of tex string to make moving effect
		Text_Run_Count = (Text_Run_Count + 1) % TEXT_MOVE_COUNT;
		if(Text_Run_Count == 0)
		{
			if(x_loc>-1200)
			{
				x_loc -= 3;
			}
			else
			{
				x_loc= 795;
				Active_String = (Active_String + 1) % 3;
			}
		}

		App_WrDl_Buffer(s_pHalContext, BEGIN(BITMAPS));
		App_WrDl_Buffer(s_pHalContext, CLEAR_COLOR_RGB(0, 0, 0));
		App_WrDl_Buffer(s_pHalContext, CLEAR(1, 1, 1)); // clear screen
		App_WrDl_Buffer(s_pHalContext, COLOR_RGB(255, 255, 255));

		uint16_t read = EVE_Hal_rd16(s_pHalContext, REG_CMD_READ);
		uint16_t write = EVE_Hal_rd16(s_pHalContext, REG_CMD_WRITE);
		if(EVE_Hal_rd16(s_pHalContext,REG_CMD_READ) == EVE_Hal_rd16(s_pHalContext,REG_CMD_WRITE))
		{
			DemoLift2_InitialVideoSetupFlash(s_pHalContext, &DisplayPanelAssets[DISP_AVI1_INDEX]);

		}

		uint32_t videoWidth = 800;
		uint32_t videoHeight = 450;
		uint32_t Original_videoHeight = 480;
		//display video frame decoded by CoProcessor
		App_WrDl_Buffer(s_pHalContext, BITMAP_HANDLE(14));
		App_WrDl_Buffer(s_pHalContext, BITMAP_SOURCE2(0, 0));
		App_WrDl_Buffer(s_pHalContext, BITMAP_LAYOUT(RGB565, videoWidth * 2L, Original_videoHeight));
		App_WrDl_Buffer(s_pHalContext, BITMAP_LAYOUT_H(((videoWidth * 2L) >> 10), ((Original_videoHeight) >> 9)));
		App_WrDl_Buffer(s_pHalContext, BITMAP_SIZE(NEAREST, BORDER, BORDER, videoWidth, videoHeight));
		App_WrDl_Buffer(s_pHalContext, BITMAP_SIZE_H((videoWidth >> 9), (videoHeight >> 9)));
		App_WrDl_Buffer(s_pHalContext, BEGIN(BITMAPS));
		App_WrDl_Buffer(s_pHalContext, CELL(0));
		App_WrDl_Buffer(s_pHalContext, VERTEX2F(0 * 16, 20 * 16));
		App_WrDl_Buffer(s_pHalContext, END());

		AppLift2_LoadAssetsFromFlash(s_pHalContext, &DisplayPanelAssets[DISP_FONT_INDEX], DISP_FONT_HANDLE);
		AppLift2_LoadAssetsFromFlash(s_pHalContext, &DisplayPanelAssets[DISP_FLOOR_INDEX], DISP_FLOOR_HANDLE);
		AppLift2_LoadAssetsFromFlash(s_pHalContext, &DisplayPanelAssets[DISP_UP_INDEX], DISP_UP_HANDLE);
		AppLift2_LoadAssetsFromFlash(s_pHalContext, &DisplayPanelAssets[DISP_DOWN_INDEX], DISP_DOWN_HANDLE);

		App_DrawRectangleF(s_pHalContext, 0,10,208,480,0,0xc0101010);
		App_DrawRectangleF(s_pHalContext, 193,432,638,68,0,0xc0101010);

		//display time
		App_WrDl_Buffer(s_pHalContext, COLOR_RGB(63, 155, 255));		
		App_WrDl_Buffer(s_pHalContext, BEGIN(BITMAPS));
		App_WrDl_Buffer(s_pHalContext, BITMAP_HANDLE(DISP_FONT_HANDLE));
		App_WrDl_Buffer(s_pHalContext, ((47UL<<24)|sappxfont.pfontext->Swizzle));		
		AppLift2_FontString(s_pHalContext,Current_Time, &sappxfont, 0, 10, 390);
		AppLift2_FontString(s_pHalContext,Current_Date, &sappxfont, 0, 2, 434);
		App_WrDl_Buffer(s_pHalContext, COLOR_RGB(255, 255, 255));		

		//display unicode string
		App_WrDl_Buffer(s_pHalContext, SAVE_CONTEXT());
		App_WrDl_Buffer(s_pHalContext, SCISSOR_XY(220,420));
		App_WrDl_Buffer(s_pHalContext, SCISSOR_SIZE(560,60));
		App_WrDl_Buffer(s_pHalContext, BEGIN(BITMAPS));
		App_WrDl_Buffer(s_pHalContext, BITMAP_HANDLE(DISP_FONT_HANDLE));
		App_WrDl_Buffer(s_pHalContext, ((47UL<<24)|sappxfont.pfontext->Swizzle));	
		AppLift2_FontString(s_pHalContext, Unicode_List[Active_String], &sappxfont, 0, x_loc, 435);
		App_WrDl_Buffer(s_pHalContext, RESTORE_CONTEXT());

		//draw Floor number
		App_WrDl_Buffer(s_pHalContext, SAVE_CONTEXT());
		App_WrDl_Buffer(s_pHalContext, SCISSOR_XY(20,138));
		App_WrDl_Buffer(s_pHalContext, SCISSOR_SIZE(200,200));
		int current_floor = LiftControl_GetCurrentFloor();
		App_DrawBitmap(s_pHalContext,DISP_FLOOR_HANDLE,10,138 - current_floor * 200, DisplayPanelAssets[DISP_FLOOR_INDEX].displaywidth, DisplayPanelAssets[DISP_FLOOR_INDEX].displayheight, TOP_LEFT_ALIGN);
		App_WrDl_Buffer(s_pHalContext, RESTORE_CONTEXT());

		//draw up/down arrow
		uint8_t lift_state =  LiftControl_GetCurrentState();
		if(lift_state == LIFT_MOVE_UP_STATE)
		{
			App_DrawBitmap(s_pHalContext,DISP_UP_HANDLE, 40, 26, DisplayPanelAssets[DISP_UP_INDEX].displaywidth, DisplayPanelAssets[DISP_UP_INDEX].displayheight, TOP_LEFT_ALIGN);

		}
		else if(lift_state == LIFT_MOVE_DOWN_STATE)
		{
			App_DrawBitmap(s_pHalContext,DISP_DOWN_HANDLE, 40, 26, DisplayPanelAssets[DISP_DOWN_INDEX].displaywidth, DisplayPanelAssets[DISP_DOWN_INDEX].displayheight, TOP_LEFT_ALIGN);
		}

		App_WrDl_Buffer(s_pHalContext, DISPLAY());
		App_Flush_DL_Buffer(s_pHalContext);
		GPU_DLSwap(s_pHalContext, DLSWAP_FRAME);

	}while(0);
}
#endif /* EVE_SUPPORT_UNICODE */
