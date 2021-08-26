#ifndef LIFT_DISPLAYPANEL_H
#define LIFT_DISPLAYPANEL_H

#define XFONTGLYPHDATAOFFSET	(10*4)
#define BITMAP_MAX_CELLS		(128)
#define BITMAP_GPUBLKRESOLN		(32)

#define FLASHSTARTADD	(8*1024*1024)		//starting address of the flash
#define DISP_FONT_ADDR	(4096UL)
#define DISP_UP_ADDR	(8161088UL)
#define DISP_DOWN_ADDR	(8173632UL)
#define DISP_FLOOR_ADDR	(7921088UL)
#define DISP_AVI1_ADDR	(8186176UL)		//BB_16.avi

#define DISP_FONTHEADER_ADDR	(524288UL)		//in RAM_G

#define ASSETS_START_ADDR (850*1024)	//location where all assets are placed at the end of the ram

void Lift_DisplayPanel();
void Lift_DispPanel_Init(Gpu_Hal_Context_t *host);

#endif	//LIFT_DISPLAYPANEL_H
