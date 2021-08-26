#ifndef IMAGE_H_
#define IMAGE_H_

typedef struct Img {
	uint32_t index;
	uint32_t addressFlash;
	uint32_t addressRamg;
	uint32_t size;
	uint32_t x;
	uint32_t y;
	uint32_t w;
	uint32_t h;
	uint16_t bitmapLayout; // see Table 13 – BITMAP_LAYOUT Format List 
	uint16_t extFormat; // See Table 12 – Bitmap formats and bits per pixel 
	uint8_t tag;
	uint8_t isFlash;
}Img_t;

int Image_Setup_Rotate(float angle, uint32_t rootx, uint32_t rooty);
int Image_Draw(EVE_HalContext *phost, Img_t *image);
int Image_Draw_From_RAM_G(EVE_HalContext* phost, uint32_t address, uint32_t x, uint32_t y, uint32_t w, uint32_t h,
	uint16_t bitmapLayout, uint32_t extFormat, uint8_t tag);
int Image_Draw_From_Flash(EVE_HalContext* phost, uint32_t address, uint32_t x, uint32_t y, uint32_t w, uint32_t h,
	uint16_t bitmapLayout, uint32_t extFormat, uint8_t tag);

int Image_Copy_To_RamG_And_Draw_Image(EVE_HalContext *phost, Img_t *image, uint32_t isRestart);
int Image_Copy_To_RamG(EVE_HalContext *phost, Img_t *image, uint32_t isRestart);

#endif /* IMAGE_H_ */
