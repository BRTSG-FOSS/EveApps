
#ifndef ESD_COWIDGET__H
#define ESD_COWIDGET__H

#include "Esd_Context.h"
#include "Esd_Math.h"

/*

The coprocessor provides several commands which use the same CMD_STOP
command, such as CMD_SPINNER and CMD_SKETCH. This source file provides
an interface to use these without causing conflicts on the CMD_STOP
command. Other long-running coprocessor commands are also handled here,
such as CMD_LOGO, and CMD_PLAYVIDEO.

*/

ESD_FUNCTION(Esd_CoWidget_Spinner, Type = void, Attributes = ESD_CORE_EXPORT, Category = _GroupHidden, Include = "Esd_Core.h")
ESD_PARAMETER(owner, Type = void *, Default = 0, Hidden, Internal)
ESD_PARAMETER(x, Type = int16_t, Default = 0) // SCREEN_SIZE
ESD_PARAMETER(y, Type = int16_t, Default = 0) // SCREEN_SIZE
ESD_PARAMETER(style, Type = uint16_t, Default = 0) // TODO: Spinner style enum
ESD_PARAMETER(scale, Type = uint16_t, Default = 0)
ESD_CORE_EXPORT void Esd_CoWidget_Spinner(void *owner, int16_t x, int16_t y, uint16_t style, uint16_t scale);

ESD_FUNCTION(Esd_CoWidget_ScreenSaver, Type = void, Attributes = ESD_CORE_EXPORT, Category = _GroupHidden, Include = "Esd_Core.h")
ESD_PARAMETER(owner, Type = void *, Default = 0, Hidden, Internal)
ESD_CORE_EXPORT void Esd_CoWidget_ScreenSaver(void *owner);

ESD_FUNCTION(Esd_CoWidget_Sketch, Type = void, Attributes = ESD_CORE_EXPORT, Category = _GroupHidden, Include = "Esd_Core.h")
ESD_PARAMETER(owner, Type = void *, Default = 0, Hidden, Internal)
ESD_PARAMETER(globalRect, Type = Esd_Rect16, Default = 0) // SCREEN_SIZE
ESD_PARAMETER(addr, Type = uint32_t, Default = 0)
ESD_PARAMETER(format, Type = uint16_t, Default = L8)
ESD_CORE_EXPORT void Esd_CoWidget_Sketch(void *owner, Esd_Rect16 globalRect, uint32_t addr, uint16_t format);

ESD_FUNCTION(Esd_CoWidget_Stop, Type = void, Attributes = ESD_CORE_EXPORT, Category = _GroupHidden, Include = "Esd_Core.h")
ESD_PARAMETER(owner, Type = void *, Default = 0, Hidden, Internal)
ESD_CORE_EXPORT void Esd_CoWidget_Stop(void *owner);

/* Pops up the default spinner on the next frame (the frame currently being built) until the subsequent frame is ready.
This is useful when a blocking function will keep the system busy on the next frame. */
ESD_FUNCTION(Esd_CoWidget_PopupSpinner, DisplayName = "Pop-up Spinner", Type = void, Attributes = ESD_CORE_EXPORT, Category = EsdUtilities, Include = "Esd_Core.h")
ESD_CORE_EXPORT void Esd_CoWidget_PopupSpinner();

/* Plays the specified Esd_BitmapInfo video in the background.
Video only, not applicable to bitmap cell animation. 
Only one video can play in the background at a time. 
Background play will be interrupted when any other video is started, and cannot be resumed. */
ESD_FUNCTION(Esd_CoWidget_PlayBgVideo, Type = bool, Attributes = ESD_CORE_EXPORT, Category = EsdUtilities, Include = "Esd_Core.h")
ESD_PARAMETER(video, Type = Esd_BitmapCell)
ESD_CORE_EXPORT bool Esd_CoWidget_PlayBgVideo(Esd_BitmapCell video);

ESD_FUNCTION(Esd_CoWidget_StopBgVideo, Type = void, Attributes = ESD_CORE_EXPORT, Category = EsdUtilities, Include = "Esd_Core.h")
ESD_CORE_EXPORT void Esd_CoWidget_StopBgVideo();

ESD_ENUM(Esd_Opt_PlayVideo, Type = uint16_t, Include = "Esd_Core.h", Flags)
ESD_IDENTIFIER(OPT_FULLSCREEN)
ESD_IDENTIFIER(OPT_MEDIAFIFO)
ESD_IDENTIFIER(OPT_FLASH) /* BT815+ */
ESD_IDENTIFIER(OPT_NOTEAR)
ESD_IDENTIFIER(OPT_SOUND)
ESD_IDENTIFIER(OPT_OVERLAY) /* BT815+ */
ESD_IDENTIFIER(OPT_NODL) /* BT815+ */
ESD_END()

ESD_ENUM(Esd_Opt_PlayVideoEx, Type = uint16_t, Include = "Esd_Core.h", Flags)
ESD_IDENTIFIER(OPT_FULLSCREEN)
ESD_IDENTIFIER(OPT_NOTEAR)
ESD_IDENTIFIER(OPT_SOUND)
ESD_END()

/* Play video fullscreen, returns when the video is done playing. No interactivity */
ESD_FUNCTION(Esd_CoWidget_PlayVideoFile, Type = bool, Attributes = ESD_CORE_EXPORT, Category = EsdUtilities, Include = "Esd_Core.h")
ESD_PARAMETER(filename, Type = const char *)
ESD_PARAMETER(options, Type = Esd_Opt_PlayVideoEx, Default = OPT_FULLSCREEN | OPT_SOUND)
ESD_CORE_EXPORT bool Esd_CoWidget_PlayVideoFile(const char *, uint16_t options);

#endif /* #ifndef ESD_COWIDGET__H */

/* end of file */
