#include "FT_Audio.h"
//#define SUPPORT_AUDIO_I2S 1

//For Audio via 81x
ft_uint8_t volume=6;
ft_void_t FT_Audio_Use_Analog_In()
{
}
void FT_Audio_Mute(){
#if defined(FT900_PLATFORM)
	//FT_Audio_Set_Headphone_Volume(-1);
#endif
}

void FT_Audio_UnMute(){
#if defined(FT900_PLATFORM)
	//FT_Audio_Set_Headphone_Volume(volume);
#endif
}

/*
Calling this function to initiate the device for audio playback.  This function resets the device so the volume output has to be re-specified.
 */
ft_uint32_t FT_Audio_Playback_Setup(struct FT_AUDIO_CONFIG *pConfig, ft_uint8_t channelBitLength, ft_uint32_t samplingFreq)
{
    return 0;
}
