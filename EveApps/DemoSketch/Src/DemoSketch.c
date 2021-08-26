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
  * Description: Demo application for BT815A platform
  */

#include "Platform.h"
#include "EVE_CoCmd.h"
#include "Common.h"
#include "App.h"

static EVE_HalContext *s_pHalContext;

  /* Application*/
void DemoSketch(EVE_HalContext* pHalContext) {
	s_pHalContext = pHalContext;
	s_pHalContext = pHalContext;
	uint32_t  tracker, color = 0;
	uint16_t  val = 32768;
	uint8_t tag = 0;
	//  Set the bitmap properties , sketch properties and Tracker for the sliders
	EVE_CoCmd_dlStart(s_pHalContext);
	EVE_CoCmd_fgColor(s_pHalContext, 0xffffff);        // Set the bg color
	EVE_CoCmd_track(s_pHalContext, (s_pHalContext->Width - 30), 40, 8, s_pHalContext->Height - 100, 1);

#if defined FT801_ENABLE
	EVE_CoCmd_cSketch(s_pHalContext, 0, 10, s_pHalContext->Width - 40, s_pHalContext->Height - 30, 0, L8, 1500L);
#elif defined FT81X_ENABLE
	EVE_CoCmd_sketch(s_pHalContext, 0, 10, s_pHalContext->Width - 40, s_pHalContext->Height - 30, 0, L8);
#else
	EVE_CoCmd_sketch(s_pHalContext, 0, 10, s_pHalContext->Width - 40, s_pHalContext->Height - 30, 0, L8);
#endif
	/*
	#if defined FT801_ENABLE
	EVE_CoCmd_cSketch(s_pHalContext,0,10,s_pHalContext->Width-40,s_pHalContext->Height-20,0,L8,1500L);
	#elif defined FT81X_ENABLE
	EVE_CoCmd_sketch(s_pHalContext,0,10,s_pHalContext->Width-40,s_pHalContext->Height-20,0,L8);
	#else
	EVE_CoCmd_sketch(s_pHalContext,0,10,s_pHalContext->Width-40,s_pHalContext->Height-20,0,L8);
	#endif
	*/
	EVE_CoCmd_memZero(s_pHalContext, 0L, (s_pHalContext->Width - 40)*(s_pHalContext->Height - 20L));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_HANDLE(1));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_SOURCE(0));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_LAYOUT(L8, s_pHalContext->Width - 40, s_pHalContext->Height - 20));
#ifdef FT81X_ENABLE
	EVE_Cmd_wr32(s_pHalContext, BITMAP_LAYOUT_H((s_pHalContext->Width - 40) >> 10, (s_pHalContext->Height - 20) >> 9));
#endif
	EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE(NEAREST, BORDER, BORDER, (s_pHalContext->Width - 40), (s_pHalContext->Height - 20)));
#ifdef FT81X_ENABLE
	EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE_H((s_pHalContext->Width - 40) >> 9, (s_pHalContext->Height - 20) >> 9));
#endif
	EVE_CoCmd_swap(s_pHalContext);
	EVE_Cmd_waitFlush(s_pHalContext);
	while (1)
	{
		// Check the tracker
		tracker = EVE_Hal_rd32(s_pHalContext, REG_TRACKER);
		// Check the Tag 
		tag = EVE_Hal_rd8(s_pHalContext, REG_TOUCH_TAG);
		//  clear the GRAM when user enter the Clear button
		if (tag == 2)
		{
			EVE_CoCmd_dlStart(s_pHalContext);
			EVE_CoCmd_memZero(s_pHalContext, 0, (s_pHalContext->Width - 40)*(s_pHalContext->Height - 20L)); // Clear the gram frm 1024 		
			EVE_Cmd_waitFlush(s_pHalContext);
		}
		// compute the color from the tracker
		if ((tracker & 0xff) == 1)      // check the tag val
		{
			val = (tracker >> 16);
		}
		color = val * 255L;
		// Start the new display list
		EVE_CoCmd_dlStart(s_pHalContext);                  // Start the display list
		EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));	  // clear the display     
		EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));  // color	
		EVE_CoCmd_bgColor(s_pHalContext, color);
		EVE_Cmd_wr32(s_pHalContext, TAG_MASK(1));
		EVE_Cmd_wr32(s_pHalContext, TAG(1));          // assign the tag value 
		EVE_CoCmd_fgColor(s_pHalContext, color);
		// draw the sliders 
		EVE_CoCmd_slider(s_pHalContext, (s_pHalContext->Width - 30), 40, 8, (s_pHalContext->Height - 100), 0, val, 65535);	 // slide j1 cmd  
		EVE_CoCmd_fgColor(s_pHalContext, (tag == 2) ? 0x0000ff : color);
		EVE_Cmd_wr32(s_pHalContext, TAG(2));          // assign the tag value 
		EVE_CoCmd_button(s_pHalContext, (s_pHalContext->Width - 35), (s_pHalContext->Height - 45), 35, 25, 26, 0, "CLR");
		EVE_Cmd_wr32(s_pHalContext, TAG_MASK(0));

		EVE_CoCmd_text(s_pHalContext, s_pHalContext->Width - 35, 10, 26, 0, "Color");

		EVE_Cmd_wr32(s_pHalContext, LINE_WIDTH(1 * 16));
		EVE_Cmd_wr32(s_pHalContext, BEGIN(RECTS));
		EVE_Cmd_wr32(s_pHalContext, VERTEX2F(0, 10 * 16));
		EVE_Cmd_wr32(s_pHalContext, VERTEX2F((int16_t)(s_pHalContext->Width - 40) * 16, (int16_t)(s_pHalContext->Height - 20) * 16));

		EVE_Cmd_wr32(s_pHalContext, COLOR_RGB((color >> 16) & 0xff, (color >> 8) & 0xff, (color) & 0xff));
		EVE_Cmd_wr32(s_pHalContext, BEGIN(BITMAPS));
		EVE_Cmd_wr32(s_pHalContext, VERTEX2II(0, 10, 1, 0));
		EVE_Cmd_wr32(s_pHalContext, END());
		EVE_Cmd_wr32(s_pHalContext, DISPLAY());
		EVE_CoCmd_swap(s_pHalContext);
		EVE_Cmd_waitFlush(s_pHalContext);
	}
}
