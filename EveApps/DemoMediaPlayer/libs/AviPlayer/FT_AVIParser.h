#if defined(FT900_PLATFORM)
#include "ff.h"
#endif
#ifndef _FT_AVIPARSER_H_
#define _FT_AVIPARSER_H_


#include "FT_Platform.h"
#include "Ft_Gpu_Hal.h"
#include "Gpu_Hal.h"
#include "Common.h"

#if defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM) 
/* AVI parser context - current status, statistics */
#define FT_AVIPARSER_BUFFER_SIZE		(1024*1024)	
#elif defined(FT900_PLATFORM)
/* AVI parser context - current status, statistics */
#define FT_AVIPARSER_BUFFER_SIZE		(10*1024)	//20 sectors of SD card
//#define FT_AVIPARSER_BUFFER_SIZE		(20*1024)
#endif

#define FT_AVIP_MAXRIFFHDLS (4)
#define FT_AVIP_SECTOR_SIZE	(512)
#define FT_AVIP_JPGFRAMEHDR	(0xD8FF)


#if defined(FT900_PLATFORM)
//#define AUDIO_BUFFER_SIZE (35*1024)
#define AUDIO_BUFFER_SIZE (27*1024)
#elif defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM)
#define AUDIO_BUFFER_SIZE (1024*1024)
#endif

#define FRAME_LEEWAY (1)
#define POTENTIALCORRUPTIONSZ (1000000)

extern volatile ft_uint32_t parsingAudio;
extern volatile ft_uint32_t parsingVideo;

typedef enum FT_AVIPARSER_SET
{
	FT_AVIPARSER_SETNONE				= 0x00000000,
	FT_AVIPARSER_AUDIO 					= 0x00000001,
	FT_AVIPARSER_VIDEO 					= 0x00000002,
	FT_AVIPARSER_TEXT 					= 0x00000004,
	FT_AVIPARSER_VIDEO_SKIPTOKEYFRAME 	= 0x00000008,
	FT_AVIPARSER_DROPAUDIO 				= 0x00000010,				//skip audio chunks
	FT_AVIPARSER_DROPVIDEO 				= 0x00000020,				//skip video chunks
	FT_AVIPARSER_DROPALLMEDIA			= 0x00000040,				//skip all media
	FT_AVIPARSER_FAKESYNC 				= 0x00000080,				//skip all media
	FT_AVIPARSER_DROP_VIDEO_ONE_FRAME	= 0x00000100,
}FT_AVIPARSER_SET;

#define FT_FCC(ch4) ((((ft_uint32_t)(ch4) & 0xFF) << 24) |     \
                  (((ft_uint32_t)(ch4) & 0xFF00) << 8) |    \
                  (((ft_uint32_t)(ch4) & 0xFF0000) >> 8) |  \
                  (((ft_uint32_t)(ch4) & 0xFF000000) >> 24))
#define FT_AVIPARSER_TESTSTUB_AUDBUFFMAXSZ	(1*1024)
/* Macro definitions for FOURCC - optimized for little endian processor */
#define FT_AVI_4CC_RIFF		FT_FCC('RIFF')
#define FT_AVI_4CC_AVI 		FT_FCC('AVI ')
#define FT_AVI_4CC_AVIX		FT_FCC('AVIX')
#define FT_AVI_4CC_LIST		FT_FCC('LIST')
#define FT_AVI_4CC_rec 		FT_FCC('rec ')
#define FT_AVI_4CC_idx1		FT_FCC('idx1')
#define FT_AVI_4CC_hdrl		FT_FCC('hdrl')
#define FT_AVI_4CC_avih		FT_FCC('avih')
#define FT_AVI_4CC_strl		FT_FCC('strl')
#define FT_AVI_4CC_strf		FT_FCC('strf')
#define FT_AVI_4CC_strh		FT_FCC('strh')
#define FT_AVI_4CC_strn		FT_FCC('strn')
#define FT_AVI_4CC_strd		FT_FCC('strd')
#define FT_AVI_4CC_isft		FT_FCC('isft')
#define FT_AVI_4CC_odml		FT_FCC('odml')
#define FT_AVI_4CC_dmlh		FT_FCC('dmlh')
#define FT_AVI_4CC_INFO		FT_FCC('INFO')
#define FT_AVI_4CC_movi		FT_FCC('movi')
#define FT_AVI_4CC_JUNK		FT_FCC('JUNK')
#define FT_AVI_4CC_auds		FT_FCC('auds')
#define FT_AVI_4CC_mids		FT_FCC('mids')
#define FT_AVI_4CC_txts		FT_FCC('txts')
#define FT_AVI_4CC_vids		FT_FCC('vids')
#define FT_AVI_4CC_indx		FT_FCC('indx')
#define FT_AVI_4CC_GAB2		FT_FCC('GAB2')
#define FT_AVI_4CC_ISFT		FT_FCC('ISFT')
#define FT_AVI_4CC_WAVE		FT_FCC('WAVE')
#define FT_AVI_4CC_RIFX		FT_FCC('RIFX')
#define FT_AVI_4CC_RMID		FT_FCC('RMID')
#define FT_AVI_4CC_vedt		FT_FCC('vedt')
#define FT_AVI_4CC_dvsd		FT_FCC('dvsd')
#define FT_AVI_4CC_dvhd		FT_FCC('dvhd')
#define FT_AVI_4CC_dvsl		FT_FCC('dvsl')
#define FT_AVI_4CC_vprp		FT_FCC('vprp')

/* Four CC codes for media chunks - ideally should be xxdc/db/ */
#define FT_AVI_4CC_00dc		FT_FCC('00dc')	//video
#define FT_AVI_4CC_01dc		FT_FCC('01dc')
#define FT_AVI_4CC_00wb		FT_FCC('00wb')	//audio
#define FT_AVI_4CC_01wb		FT_FCC('01wb')
#define FT_AVI_4CC_00tx		FT_FCC('00tx')	//text
#define FT_AVI_4CC_01tx		FT_FCC('01tx')
#define FT_AVI_4CC_ix00		FT_FCC('ix00')	//index
#define FT_AVI_4CC_ix01		FT_FCC('ix01')
#define FT_AVI_4CC_00db		FT_FCC('00db')	//uncompressed video
#define FT_AVI_4CC_01db		FT_FCC('01db')
#define FT_AVI_4CC_00pc		FT_FCC('00pc')	//palette change
#define FT_AVI_4CC_01pc		FT_FCC('01pc')

#define FT_AVI_4CC_COMPRESSED_VIDEO  	FT_FCC('00dc')
#define FT_AVI_4CC_UNCOMPRESSED_VIDEO 	FT_FCC('00db')
#define FT_AVI_4CC_AUDIO 				FT_FCC('00wb')
#define FT_AVI_4CC_TEXT					FT_FCC('00tx')	
#define FT_AVI_4CC_PALETTE_CHANGE		FT_FCC('00pc')	

/* FourCC for motion jpeg  */
#define FT_AVI_4CCHANDLER_JPEG	FT_FCC('jpeg')   
#define FT_AVI_4CCHANDLER_MJPG	FT_FCC('MJPG')
#define FT_AVI_4CCHANDLER_FLJP	FT_FCC('FLJP')
#define FT_AVI_4CCHANDLER_FRWA	FT_FCC('FRWA')
#define FT_AVI_4CCHANDLER_FRWD	FT_FCC('FRWD')
#define FT_AVI_4CCHANDLER_GPEG	FT_FCC('GPEG')
#define FT_AVI_4CCHANDLER_VIXL	FT_FCC('VIXL')

/* Audio code types */
#define FT_ACM_MPEG_LAYER1 0x0001
#define FT_ACM_MPEG_LAYER2 0x0002
#define FT_ACM_MPEG_LAYER3 0x0004
#define FT_ACM_MPEG_STEREO 0x0001
#define FT_ACM_MPEG_JOINTSTEREO 0x0002
#define FT_ACM_MPEG_DUALCHANNEL 0x0004
#define FT_ACM_MPEG_SINGLECHANNEL 0x0008
#define FT_ACM_MPEG_PRIVATEBIT 0x0001
#define FT_ACM_MPEG_COPYRIGHT 0x0002
#define FT_ACM_MPEG_ORIGINALHOME 0x0004
#define FT_ACM_MPEG_PROTECTIONBIT 0x0008
#define FT_ACM_MPEG_ID_MPEG1 0x0010

#define FT_AVIFORMATTAG_AC3			0x2000UL
#define FT_AVIFORMATTAG_DTS			0x2001UL
#define FT_AVIFORMATTAG_VORBIS		0x566FUL

/* mp3 specific flags */
#define FT_MPEGLAYER3_FLAG_PADDING_ISO 		0x00000000UL
#define FT_MPEGLAYER3_FLAG_PADDING_ON 		0x00000001UL
#define FT_MPEGLAYER3_FLAG_PADDING_OFF	 	0x00000002UL

/* PCM types */
#define FT_WAVE_FORMAT_UNKNOWN                 0x0000
#define FT_WAVE_FORMAT_PCM                     0x0001
#define FT_WAVE_FORMAT_ADPCM                   0x0002 //original AVI spec value
#define FT_WAVE_FORMAT_IEEE_FLOAT              0x0003
#define FT_WAVE_FORMAT_VSELP                   0x0004
#define FT_WAVE_FORMAT_IBM_CVSD                0x0005
#define FT_WAVE_FORMAT_ALAW                    0x0006
#define FT_WAVE_FORMAT_MULAW                   0x0007
#define FT_WAVE_FORMAT_ADPCM_IMA_WAV           0x0011

#define FT_WAVE_FORMAT_MPEGLAYER3              0x0055

/* dwFlags of avih - AVIMAINHEADER */
#define FT_AVIF_HASINDEX       		0x00000010UL
#define FT_AVIF_MUSTUSEINDEX   		0x00000020UL
#define FT_AVIF_ISINTERLEAVED  		0x00000100UL
#define FT_AVIF_TRUSTCKTYPE    		0x00000800UL
#define FT_AVIF_WASCAPTUREFILE 		0x00010000UL
#define FT_AVIF_COPYRIGHTED    		0x00020000UL

/* dwflags for strh -AVISTREAMHEADER */
#define FT_AVISF_DISABLED         	0x00000001UL
#define FT_AVISF_VIDEO_PALCHANGES 	0x00010000UL

/* flags for dwFlags member of _avioldindex_entry */
#define FT_AVIIF_LIST       		0x00000001UL
#define FT_AVIIF_KEYFRAME   		0x00000010UL
#define FT_AVIIF_NO_TIME    		0x00000100UL
#define FT_AVIIF_COMPRESSOR 		0x0FFF0000UL


/* flags for avi index */
#define FT_AVISTDINDEX_DELTAFRAME ( 0x80000000) // Delta frames have the high bit set
#define FT_AVISTDINDEX_SIZEMASK   (~0x80000000)

#define FT_AVI_MAXSIZE_1_0			(0x80000000UL)
#define FT_AVI_MAXSIZE_ODML			(0x40000000UL)
#define FT_AVI_MAXSIZE_ODML_AVIX	(0x80000000UL)

/* AVI index flags */
#define FT_AVI_INDEX_OF_INDEXES			0x00000000
#define FT_AVI_INDEX_OF_CHUNKS			0x00000000
#define FT_AVI_INDEX_IS_DATA			0x00000080

/* AVI index type: standard, field, or super index */
#define FT_AVI_STANDARD_INDEX_CHUNK 	0x00  //standard index, might or might not be present
#define FT_AVI_INDEX_2FIELD 0x01  //field index, same as standard index but a little more information is embedded in the structure

#define FT_AVI_INVALID_STREAM	(65535)
#define FT_AVI_MIN_FREE_SPACE_FOR_WRITE (50000)

typedef enum FT_AVIPARSER_FLAGS
{
	FT_AVIPARSER_FLAGSNONE = 0x00000000,
	FT_AVIPARSER_ISODML = 0x00000001,
	FT_AVIPARSER_HASIDX = 0x00000002,
	FT_AVIPARSER_KEYFRAME = 0x00000004,
	FT_AVIPARSER_PARTIALFRAME = 0x00000008,
	FT_AVIPARSER_PARTIALBUFFER = 0x00000010,
	FT_AVIPARSER_NEWFRAME = 0x00000020,
}FT_AVIPARSER_FLAGS;

typedef enum FT_AVIPMETAD_FLAGS
{
	FT_AVIPMETAD_FLAGSNONE = 0x00000000,
	FT_AVIPMETAD_KEYFRAME = 0x00000001,
	FT_AVIPMETAD_PARTIALFRAME = 0x00000002,
	FT_AVIPMETAD_PARTIALBUFFER = 0x00000004,
	FT_AVIPMETAD_NEWFRAME = 0x00000008,
	FT_AVIMETAD_DROPPEDFRAME = 0x00000010,
}FT_AVIPMETAD_FLAGS;

/* return types of parser */
typedef enum FT_AVIPARSER_STATUS
{
	FT_AVIPARSER_COPYERROR = -10,
	FT_AVIPARSER_FREADERR = -9,
	FT_AVIPARSER_FSEEKERR = -8,
	FT_AVIPARSER_INVALIDFILE = -7,
	FT_AVIPARSER_NOTAVIFILE = -6,
	FT_AVIPARSER_FILENOTFOUND = -5,
	FT_AVIPARSER_INVALIDPARAMS = -4,
	FT_AVIPARSER_UNKNOWNERR = -3,
	FT_AVIPARSER_FILEERR = -2,
	FT_AVIPARSER_HDRERR = -1,
	FT_AVIPARSER_OK = 0,
	FT_AVIPARSER_EOF = 1,
	FT_AVIPARSER_BQEFULL = 2,		//either buffers or queue elements are full
	FT_AVIPARSER_PARTIALBUFFERUSECASE = 3,
}FT_AVIPARSER_STATUS;
/*
typedef enum FT_AVIPARSER_SET
{
	FT_AVIPARSER_SETNONE				= 0x00000000,
	FT_AVIPARSER_AUDIO 					= 0x00000001,
	FT_AVIPARSER_VIDEO 					= 0x00000002,
	FT_AVIPARSER_TEXT 					= 0x00000004,
	FT_AVIPARSER_VIDEO_SKIPTOKEYFRAME 	= 0x00000008,
	FT_AVIPARSER_DROPAUDIO 				= 0x00000010,				//skip audio chunks
	FT_AVIPARSER_DROPVIDEO 				= 0x00000020,				//skip video chunks
	FT_AVIPARSER_DROPALLMEDIA			= 0x00000040,				//skip all media
	FT_AVIPARSER_FAKESYNC 				= 0x00000080,				//skip all media

}FT_AVIPARSER_SET;
*/
/* Standard structures from msdn */
typedef struct avimainheader_t {
  ft_uint32_t  fcc;
  ft_uint32_t  cb;
  ft_uint32_t  dwMicroSecPerFrame;
  ft_uint32_t  dwMaxBytesPerSec;
  ft_uint32_t  dwPaddingGranularity;
  ft_uint32_t  dwFlags;
  ft_uint32_t  dwTotalFrames;
  ft_uint32_t  dwInitialFrames;
  ft_uint32_t  dwStreams;
  ft_uint32_t  dwSuggestedBufferSize;
  ft_uint32_t  dwWidth;
  ft_uint32_t  dwHeight;
  ft_uint32_t  dwReserved[4];
} AVIMAINHEADER;

typedef struct avistreamheader_t {
	ft_uint32_t  fcc;
	ft_uint32_t  cb;
	ft_uint32_t  fccType;
	ft_uint32_t  fccHandler;
	ft_uint32_t  dwFlags;
	ft_uint16_t  wPriority;
	ft_uint16_t  wLanguage;
	ft_uint32_t  dwInitialFrames;
	ft_uint32_t  dwScale;
	ft_uint32_t  dwRate;
	ft_uint32_t  dwStart;
	ft_uint32_t  dwLength;
	ft_uint32_t  dwSuggestedBufferSize;
	ft_uint32_t  dwQuality;
	ft_uint32_t  dwSampleSize;
  struct {
	  ft_uint16_t left;
	  ft_uint16_t top;
	  ft_uint16_t right;
	  ft_uint16_t bottom;
  } rcFrame;
} AVISTREAMHEADER;

typedef struct BITMAPINFOHEADER_T {
  ft_uint32_t  biSize;
  ft_int32_t   biWidth;
  ft_int32_t   biHeight;
  ft_uint16_t  biPlanes;
  ft_uint16_t  biBitCount;
  ft_uint32_t  biCompression;
  ft_uint32_t  biSizeImage;
  ft_int32_t   biXPelsPerMeter;
  ft_int32_t   biYPelsPerMeter;
  ft_uint32_t  biClrUsed;
  ft_uint32_t  biClrImportant;
} BITMAPINFOHEADER_T;

typedef struct {
  ft_uint16_t  wFormatTag;
  ft_uint16_t  nChannels;
  ft_uint32_t  nSamplesPerSec;
  ft_uint32_t  nAvgBytesPerSec;
  ft_uint16_t  nBlockAlign;
  ft_uint16_t  wBitsPerSample;
  ft_uint16_t  cbSize;
} WAVEFORMATEX_T;

typedef struct {
  WAVEFORMATEX_T Format;
  union {
    ft_uint16_t wValidBitsPerSample;
    ft_uint16_t wSamplesPerBlock;
    ft_uint16_t wReserved;
  } Samples;
  ft_uint32_t         dwChannelMask;
  ft_uint8_t          SubFormat[16];
} WAVEFORMATEXTENSIBLE;


typedef struct mpeg1waveformat_tag {
  WAVEFORMATEX_T 			wfx;
  ft_uint16_t          	fwHeadLayer;
  ft_uint32_t          	dwHeadBitrate;
  ft_uint16_t         	fwHeadMode;
  ft_uint16_t         	fwHeadModeExt;
  ft_uint16_t        	wHeadEmphasis;
  ft_uint16_t         	fwHeadFlags;
  ft_uint32_t        	dwPTSLow;
  ft_uint32_t        	dwPTSHigh;
} MPEG1WAVE;

typedef struct mpeglayer3waveformat_tag {
  WAVEFORMATEX_T 		  wfx;
  ft_uint16_t         wID;
  ft_uint32_t         fdwFlags;
  ft_uint16_t         nBlockSize;
  ft_uint16_t         nFramesPerBlock;
  ft_uint16_t         nCodecDelay;
} MPEGLAYER3WAVEFORMAT;


typedef struct _avioldindex
{
    ft_uint32_t fcc;
    ft_uint32_t cb;
    struct _avioldindex_entry
    {
        ft_uint32_t dwChunkId;
        ft_uint32_t dwFlags;
        ft_uint32_t dwOffset;
        ft_uint32_t dwSize;
    } aIndex[];
} AVIOLDINDEX;

typedef union _timecode
{
    struct
    {
        ft_uint16_t wFrameRate;
        ft_uint16_t wFrameFract;
        ft_int32_t cFrames;
    } DUMMYSTRUCTNAME;
    ft_uint8_t qw[8];
} TIMECODE;


typedef struct _avimetaindex {
   ft_uint32_t fcc;
   ft_int32_t   cb;
   ft_uint16_t   wLongsPerEntry;
   ft_uint8_t   bIndexSubType;
   ft_uint8_t   bIndexType;
   ft_uint32_t  nEntriesInUse;
   ft_uint32_t  dwChunkId;
   ft_uint32_t  dwReserved[3];
   ft_uint32_t  adwIndex[];
   } AVIMETAINDEX;

   typedef struct _avisuperindex {
   ft_uint32_t   fcc;               // 'indx'
   ft_uint32_t     cb;                // size of this structure
   ft_uint16_t     wLongsPerEntry;    // ==4
   ft_uint8_t     bIndexSubType;     // ==0 (frame index) or AVI_INDEX_SUB_2FIELD 
   ft_uint8_t     bIndexType;        // ==AVI_INDEX_OF_INDEXES
   ft_uint32_t    nEntriesInUse;     // offset of next unused entry in aIndex
   ft_uint32_t    dwChunkId;         // chunk ID of chunks being indexed, (i.e. RGB8)
   ft_uint32_t    dwReserved[3];     // must be 0
   struct _avisuperindex_entry {
      ft_uint8_t qwOffset[8];    // 64 bit offset to sub index chunk
      ft_uint32_t    dwSize;       // 32 bit size of sub index chunk
      ft_uint32_t    dwDuration;   // time span of subindex chunk (in stream ticks)
      } aIndex[];
   } AVISUPERINDEX;
// struct of a standard index (AVI_INDEX_OF_CHUNKS)
//
typedef struct _avistdindex_entry {
   ft_uint32_t dwOffset;       // 32 bit offset to data (points to data, not riff header)
   ft_uint32_t dwSize;         // 31 bit size of data (does not include size of riff header), bit 31 is deltaframe bit
   } AVISTDINDEX_ENTRY;
typedef struct _avistdindex {
   ft_uint32_t   fcc;               // 'indx' or '##ix'
   ft_int32_t     cb;                // size of this structure
   ft_uint16_t     wLongsPerEntry;    // ==2
   ft_uint8_t     bIndexSubType;     // ==0
   ft_uint8_t     bIndexType;        // ==AVI_INDEX_OF_CHUNKS
   ft_uint32_t    nEntriesInUse;     // offset of next unused entry in aIndex
   ft_uint32_t    dwChunkId;         // chunk ID of chunks being indexed, (i.e. RGB8)
   ft_uint8_t qwBaseOffset[8];     // base offset that all index intries are relative to
   ft_uint32_t    dwReserved_3;      // must be 0
   AVISTDINDEX_ENTRY aIndex[];
   } AVISTDINDEX;

// struct of a time variant standard index (AVI_INDEX_OF_TIMED_CHUNKS)
//
typedef struct _avitimedindex_entry {
   ft_uint32_t dwOffset;       // 32 bit offset to data (points to data, not riff header)
   ft_uint32_t dwSize;         // 31 bit size of data (does not include size of riff header) (high bit is deltaframe bit)
   ft_uint32_t dwDuration;     // how much time the chunk should be played (in stream ticks)
   }AVITIMEDINDEX_ENTRY;

typedef struct _avitimedindex {
   ft_uint32_t   fcc;               // 'indx' or '##ix'
   ft_int32_t     cb;                // size of this structure
   ft_uint16_t     wLongsPerEntry;    // ==3
   ft_uint8_t     bIndexSubType;     // ==0
   ft_uint8_t     bIndexType;        // ==AVI_INDEX_OF_TIMED_CHUNKS
   ft_uint32_t    nEntriesInUse;     // offset of next unused entry in aIndex
   ft_uint32_t    dwChunkId;         // chunk ID of chunks being indexed, (i.e. RGB8)
   ft_uint8_t qwBaseOffset[8];     // base offset that all index intries are relative to
   ft_uint32_t    dwReserved_3;      // must be 0
   AVITIMEDINDEX_ENTRY aIndex[1];
   ft_uint32_t adwTrailingFill[1]; // to align struct to correct size
   } AVITIMEDINDEX;

// structure of a timecode stream
//
typedef struct _avitimecodeindex {
   ft_uint32_t   fcc;               // 'indx' or '##ix'
   ft_int32_t     cb;                // size of this structure
   ft_uint16_t     wLongsPerEntry;    // ==4
   ft_uint8_t     bIndexSubType;     // ==0
   ft_uint8_t     bIndexType;        // ==AVI_INDEX_IS_DATA
   ft_uint32_t    nEntriesInUse;     // offset of next unused entry in aIndex
   ft_uint32_t    dwChunkId;         // 'time'
   ft_uint32_t    dwReserved[3];     // must be 0
   //TIMECODEDATA aIndex[NUMINDEX(sizeof(TIMECODEDATA)/sizeof(ft_int32_t))];
   } AVITIMECODEINDEX;



typedef struct _avifieldindex_chunk {
   ft_uint32_t   fcc;               // 'ix##'
   ft_uint32_t    cb;                // size of this structure
   ft_uint16_t     wLongsPerEntry;    // must be 3 (size of each entry in
                               // aIndex array)
   ft_uint8_t     bIndexSubType;     // AVI_INDEX_2FIELD
   ft_uint8_t     bIndexType;        // AVI_INDEX_OF_CHUNKS
   ft_uint32_t    nEntriesInUse;     //
   ft_uint32_t    dwChunkId;         // '##dc' or '##db'
   ft_uint8_t 	  qwBaseOffset[8];     // offsets in aIndex array are relative to this
   ft_uint32_t    dwReserved3;       // must be 0
   struct _avifieldindex_entry {
      ft_uint32_t    dwOffset;
      ft_uint32_t    dwSize;         // size of all fields
                               // (bit 31 set for NON-keyframes)
      ft_uint32_t    dwOffsetField2; // offset to second field
   } aIndex[1];
} AVIFIELDINDEX, * PAVIFIELDINDEX;


/* Metadata element for video */
typedef struct FT_MetaV
{
	ft_uint32_t Pts;			/* Presentation time stamp in terms of units specified in file format */
	ft_uint32_t Curr;			/* Location of the video data */
	ft_uint32_t DeviceWrtPtr;  /* the current valid data pointer in the fifo buffer*/
	ft_uint32_t Length;			/* Length of the video frame */
	ft_uint32_t PartialSz;		/* Size of partial data copied into ring buffer */
	ft_uint32_t ReadSz;		/* The amount of data has been read by the */
	ft_uint32_t Flags;			/* Flags such as valid, standalone decodable frame, complete frame,  */
	ft_uint32_t Frame; 
	ft_uint8_t FlushData;
}FT_MetaV_t;

typedef struct FT_AviPMediaStat
{
	ft_uint32_t 	MoviOff;				/* Offset of movichunk */
	ft_uint32_t 	IndxOff;				/* offset of idx1 */
}FT_AviPMediaStat_t;

/* Structure used for passing fillchunks */
typedef struct FT_AviFillCkhs
{
	ft_uint32_t VideoLen;		//video length to be filled
	ft_uint32_t AudioLen;		//audio length to be filled

}FT_AviFillCkhs_t;

typedef struct FT_MetaA
{
	ft_uint32_t Pts;			/* Presentation time stamp in terms of units specified in file format */
	ft_uint32_t Curr;			/* Location of the audio data */
	ft_uint32_t Length;			/* Length of the video frame */
	ft_uint32_t PartialSz;		/* Size of partial data copied into ring buffer */
	ft_uint32_t ReadSz;   ///this variable indicates the amount of data read by the audio device.
	ft_uint32_t Flags;			/* Flags such as valid, standalone decodable frame, complete frame,  */
	ft_uint32_t Frame;			//if the AVI consist of audio and video then this variable indicates which video the current audio frame belongs as video frame might not have corresponding audio frame(s)
	ft_uint8_t FlushData;
}FT_MetaA_t;

/* parser context for RIFF chunk */
typedef struct FT_AVIRiff
{
	ft_uint32_t 	RiffOff;				/* Offset of RIFF in file */
	ft_uint32_t 	RiffSize;				/* Size of riff chunk */
	ft_uint32_t 	MoviOff;				/* Offset of movichunk */
	ft_uint32_t 	MoviSize;				/* Size of movi chunk */
	ft_uint32_t 	indxOff;				/* offset of idx1 */
	ft_uint32_t 	indxSize;				/* size of idx1 chunk */
}FT_AVIRiff_t;
/* parser context for RIFF chunk */


/* Structures maintained by parser for codec and media */
typedef struct FT_AviV
{
    ft_uint32_t remaining;
    ft_uint32_t packet_size;

    ft_uint32_t TotFrames;			/* total number of frames of all the RIFF chunks */
	ft_uint32_t CurrFrame;			/* Current frame parsed */
    ft_uint32_t TimeScale;
    ft_uint32_t DataRate;
    ft_uint32_t SampleSize;        /* size of one sample (or packet) (in the rate/scale sense) in bytes */

    ft_uint8_t IndexSubType;
    ft_uint32_t ValidSuperIndexEntries;
    ft_uint32_t SuperIndexOffset;
    ft_uint32_t Flags;
    ft_uint16_t Width;
    ft_uint16_t Height;
}FT_AviV_t;

typedef struct FT_AviA
{
    ft_uint32_t remaining;
    ft_uint32_t packet_size;

    ft_uint32_t TotFrames;			/* total number of frames of all the RIFF chunks */
	ft_uint32_t CurrFrame;			/* Current frame parsed */
    ft_uint32_t TimeScale;
    ft_uint32_t DataRate;
    ft_uint32_t SampleSize;        /* size of one sample (or packet) (in the rate/scale sense) in bytes */
    ft_uint32_t	SamplingFreq;
    ft_uint8_t  NumChannels;
    ft_uint8_t  BitsPerSample;
    ft_uint32_t AudioFormat;
    ft_uint8_t IndexSubType;
    ft_uint32_t ValidSuperIndexEntries;
    ft_uint32_t SuperIndexOffset;
}FT_AviA_t;

/* audio data path structure - except parser */
typedef struct FT_AudDataPath
{
	/* input queue for bitstream - metadata */
	FT_RingQueue_t  *pairq;
	/* input ring buffer for input bitstream data */
	FT_RingBuffer_t *pairb;
	
	/* output queue for PCM - metadata */
	FT_RingQueue_t  *paorq;
	/* Output ring buffer for output PCM data */
	FT_RingBuffer_t *paorb;
}FT_AudDataPath_t;

/* Structure for video data path except parser */
typedef struct FT_VidDataPath
{
	/* input queue for bitstream - metadata */
	FT_RingQueue_t  *pvirq;
	/* pointer to hal context */
	Ft_Gpu_Hal_Context_t *phalctxt;
	/* input ring buffer maintained in ft81x */
	Fifo_t *pfifo;
	
	/* Device ring buffer for the video data from the parser */
	Fifo_t *vfifo;

	/* output queue for frames - metadata */
	FT_RingQueue_t  *pvorq;
	/* Output ring buffer for output frames */
	FT_RingBuffer_t *pvorb;
	ft_uint32_t NumDecoded;
}FT_VidDataPath_t;

/* Structure to hold the status of parser */


typedef struct FT_AVIParser
{
	/* file related parameters */
	ft_uint8_t			NumRiff;			/* Number of RIFF files in the stream */
	ft_uint8_t			CurrRiff;			/* Current avi riff */
	ft_uint8_t			NumMedia;			/* Number of media - audio/video/text etc */
	ft_uint16_t			VideoStream;  
	ft_uint8_t			VideoStreamFound;
	ft_uint16_t 		AudioStream;  
	ft_uint8_t			AudioStreamFound;
	ft_uint8_t			CurrMediaType;		/* Current media type been parsed */
	ft_uint32_t			FileSz;				/* Total file size - max is 4GB as of now - should we support 64bit datatype? */
	ft_uint32_t			VideoChunkSz; //video frame chunk size for the media fifo
	FT_NBuffer_t		SNbuff;				/* Scratch buffer */
	//FT_RingBuffer_t SRingBuff;			/* First implementation with ring buffer, later change to Nbuffer for parallel approach */

	/* Flags */
	ft_uint32_t			Flags;				/* audio, video, fake,  */
#if defined(FT900_PLATFORM)
	FIL					File;				/* File handle - for streaming better to come up with abstraction layer */
#elif defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM)
	FILE	*File;
#endif
	FT_AVIRiff_t		Riff[FT_AVIP_MAXRIFFHDLS];//hardcoding the riffs to 4
	FT_AviPMediaStat_t	CurrStat;			/* Current offsets maintained for parsing */
	FT_AviV_t			SVid;				/* media structure for video information */
	FT_AviA_t			SAud;				/* media structure for audio information */
	ft_uint32_t 		tempstore;
	ft_uint32_t 		curFrame;
}FT_AVIParser_t;

#ifdef __cplusplus
extern "C" {
#endif

/* Layer for parsing - return 0 in case of EOF - currently directly usign file pointer for optimization */
ft_int32_t FT_AVIPRead(FT_AVIParser_t *pAviP,ft_uint8_t *pOutBuffer,ft_int32_t OutLength, ft_int32_t *pReadLength);//return status in case of EOF or fatal error etc
ft_uint8_t FT_AVIPRead8(FT_AVIParser_t *pAviP);
ft_uint16_t FT_AVIPReadL16(FT_AVIParser_t *pAviP);
ft_uint32_t FT_AVIPReadL32(FT_AVIParser_t *pAviP);
ft_uint16_t FT_AVIPReadB16(FT_AVIParser_t *pAviP);
ft_uint32_t FT_AVIPReadB32(FT_AVIParser_t *pAviP);//read 64bit?
ft_int32_t FT_AVIPFlush(FT_AVIParser_t *pAviP,ft_int32_t FlushLength);
ft_int32_t FT_AVIPFetch(FT_AVIParser_t *pAviP,ft_uint32_t FileOffset);//fetch fresh data from this offset


/* Initialize AVI parser */
ft_int32_t	FT_AVIParser_Init(FT_AVIParser_t *pCtxt);
//set audio only, video only, fast seek, drop video frames, fakesync
ft_int32_t	FT_AVIParser_SetProp(FT_AVIParser_t *pCtxt);
//properties of overall stream, audio, video, text etc,
ft_int32_t	FT_AVIParser_GetProp(FT_AVIParser_t *pCtxt);
ft_int32_t	FT_AVIParser_SetBuff(FT_AVIParser_t *pCtxt,ft_uint8_t *pbuffer,ft_int32_t BuffLen);
ft_int32_t	FT_AVIParser_SetOpenFile(FT_AVIParser_t *pCtxt,ft_char8_t *pFilename);
ft_int32_t	FT_AVIParser_Reset(FT_AVIParser_t *pCtxt,ft_char8_t *pFilename);
//parse the whole avi file - to be done at the starting of the playback
//need to put protection for parser to parse upto 4 riffs
ft_int32_t	FT_AVIParser_ParseAvi(FT_AVIParser_t *pCtxt);
//parser only one RIFF and fill the context respectively
ft_int32_t	FT_AVIParser_ParseAviRiff(FT_AVIParser_t *pCtxt);
ft_int32_t	FT_AVIParser_SetRiff(FT_AVIParser_t *pCtxt,ft_uint8_t RiffNo);

//fill one chunk/frame of all the media
ft_int32_t	FT_AVIParser_FillChunk(FT_AVIParser_t *pCtxt,FT_VidDataPath_t *pvdp,FT_AudDataPath_t *padp);

ft_int32_t FT_AVIFillVdata(FT_AVIParser_t *pCtxt,FT_VidDataPath_t *pvdp,ft_uint32_t VDatatocopy, int isPartial);

ft_int32_t FT_AVIFillAdata(FT_AVIParser_t *pCtxt,FT_AudDataPath_t *padp,ft_uint32_t ADatatocopy, int isPartial);
//flush all media contents
ft_int32_t	FT_AVIParser_FlushAll(FT_AVIParser_t *pCtxt);
//flush only specific media content
ft_int32_t	FT_AVIParser_Flush(FT_AVIParser_t *pCtxt);
//seek all the media contents to a specific time
ft_int32_t	FT_AVIParser_AviSeek(FT_AVIParser_t *pCtxt);
//free up every thing and exit
ft_int32_t	FT_AVIParser_Exit(FT_AVIParser_t *pCtxt);

#ifdef __cplusplus
}
#endif

#endif
