

#ifndef FT_AUDIO_H_
#define FT_AUDIO_H_
#include "FT_Platform.h"

#if defined(FT900_PLATFORM)
#include <ft900.h>

#define WM8731_WR_ADDR (0x34)
#define WM8731_RD_ADDR (0x35)
#define I2S_TXRX_FIFO_SPACE (2048)
#define I2S_TXRX_FIFO_SPACE_HALF (I2S_TXRX_FIFO_SPACE/2)
#define ZERO_ARRAY_SIZE (1024)
//#define ZERO_DECIBEL_VOLUME (121)
#define ZERO_DECIBEL_VOLUME (249)
#define MAX_VOLUME (6)
#define DIGITAL_AUDIO_FORMAT (0x42)
#define AUDIO_SAMPLING (0x01)

#define BITS 16

#define FT_AUDIO_DEBUG_ON
//#define FT_AUDIO_TEST

#define FT_AUDIO_DAIF_DATA_LENGTH_MASK (0xF3)
#define FT_AUDIO_SC_SAMPLING_FREQ_MASK (0xC1)

#if defined(FT_AUDIO_DEBUG_ON)
static const char* I2C_statusbits[] = {
    "I2C_BUSY (The Bus is currently busy transmitting/reveiving)",
    "I2C_ERR (An error occurred (ADDR_ACK, DATA_ACK, ARB_LOST))",
    "ADDR_ACK (Slave address was not acknowledged)",
    "DATA_ACK (Data was not acknowledged)",
    "ARB_LOST (Arbitration lost)",
    "I2C_IDLE (The I2C Controller is idle)",
    "BUS_BUSY (The I2C Bus is busy)",
    "-"
};
#endif

typedef struct
{
    uint8_t addr;
    uint8_t data;
} reg_t;

typedef enum FT_AUDIO_SAMPLING_FREQUENCY
{
	FT_AUDIO_UNSUPPORTED_SAMPLING_FREQ = 0x00000000,
	FT_AUDIO_8000hz  =  0x00001F40,
	FT_AUDIO_11025hz =  0x00002B11,
	FT_AUDIO_22050hz =  0x00005622,
	FT_AUDIO_44100hz =  0x0000AC44,
	FT_AUDIO_16000hz =  0x00003E80,
	FT_AUDIO_32000hz =  0x00007D00,
	FT_AUDIO_48000hz =  0x0000BB80,
	FT_AUDIO_96000hz =  0x00017700,
	FT_AUDIO_192000hz = 0x0002EE00,
}FT_AUDIO_SAMPLING_FREQUENCY;

typedef enum FT_AUDIO_WM8731_SAMPLING_FREQUENCY
{
	FT_AUDIO_WM8731_UNSUPPORTED_SAMPLING_FREQ = 0x00000000,
	FT_AUDIO_WM8731_32000hz =  0x00007D00,
	FT_AUDIO_WM8731_44100hz =  0x0000AC44,
}FT_AUDIO_WM8731_SAMPLING_FREQUENCY;

typedef enum FT_AUDIO_OVERSAMPLING_RATE
{
	FT_AUDIO_32fs = 0x00000000,
	FT_AUDIO_64fs = 0x00000001,
	FT_AUDIO_128fs = 0x00000002,
	FT_AUDIO_256fs = 0x00000004,
	FT_AUDIO_384fs = 0x00000008,
	FT_AUDIO_512fs = 0x00000010,
	FT_AUDIO_MINIMUM_OVERSAMPLING = 0x00000020,
	FT_AUDIO_MAXIMUM_OVERSAMPLING = 0x00000040,
}FT_AUDIO_OVERSAMPLING_RATE;

typedef enum FT_AUDIO_CHANNEL_BIT_LENGTH
{
	FT_AUDIO_UNSUPPORTED_CHANNEL_LENGTH = 0x00000000,
	FT_AUDIO_16bit = 0x00000010,
	FT_AUDIO_20bit = 0x00000014,
	FT_AUDIO_24bit = 0x00000018,
	FT_AUDIO_32bit = 0x00000020,
}FT_AUDIO_CHANNEL_BIT_LENGTH;

typedef enum FT_AUDIO_DATA_FORMAT
{
	FT_AUDIO_I2S_FORMAT = 0x00000000,
	FT_AUDIO_LEFT_JUSTIFIED_FORMAT = 0x00000001,
	FT_AUDIO_RIGHT_JUSTIFIED_FORMAT = 0x00000002,
}FT_AUDIO_DATA_FORMAT;

typedef enum FT_AUDIO_DATA_PADDING
{
	FT_AUDIO_NO_PADDING  = 0x00000000,
	FT_AUDIO_PADD_4bits  = 0x00000001,
	FT_AUDIO_PADD_8bits  = 0x00000002,
	FT_AUDIO_PADD_12bits = 0x00000004,
	FT_AUDIO_PADD_16bits = 0x00000008,
}FT_AUDIO_DATA_PADDING;

typedef enum FT_AUDIO_DEVICE_MODE
{
	FT_AUDIO_MASTER = 0x00000000,
	FT_AUDIO_SLAVE = 0x00000001,
}FT_AUDIO_DEVICE_MODE;

typedef enum FT_AUDIO_ERROR_STATUS
{
	FT_AUDIO_ERROR_UNSUPPORTED_CHANNEL_BIT_LENGTH = 0x00000001,
	FT_AUDIO_ERROR_UNSUPPORTED_SAMPLING_FREQ = 0x00000002,
}FT_AUDIO_ERROR_STATUS;

typedef struct FT_AUDIO_CONFIG
{
	i2s_mode_t mode;
	i2s_length_t length;
	i2s_format_t format;
	i2s_padding_t padding;
	i2s_master_input_clk_t mclk_in;
	i2s_bclk_div bclk_div;
	i2s_mclk_div_t mclk_div;
	i2s_bclk_per_channel_t bclk_per_channel;
}FT_AUDIO_CONFIG;

typedef enum FT_AUDIO_REGISTER_UPDATE_MODES
{
	FT_AUDIO_UPDATE_REGISTER,
	FT_AUDIO_SET_REGISTER,
}FT_AUDIO_REGISTER_UPDATE_MODES;

#if 0
static const reg_t i2c_data[] =
{
    /* 0xF Reset -
       Writing 0x00 to register resets device
    */
    {0x0F << 1, 0x00},

    /* 0x6 Power Down Control -
       bit 0 = 1 : LINEINPD - 0 = Disable Power Down 0
       bit 1 = 1 : MICPD    - 0 = Disable Power Down 1
       bit 2 = 1 : ADCPD    - 0 = Disable Power Down 0
       bit 3 = 0 : DACPD    - 0 = Disable Power Down 0
       bit 4 = 0 : OUTPD    - 0 = Disable Power Down 0
       bit 5 = 0 : OSCPD    - 0 = Disable Power Down 1
       bit 6 = 1 : CLKOUTPD - 0 = Disable Power Down 1
       bit 7 = 0 : POWEROFF - 0 = Disable POWEROFF
    */
    {0x06 << 1, 0x00},  //original
	//{0x06 << 1, 0x6A},
	//{0x06 << 1, 0x22},  //less noise but i2s audio requires the powered down audio

    /*
       0x7 Digital Audio Interface Format -
       bit 1:0 = 10 : FORMAT[1:0] - 10 = I2S
       bit 3:2 = 00 : IWL[1:0]    - 00 = 16bit
       bit 4   = 0  : LRP         - 0 = Right Ch when DACLRC Low
       bit 5   = 0  : LRSWAP      - DAC Left Right Clock Swap
       bit 6   = 0  : MS          - 0 = Enable Slave Mode
       bit 7   = 0  : BCLKINV     - 1 = 0 = Don't Invert BCLK
    */
	{0x07 << 1, DIGITAL_AUDIO_FORMAT},

    /* 0x0 Left Line In -
       bit 4:0 = 10111 : LINVOL[4:0] - 10111 = Input Volume to 0dB
       bit 7   = 0     : LINMUTE     - 0 = Disable Mute
    */
    //{0x00 << 1 | 1 /* Load left and right */, 0x1F},  //+6dB
	//{0x00, 0x97}, //original input volume, no gain
	//{0x02, 0x97},
	{0x00, 0x17},
	{0x02, 0x17},

    /* 0x2 Left Headphone Out -
       bit 6:0 = 1111001 : LHPVOL[6:0] - 1111001 = Output Volume to 0dB
       bit 7   = 1       : LZCEN       - Left Channel Zero Cross detect 1 = Enable
    */
    {0x02 << 1 | 1 /* Load left and right */, 0xFF}, //max volume for both audio out channels
	//{0x02 << 1 | 1 /* Load left and right */, 0xF9}, //no gain

    /* 0x5 Digital Audio Path Control -
       bit 0   = 0  : ADCHPD     - 0 = Enable High Pass Filter
       bit 2:1 = 00 : DEEMP[1:0] - De-emphasis Control 00 = Disable
       bit 3   = 0  : DACMU      - 0 = Disable Soft Mute
       bit 4   = 0  : HPOR       - 0 = Clear Offset
    */
    {0x05 << 1, 0x00}, //original
	//{0x05 << 1, 0x14},
	//{0x05 << 1, 0x04},


    /* 0x4 Analogue Audio Path Control -
       bit 0 = 0 : MICBOOST     - 0 = Enable Boost
       bit 1 = 1 : MUTEMIC      - 1 = Enable Mute
       bit 2 = 0 : INSEL        - 0 = Microphone Input Select to ADC
       bit 3 = 1 : BYPASS       - 1 = Disable Bypass
       bit 4 = 1 : DACSEL       - 0 = Select DAC
       bit 5 = 0 : SIDETONE     - 0 = Disable Side Tone Attenuation
       bit 6 = 0 : SIDEATT[1:0] - Ignored
    */
    {0x04 << 1, 0x1A}, //original.
	//{0x04 << 1, 0x14}, //no background noise for i2s audio but bypass is required for pwm audio from ft81x


    /* 0x8 Sampling Control -
       bit 0   = 0    : USB/NORMAL - 1 = Normal Mode (256/384 fs)
       bit 1   = 0    : BOSR       - 1 = 256 fs
       bit 5:2 = 1000 : SR[3:0]    - 1000 = 44.1 kHz sample rate
       bit 6   = 0    : CLKIDIV2   - 0 = Core Clock is MCLK
       bit 7   = 0    : CLKODIV2   - 0 = CLOCKOUT is Core
    */
	{0x08 << 1, 0x21},


    /* 0x9 Active Control -
       bit 0 = 1 : ACTIVE - 1 = Active
    */
    {0x09 << 1, 0x01}
};
#endif //if 0

void FT_Audio_CustomSetup(i2s_mode_t mode, i2s_length_t length, i2s_format_t format, i2s_padding_t padding, i2s_master_input_clk_t mclk_in, i2s_bclk_div bclk_div, i2s_mclk_div_t mclk_div, i2s_bclk_per_channel_t bclk_per_channel);

#endif

ft_uint32_t FT_Audio_Getfreespace();

ft_int32_t FT_Audio_Fill_With_Zeros();

/*Play the specified amount of audio data in the 'buf' buffer.*/
ft_uint32_t FT_Audio_Play(ft_uint8_t* buf, ft_uint32_t sz);

/* Mute the current audio play back device.*/
void FT_Audio_Mute();

/* Unmute the current audio play back device.*/
void FT_Audio_UnMute();

void FT_Audio_Use_Analog_In();

void FT_Audio_Use_I2S();

/* Stop the audio data transmission */
void FT_Audio_Stop_Transmission();

/* Start the audio data transmission after the audio data is in the fifo*/
void FT_Audio_Start_Transmission();

void FT_Audio_Set_Headphone_Volume(ft_int8_t vol);

ft_int32_t FT_Audio_Fill_With_Num_Zeros(ft_uint32_t num);

void FT_Audio_Set_Bit_Per_Sample(ft_uint8_t sample);

ft_uint32_t FT_Audio_Playback_Setup(struct FT_AUDIO_CONFIG *pConfig, ft_uint8_t channelBitLength, ft_uint32_t samplingFreq);

#if defined(FT900_PLATFORM)
void FT_Audio_Set_Config(FT_AUDIO_CONFIG *config, FT_AUDIO_DEVICE_MODE mode, FT_AUDIO_DATA_FORMAT format, FT_AUDIO_DATA_PADDING padding, FT_AUDIO_CHANNEL_BIT_LENGTH channelBitLength, FT_AUDIO_SAMPLING_FREQUENCY samplingFreq, FT_AUDIO_OVERSAMPLING_RATE overSamplingFreq);
#endif

#endif /* FT_AUDIO_H_ */
