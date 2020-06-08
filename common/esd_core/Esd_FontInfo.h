/*
ESD Framework
Copyright (C) 2018  Bridgetek Pte Lte
Author: Jan Boon <jan.boon@kaetemi.be>
*/

/*
Font info structure
*/

#ifndef ESD_FONTINFO__H
#define ESD_FONTINFO__H

#include "ESD_Base.h"
#include "ESD_GpuAlloc.h"
#include "ESD_ResourceInfo.h"

ESD_ENUM(ESD_FontType, DisplayName = "Bitmap Format")
#define ESD_FONT_ROM 0
#define ESD_FONT_LEGACY 1
#define ESD_FONT_EXTENDED 2
ESD_END()

// Preset font formats
ESD_ENUM(_FontResourceFormat, DisplayName = "Font Resource Format")
// ESD_IDENTIFIER(LEGACY_L1)
// ESD_IDENTIFIER(LEGACY_L8)
ESD_IDENTIFIER(EXTENDED_ASTC)
ESD_END()

// Structure providing information on how a font is loaded, as well as it's current loaded state
ESD_TYPE(ESD_FontInfo, Native = Struct)
typedef struct ESD_FontInfo // (40 bytes) (56 bytes on 64 bit)
{
	// (Runtime) Bitmap handle that is being used
	uint8_t BitmapHandle : 6; // Do not change. (see ESD_RomFontInfo)

	// Type of font to load (ESD_FontType)
	uint8_t Type : 2; // Do not change. (see ESD_RomFontInfo)

	// First character in the glyph map (for legacy fonts)
	uint8_t FirstChar;

	// (Runtime) Font height
	uint16_t FontHeight;

	// (Runtime) Baseline, from top of font glyph
	uint16_t BaseLine;

	// (Runtime) Caps height of font, measured from baseline
	uint16_t CapsHeight;

	// (Runtime) The glyph address as specified in the loaded font resource
	uint32_t GlyphAddress;

	// The font map
	ESD_ResourceInfo FontResource;

	// Glyphs
	ESD_ResourceInfo GlyphResource;

} ESD_FontInfo;

ESD_TYPE(ESD_FontInfo *, Native = Pointer, Edit = Library)

// Reduced ESD FontInfo structure with binary compatible type header, specific for ROM fonts
typedef struct ESD_RomFontInfo // (4 bytes)
{
	// (Runtime) Bitmap handle that is being used
	uint8_t BitmapHandle : 6;

	// Type of font to load (ESD_FONT_ROM)
	uint8_t Type : 2;

	// Rom font
	uint8_t RomFont;

	// Font height
	uint16_t FontHeight;

	// (Runtime) Baseline, from top of font glyph
	uint16_t BaseLine;

	// (Runtime) Caps height of font, measured from baseline
	uint16_t CapsHeight;

} ESD_RomFontInfo;

/// A function to load font data into RAM_G (or to use the font glyphs from flash directly if so specified)
/// Returns RAM_G address of the font information block (or GA_INVALID if the font failed to load)
uint32_t ESD_LoadFont(ESD_FontInfo *fontInfo);

/// A function to make fonts persistent in memory by reloading the data if necessary, called during the Update cycle of each frame
ESD_UPDATE(ESD_FontPersist, DisplayName = "Persist Font", Category = EsdUtilities)
ESD_PARAMETER(fontInfo, Type = ESD_FontInfo *)
void ESD_FontPersist(ESD_FontInfo *fontInfo);

#endif /* #ifndef ESD_FONTINFO__H */

/* end of file */
