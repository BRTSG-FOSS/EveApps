/**
* This source code ("the Software") is provided by Bridgetek Pte Ltd
* ("Bridgetek") subject to the licence terms set out
*   http://brtchip.com/BRTSourceCodeLicenseAgreement/ ("the Licence Terms").
* You must read the Licence Terms before downloading or using the Software.
* By installing or using the Software you agree to the Licence Terms. If you
* do not agree to the Licence Terms then do not download or use the Software.
*
* Without prejudice to the Licence Terms, here is a summary of some of the key
* terms of the Licence Terms (and in the event of any conflict between this
* summary and the Licence Terms then the text of the Licence Terms will
* prevail).
*
* The Software is provided "as is".
* There are no warranties (or similar) in relation to the quality of the
* Software. You use it at your own risk.
* The Software should not be used in, or for, any medical device, system or
* appliance. There are exclusions of Bridgetek liability for certain types of loss
* such as: special loss or damage; incidental loss or damage; indirect or
* consequential loss or damage; loss of income; loss of business; loss of
* profits; loss of revenue; loss of contracts; business interruption; loss of
* the use of money or anticipated savings; loss of information; loss of
* opportunity; loss of goodwill or reputation; and/or loss of, damage to or
* corruption of data.
* There is a monetary cap on Bridgetek's liability.
* The Software may have subsequently been amended by another user and then
* distributed by that other user ("Adapted Software").  If so that user may
* have additional licence terms that apply to those amendments. However, Bridgetek
* has no liability in relation to those amendments.
*
* Abstract: ILI9488 driver source
*
*/

#include "EVE_Platform.h"

#if defined(RP2040_PLATFORM) \
    && (defined(ENABLE_ILI9488_HVGA_PORTRAIT) || defined(ENABLE_KD2401_HVGA_PORTRAIT))

#define GPIO_ILI9488_CS 9
#define GPIO_ILI9488_DCX 8

int16_t ILI9488_SPI_WriteCmd(EVE_HalContext *phost, uint8_t cmd, uint8_t data)
{
	gpio_put(GPIO_ILI9488_CS, 0);
	gpio_put(GPIO_ILI9488_DCX, 0);

	uint8_t hcmd[1] = { 0 };
	hcmd[0] = cmd;

	spi_write_blocking(phost->SpiPort, hcmd, 1);

	uint8_t hdata[1] = { 0 };
	hdata[0] = data;

	gpio_put(GPIO_ILI9488_DCX, 1);

	spi_write_blocking(phost->SpiPort, hdata, 1);

	gpio_put(GPIO_ILI9488_CS, 1);

	return 0;
}

int16_t ILI9488_SPI_WriteCmdN(EVE_HalContext *phost, uint8_t cmd, uint8_t bytecount, const uint8_t *pbuff)
{

	gpio_put(GPIO_ILI9488_CS, 0);
	gpio_put(GPIO_ILI9488_DCX, 0);

	uint8_t hcmd[1] = { 0 };
	hcmd[0] = cmd;

	spi_write_blocking(phost->SpiPort, hcmd, 1);

	gpio_put(GPIO_ILI9488_DCX, 1);

	int byteWritten = spi_write_blocking(phost->SpiPort, pbuff, bytecount);

	gpio_put(GPIO_ILI9488_CS, 1);

	return byteWritten;
}

void EVE_ILI9488_RP2040_bootup(EVE_HalContext *phost)
{
	gpio_put(phost->SpiCsPin, 1);

	gpio_init(GPIO_ILI9488_DCX);
	gpio_set_dir(GPIO_ILI9488_DCX, GPIO_OUT); //gpios for ili9488 - dcx
	gpio_put(GPIO_ILI9488_DCX, 0);

	gpio_init(GPIO_ILI9488_CS);
	gpio_set_dir(GPIO_ILI9488_CS, GPIO_OUT); //gpios for ili9488 - cs
	gpio_put(GPIO_ILI9488_CS, 1);

	uint8_t arraytemp[16];

	ILI9488_SPI_WriteCmd(phost, ILI9488_CMD_SOFTWARE_RESET, 0);
	EVE_sleep(1);

	//colomn address set - 0 to 319
	arraytemp[0] = 0x00;
	arraytemp[1] = 0x00;
	arraytemp[2] = 0x01;
	arraytemp[3] = 0x3f;
	ILI9488_SPI_WriteCmdN(phost, ILI9488_CMD_COLOMNADDR, 4, arraytemp);

	//row address set - 0 to 479
	arraytemp[0] = 0x00;
	arraytemp[1] = 0x00;
	arraytemp[2] = 0x01;
	arraytemp[3] = 0xdf;
	ILI9488_SPI_WriteCmdN(phost, ILI9488_CMD_ROWADDR, 4, arraytemp);

	//Frame rate 70HZ
	ILI9488_SPI_WriteCmd(phost, ILI9488_CMD_FRAME_RATE_CONTROL, 0xB0);

	//adjust control 3
	arraytemp[0] = 0xa9;
	arraytemp[1] = 0x51;
	arraytemp[2] = 0x2c;
	arraytemp[3] = 0x82;
	ILI9488_SPI_WriteCmdN(phost, 0XF7, 4, arraytemp);

	ILI9488_SPI_WriteCmd(phost, ILI9488_CMD_INTERFACE_MODE_CONTROL, 2);
	ILI9488_SPI_WriteCmd(phost, ILI9488_CMD_INTERFACE_PIXEL_FORMAT, ILI9488_INTERFACE_PIXEL_FORMAT_18BIT_DPI);
	ILI9488_SPI_WriteCmd(phost, ILI9488_CMD_IMAGEFUNCTION, 0);
	ILI9488_SPI_WriteCmd(phost, ILI9488_CMD_WRITE_CONTROL_DISPLAY, 0x2c);

	ILI9488_SPI_WriteCmd(phost, ILI9488_CMD_MADCTRL, 0x48); //bgr connection and colomn address order

	arraytemp[0] = 0x30;
	arraytemp[1] = 0x02;
	arraytemp[2] = 0x3b;

	ILI9488_SPI_WriteCmdN(phost, ILI9488_CMD_DISPLAY_FUNCTION_CONTROL, 3, arraytemp);

	ILI9488_SPI_WriteCmd(phost, ILI9488_CMD_SLEEP_OUT, 0);
	EVE_sleep(1);

	ILI9488_SPI_WriteCmd(phost, ILI9488_CMD_DISPLAYON, 0);
	ILI9488_SPI_WriteCmd(phost, 0x2c, 0xB0);
}

#endif

/*end of file*/