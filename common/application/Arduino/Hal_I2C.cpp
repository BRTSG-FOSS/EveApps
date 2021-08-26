/*

Copyright (c) Bridgetek Pte Ltd

THIS SOFTWARE IS PROVIDED BY BRIDGETEK PTE LTD "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
BRIDGETEK PTE LTD BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
OF SUBSTITUTE GOODS OR SERVICES LOSS OF USE, DATA, OR PROFITS OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

BRIDGETEK DRIVERS MAY BE USED ONLY IN CONJUNCTION WITH PRODUCTS BASED ON BRIDGETEK PARTS.

BRIDGETEK DRIVERS MAY BE DISTRIBUTED IN ANY FORM AS LONG AS LICENSE INFORMATION IS NOT MODIFIED.

IF A CUSTOM VENDOR ID AND/OR PRODUCT ID OR DESCRIPTION STRING ARE USED, IT IS THE
RESPONSIBILITY OF THE PRODUCT MANUFACTURER TO MAINTAIN ANY CHANGES AND SUBSEQUENT WHQL
RE-CERTIFICATION AS A RESULT OF MAKING THESE CHANGES.

Author : BRIDGETEK 

Revision History: 
0.1 - date 2013.04.24 - Initial version
0.2 - date 2013.08.19 - few minor edits

*/

/* This file contains apis related to i2c */

/* Standard includes */
#include "Platform.h"
#include <Wire.h>
#include "Hal_I2C.h"

/* mainly assosiated to rtc - need to make it generic */

int16_t hal_rtc_i2c_init()
{
  Wire.begin();
  delay(100);  
  return 0;
}

/* api to read n bytes from addr */
int16_t hal_rtc_i2c_read(uint8_t addr, uint8_t *buffer,uint16_t length)
{
  uint16_t i;
  short count = 0;
  uint8_t writeResult = 0;
   while (length > 28)
  {
    hal_rtc_i2c_read(addr,buffer,28);
    buffer += 28;
    addr += 28;
    length -= 28;
  }
  
  Wire.beginTransmission(0x6f); // transmit to device (0x23)
  
 /* address bytes for rtc are from 00 to 0xff */
  Wire.write(addr);        // sends value byte  
  
  /* end the transmission but do not release the bus - usage is random data read use case from rtc */
  writeResult = Wire.endTransmission(false);//hold the bus to read the next data

  if (0 != writeResult)
  {
    return -1;//error case
  }

  Wire.requestFrom(0x6f, length);// request length bytes from slave device and end the transmission after this
  for(i=0;i<length;i++)
  {
    /* need to consider timout here */
    while(0 == Wire.available());//blocking call - at least one byte must be available
    buffer[i] = Wire.read();
  }
  
  return 0;
}

/* API to write data into particular location */
int16_t hal_rtc_i2c_write(uint8_t addr, uint8_t *buffer,uint16_t length)
{
  uint16_t i;
  byte writeResult = 0;
  if(0 == length)
  {
    return -1;
  }
  
  /* for read the lower bit must be set to 1 and for write set to 0 */
  Wire.beginTransmission(0x6f);

  /* address bytes for rtc are from 00 to 0xff */
  Wire.write(addr);        // sends value byte  
  
  /* check for each byte */
  for(i=0;i<length;i++)
  {
    Wire.write(*buffer++);//send the data to slave
  }

  /* end the transmission by stop bit */
  
  writeResult = Wire.endTransmission();//end the transmission by setting the stop bit
  
  if(0 != writeResult)
  {
    return writeResult;
  }
  return 0;
}

/* Nothing beyond this */

