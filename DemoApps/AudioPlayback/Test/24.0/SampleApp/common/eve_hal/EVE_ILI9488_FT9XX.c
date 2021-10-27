
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
#if (defined(FT9XX_PLATFORM)) \
    && (defined(ENABLE_ILI9488_HVGA_PORTRAIT) || defined(ENABLE_KD2401_HVGA_PORTRAIT))
#define ILI9488_SEL 1

/**
 * @brief Write commandto SPI
 * 
 * @param cmd Command to write
 * @param data Data of the command
 * @return int16_t Always 0
 */
int16_t ILI9488_SPI_WriteCmd(uint8_t cmd, uint8_t data)
{
	uint8_t i;
	/* bit banging mechanism */
	ILI9488_DCX_LOW;

	ILI9488_CS_LOW;
	usleep(1);
	spi_open(SPIM, ILI9488_SEL);
	spi_writen(SPIM, &cmd, 1);
	ILI9488_DCX_HIGH;
	spi_writen(SPIM, &data, 1);
	ILI9488_CS_HIGH;
	ILI9488_DCX_LOW;
	spi_close(SPIM, ILI9488_SEL);

	return 0;
}

/**
 * @brief Write N number of bytes
 * 
 * @param cmd Command to write
 * @param bytecount Number of bytes in buffer
 * @param pbuff Data on the command
 * @return int16_t Always 0
 */
int16_t ILI9488_SPI_WriteCmdN(uint8_t cmd, uint8_t bytecount, const uint8_t *pbuff)
{
	uint8_t i, j, transbit;
	/* bit banging mechanism */
	ILI9488_DCX_LOW;

	ILI9488_CS_LOW;
	usleep(1);
	spi_open(SPIM, ILI9488_SEL);
	spi_writen(SPIM, &cmd, 1);
	ILI9488_DCX_HIGH;
	spi_writen(SPIM, pbuff, bytecount);
	spi_close(SPIM, ILI9488_SEL);
	ILI9488_CS_HIGH;
	ILI9488_DCX_LOW;

	return 0;
}

/**
 * @brief Read/write data with chip select pin being toggled
 * 
 * @param cmd Command to write
 * @return uint8_t Number of bytes read
 */
uint8_t ILI9488_SPI_Read(uint8_t cmd)
{
	uint8_t i, readbyte = 0;
	/* bit banging mechanism */
	ILI9488_DCX_LOW;
	ILI9488_CS_LOW;
	usleep(1);
	spi_open(SPIM, ILI9488_SEL);
	spi_writen(SPIM, &cmd, 1);
	ILI9488_DCX_HIGH;
	spi_readn(SPIM, &readbyte, 1);
	spi_close(SPIM, ILI9488_SEL);
	ILI9488_CS_HIGH;
	ILI9488_DCX_LOW;

	return readbyte;
}

/**
 * @brief Read/write N bytes with chip select pin being toggled
 * 
 * @param cmd Command to write
 * @param numbytes Number of bytes to read
 * @param pbuffer Buffer to get result
 * @return uint8_t Number of bytes read
 */
uint8_t ILI9488_SPI_ReadN(uint8_t cmd, uint8_t numbytes, const uint8_t *pbuffer)
{
	uint8_t i, readbyte = 0, j;
	/* bit banging mechanism */
	ILI9488_DCX_LOW;
	ILI9488_CS_LOW;
	usleep(1);
	spi_open(SPIM, ILI9488_SEL);
	spi_writen(SPIM, &cmd, 1);
	ILI9488_DCX_HIGH;
	spi_readn(SPIM, pbuffer, numbytes);
	spi_close(SPIM, ILI9488_SEL);
	ILI9488_CS_HIGH;
	ILI9488_DCX_LOW;

	return readbyte;
}

/**
 * @brief Read 24 bits
 * 
 * @param cmd Command to write
 * @return uint32_t Number of bytes read
 */
uint32_t ILI9488_SPI_ReadRDDID(uint8_t cmd)
{
	uint8_t i;
	uint32_t readword = 0;
	/* bit banging mechanism */
	ILI9488_DCX_LOW;
	ILI9488_CS_LOW;
	usleep(1);
	spi_open(SPIM, ILI9488_SEL);
	spi_writen(SPIM, &cmd, 1);
#if 0
	//extra clock cycle for dummy
	ILI9488_CLK_LOW;
	usleep(1);
	ILI9488_CLK_HIGH;
	usleep(1);
#endif
	ILI9488_DCX_HIGH;
	//NOTE: for little-endian, this is fine
	spi_readn(SPIM, &readword, 3);
	spi_close(SPIM, ILI9488_SEL);
	ILI9488_CS_HIGH;

	return readword;
}

/**
 * @brief Read 32 bits
 * 
 * @param cmd Command to write
 * @return uint32_t Number of bytes read
 */
uint32_t ILI9488_SPI_ReadRDDST(uint8_t cmd)
{
	uint8_t i;
	uint32_t readword = 0;
	/* bit banging mechanism */
	ILI9488_DCX_LOW;
	ILI9488_CS_LOW;
	usleep(1);
	spi_open(SPIM, ILI9488_SEL);
	spi_writen(SPIM, &cmd, 1);
#if 0
	//extra clock cycle for dummy
	ILI9488_CLK_LOW;
	usleep(1);
	ILI9488_CLK_HIGH;
	usleep(1);
#endif

	ILI9488_DCX_HIGH;
	spi_readn(SPIM, &readword, 4);
	spi_close(SPIM, ILI9488_SEL);
	ILI9488_CS_HIGH;
	ILI9488_DCX_LOW;
	return readword;
}

/**
 * @brief ILI9488 bootup
 * 
 */
void EVE_ILI9488_bootup()
{
#ifdef FT900_PLATFORM
#define pad_pwd          pad_gpio43
#elif defined(FT93X_PLATFORM)
#define pad_pwd          pad_gpio15
#endif
	/* ILI9488 driver - configure pins for bit bang */
	gpio_function(GPIO_SPIM_CLK, pad_spim_sck);
	gpio_function(GPIO_SPIM_SS0, pad_spim_ss0);
	gpio_function(GPIO_SPIM_MOSI, pad_spim_mosi);
	gpio_function(GPIO_SPIM_MISO, pad_spim_miso);
	gpio_function(GPIO_ILI9488_DCX, pad_ili9488_dcx);
	gpio_function(GPIO_ILI9488_CS1, pad_ili9488_cs1);

	gpio_function(GPIO_FT800_PWD, pad_pwd);

	gpio_dir(GPIO_ILI9488_DCX, pad_dir_output); //gpios for ili9488 - dcx
	gpio_dir(GPIO_ILI9488_CS1, pad_dir_output); //gpios for ili9488 - cs1#

	gpio_dir(GPIO_FT800_PWD, pad_dir_output); //gpios for ili9488 - pwd#

	gpio_write(GPIO_ILI9488_DCX, 1);
	gpio_write(GPIO_SPIM_SS0, 1);
	gpio_write(GPIO_ILI9488_CS1, 1);
	gpio_write(GPIO_FT800_PWD, 1);

	/* Enable the SPI Master device... */

	sys_enable(sys_device_spi_master);
	spi_init(SPIM, spi_dir_master, spi_mode_0, 64);
	spi_option(SPIM, spi_option_fifo_size, 16);
	spi_option(SPIM, spi_option_fifo, 1);
	spi_option(SPIM, spi_option_bus_width, 1);

	//display driver bring up
	{

		/* Temp buffer to construct the parameters */
		uint8_t arraytemp[16];

		ILI9488_SPI_WriteCmd(ILI9488_CMD_SOFTWARE_RESET, 0);
		usleep(120);

		//colomn address set - 0 to 319
		arraytemp[0] = 0x00;
		arraytemp[1] = 0x00;
		arraytemp[2] = 0x01;
		arraytemp[3] = 0x3f;
		ILI9488_SPI_WriteCmdN(ILI9488_CMD_COLOMNADDR, 4, arraytemp);

		//row address set - 0 to 479
		arraytemp[0] = 0x00;
		arraytemp[1] = 0x00;
		arraytemp[2] = 0x01;
		arraytemp[3] = 0xdf;
		ILI9488_SPI_WriteCmdN(ILI9488_CMD_ROWADDR, 4, arraytemp);

		//Frame rate 70HZ
		ILI9488_SPI_WriteCmd(ILI9488_CMD_FRAME_RATE_CONTROL, 0xB0);

		//adjust control 3
		arraytemp[0] = 0xa9;
		arraytemp[1] = 0x51;
		arraytemp[2] = 0x2c;
		arraytemp[3] = 0x82;
		ILI9488_SPI_WriteCmdN(0XF7, 4, arraytemp);

		ILI9488_SPI_WriteCmd(ILI9488_CMD_INTERFACE_MODE_CONTROL, 2);
		ILI9488_SPI_WriteCmd(ILI9488_CMD_INTERFACE_PIXEL_FORMAT, ILI9488_INTERFACE_PIXEL_FORMAT_18BIT_DPI);
		ILI9488_SPI_WriteCmd(ILI9488_CMD_IMAGEFUNCTION, 0);
		ILI9488_SPI_WriteCmd(ILI9488_CMD_WRITE_CONTROL_DISPLAY, 0x2c);

		ILI9488_SPI_WriteCmd(ILI9488_CMD_MADCTRL, 0x48); //bgr connection and colomn address order

		arraytemp[0] = 0x30;
		arraytemp[1] = 0x02;
		arraytemp[2] = 0x3b;

		ILI9488_SPI_WriteCmdN(ILI9488_CMD_DISPLAY_FUNCTION_CONTROL, 3, arraytemp);

		ILI9488_SPI_WriteCmd(ILI9488_CMD_SLEEP_OUT, 0);
		usleep(120);

		ILI9488_SPI_WriteCmd(ILI9488_CMD_DISPLAYON, 0);
		ILI9488_SPI_WriteCmd(0x2c, 0xB0);
	}
}

#endif

/* end of file */
