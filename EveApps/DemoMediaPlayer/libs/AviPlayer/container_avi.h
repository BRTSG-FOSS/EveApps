#ifndef _AVI_CONTAINER_H_
#define _AVI_CONTAINER_H_
#include "FT_Platform.h"

#if defined(FT900_PLATFORM)
#include "ff.h"
#endif

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

#define AVI_CONTAINER_STANDARD_INDEX_ENTRIES_BUFFER_SIZE (32)
#define AVI_CONTAINER_IDX1_INDEX_ENTRIES_BUFFER_SIZE (AVI_CONTAINER_STANDARD_INDEX_ENTRIES_BUFFER_SIZE * 2)

#define AVI_CONTAINER_MAX_MOVI_SIZE (2147483647) /* 2 GB for AVI 1.0 */

#define AVI_CONTAINER_VIDEO_STREAM_INDEX (0x00000000) //video at stream 0
#define AVI_CONTAINER_AUDIO_STREAM_INDEX (0x00010000) //audio at stream 1

#define AVI_CONTAINER_VIDEO_FRAME_FOURCC /*(0x00006463)*/ (0x63643030)
#define AVI_CONTAINER_AUDIO_FRAME_FOURCC /*(0x00017762)*/ (0x62773130)

#define AVI_CONTAINER_SUPER_INDEX_SIZE (4 * 1024)
#define AVI_CONTAINER_SUPER_INDEX_ENTRY_SIZE (16)
#define AVI_CONTAINER_SUPER_INDEX_MAX_ENTRIES (AVI_CONTAINER_SUPER_INDEX_SIZE/16)

#define AVI_CONTAINER_AVI_STANDARD_INDEX_TABLE_SIZE (100 * 1024)
#define AVI_CONTAINER_AVI_STANDARD_INDEX_MAX_ENTRIES (AVI_CONTAINER_AVI_STANDARD_INDEX_TABLE_SIZE/8)

#define AVI_CONTAINER_IDX1_INDEX_TABLE_SIZE (AVI_CONTAINER_AVI_STANDARD_INDEX_TABLE_SIZE * 2)
#define AVI_CONTAINER_IDX1_INDEX_MAX_ENTRIES (AVI_CONTAINER_AVI_STANDARD_INDEX_TABLE_SIZE/16)

#define AVI_CONTAINER_AVIX_STANDARD_INDEX_TABLE_SIZE (500 * 1024)
#define AVI_CONTAINER_AVIX_STANDARD_INDEX_MAX_ENTRIES (AVI_CONTAINER_AVIX_STANDARD_INDEX_TABLE_SIZE/8)

#define AVI_CONTAINER_STANDARD_INDEX_NOENTRY_SIZE (24) //struct size without the entries

#define AVI_CONTAINER_TEMP_BUF_SIZE 1024

/* relative index table fields offset from 'movi', assuming the video standard index tableAVIRIFF_t
 * is placed right after the 'movi' list and followed by the audio standard index
 * table. */
#define AVI_CONTAINER_VIDEO_SINDEX_NENTRIESINUSE (16)
#define AVI_CONTAINER_VIDEO_SINDEX_QWBASEOFFSET (24)
#define AVI_CONTAINER_VIDEO_SINDEX_OFFSET (44)

#define AVI_CONTAINER_AUDIO_SINDEX_NENTRIESINUSE (48 + AVI_CONTAINER_AVIX_STANDARD_INDEX_TABLE_SIZE)
#define AVI_CONTAINER_AUDIO_SINDEX_QWBASEOFFSET (56 + AVI_CONTAINER_AVIX_STANDARD_INDEX_TABLE_SIZE)
#define AVI_CONTAINER_AUDIO_SINDEX_OFFSET (68 + AVI_CONTAINER_AVIX_STANDARD_INDEX_TABLE_SIZE)

#define AVI_CONTAINER_WRITE_OUT_HEADER (0x01)
#define AVI_CONTAINER_UPDATE_HEADER (0x02)

#define AVI_CONTAINER_AVI_RIFF (0x01)
#define AVI_CONTAINER_AVIX_RIFF (0x02)

#define AVI_CONTAINER_CHUNK_HEADER_SZ (8)
#define AVI_CONTAINER_LIST_HEADER_SZ (8)
/* Audio formats */
#define FT_WAVE_FORMAT_UNKNOWN                 0x0000
#define FT_WAVE_FORMAT_PCM                     0x0001 //MM900EVXX only support PCM audio in 16, 24, 32 bit length
///no direct support on the following
/*
#define FT_WAVE_FORMAT_ADPCM                   0x0002
#define FT_WAVE_FORMAT_IEEE_FLOAT              0x0003
#define FT_WAVE_FORMAT_VSELP                   0x0004
#define FT_WAVE_FORMAT_IBM_CVSD                0x0005
#define FT_WAVE_FORMAT_ALAW                    0x0006
#define FT_WAVE_FORMAT_MULAW                   0x0007
#define FT_WAVE_FORMAT_ADPCM_IMA_WAV           0x0011
*/

#define FCC_AVI_RIFF_LE
#define FCC_AVI_AVI
#define FCC_AVI_LIST

#define FT_AVI_CHUNK_HDR_SIZE (8) /* dwFourCC, dwSize */
/*

 */
#define AVI_CONTAINER_MAX_FILE_SIZE_WITH_SEEK (2147483647)


typedef enum FRAME_TYPE_T
{
	FT_AVI_CONTAINER_4CC_COMPRESSED_VIDEO = 0x00000001,
	FT_AVI_CONTAINER_4CC_UNCOMPRESSED_VIDEO = 0x00000002,
	FT_AVI_CONTAINER_4CC_AUDIO = 0x00000004,
	FT_AVI_CONTAINER_4CC_TEXT = 0x00000008,
	FT_AVI_CONTAINER_4CC_PALETTE_CHANGE = 0x00000010,
}FRAME_TYPE;

typedef enum FT_AVI_CONTAINER_STATUS_T
{
	FT_AVI_CONTAINER_STATUS_NO_ERROR = 0x00000000,
	FT_AVI_CONTAINER_STATUS_DISK_ERROR = 0x00000001,
	FT_AVI_CONTAINER_STATUS_MAX_VIDEO_SIZE = 0x00000002,
}FT_AVI_CONTAINER_STATUS;

typedef enum FT_AVI_CONTAINER_STATE_T
{
	FT_AVI_CONTAINER_STATE_READY = 0x00000000,
	FT_AVI_CONTAINER_STATE_PRECHUNK_PROCESSED = 0x00000001,
	FT_AVI_CONTAINER_STATE_POSTCHUNK_PROCESSED = 0x00000002,
}FT_AVI_CONTAINER_STATE;

typedef struct avistreamheader_t {
	ft_uint32_t  fcc; //'strh' lower case (big endian)
	ft_uint32_t  dwSize;	//0x00000038 , 56 bytes
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
} __attribute__((packed)) AVISTREAMHEADER;

typedef struct AVIMOVILIST_T{
	ft_uint32_t LIST; //always 'LIST'
	ft_uint32_t dwSize;
	ft_uint32_t listType; //always 'movi'

	//frame data follows
} __attribute__((packed)) AVIMOVILIST;

typedef struct BITMAPINFOHEADER_T {
  ft_uint32_t  fcc;
  ft_uint32_t  dwSize;
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
} __attribute__((packed)) BITMAPINFOHEADER_T;

typedef struct WAVEFORMATEX_T{
	ft_uint32_t fcc;
	ft_uint32_t dwSize;
  ft_uint16_t  wFormatTag;
  ft_uint16_t  nChannels;
  ft_uint32_t  nSamplesPerSec;
  ft_uint32_t  nAvgBytesPerSec;
  ft_uint16_t  nBlockAlign;
  ft_uint16_t  wBitsPerSample;
  ft_uint16_t  cbSize; //this property should not be present in some audio formats, non-pcm types.
} __attribute__((packed)) WAVEFORMATEX_T;

typedef struct ODML_T{
	ft_uint32_t LIST; //always 'LIST'
	ft_uint32_t list_Size;
	ft_uint32_t list_type; //always 'odml'
	ft_uint32_t fcc; //always 'dmlh'
	ft_uint32_t dwSize;
	ft_uint32_t totalFrames;
} __attribute__((packed)) ODML;

typedef struct VIDEOSTREAMLIST_T{
	//12bytes
	ft_uint32_t LIST; //always 'LIST' (big endian)
	ft_uint32_t dwSize; //calculate me
	ft_uint32_t fcc; //always 'strl' (big endian)

	AVISTREAMHEADER vstrh; //video stream header //64bytes

	BITMAPINFOHEADER_T vstrf; //video stream format //48bytes
} __attribute__((packed)) VIDEOSTREAMLIST;

typedef struct AUDIOSTREAMLIST_T{
	ft_uint32_t LIST; //always 'LIST' (big endian)
	ft_uint32_t dwSize; //calculate me
	ft_uint32_t fcc; //always 'strl' (big endian)
	AVISTREAMHEADER astrh; //audio stream header //64bytes
	WAVEFORMATEX_T astrf; //audio stream format //26bytes
} __attribute__((packed)) AUDIOSTREAMLIST;

typedef struct avimainheader_t {
  ft_uint32_t  fcc; // 'avih' lower case
  ft_uint32_t  dwSize; //56 - does not include fcc and chunk size
  ft_uint32_t  dwMicroSecPerFrame; //micro seconds per frame: 1000000 / fps, fps need from setup
  ft_uint32_t  dwMaxBytesPerSec; //ignore
  ft_uint32_t  dwPaddingGranularity; //ignore
  ft_uint32_t  dwFlags; //always AVIF_HASINDEX and AVIF_ISINTERLEAVED , fixed at 0x00000110
  ft_uint32_t  dwTotalFrames;  //the total frames for RIFF AVI, open dml updates another header chunk.
  ft_uint32_t  dwInitialFrames; //ignore
  ft_uint32_t  dwStreams; //number of streams in the file
  ft_uint32_t  dwSuggestedBufferSize; //the largest chunk size
  ft_uint32_t  dwWidth; //resolution width need from setup
  ft_uint32_t  dwHeight;//resolution height need from setup
  ft_uint32_t  dwReserved[4]; //ignore
} __attribute__((packed))  AVIMAINHEADER;

typedef struct _avioldindex
{
    ft_uint32_t fcc;
    ft_uint32_t dwSize;
} __attribute__((packed)) AVIOLDINDEX;

//Align to sector size 512
#define FILE_SECTOR_SIZE 512
#define FILE_SECTOR_ALIGN_MASK (FILE_SECTOR_SIZE-1)

#define FILE_SECTOR_ALIGNMENT(s) (((s) + FILE_SECTOR_ALIGN_MASK) & ~FILE_SECTOR_ALIGN_MASK)

typedef struct _JUNK_AVI_header_t{
    ft_uint32_t fcc;
    ft_uint32_t dwSize;
    ft_uint8_t  unused[FILE_SECTOR_SIZE - 4*8 - sizeof(AVIMAINHEADER) - sizeof(VIDEOSTREAMLIST) - sizeof(AVIMOVILIST)];
} __attribute__((packed)) JUNK_AVI_header;

typedef struct AVIRIFF_t {
	//24bytes
	ft_uint32_t RIFF; //always 'RIFF'
	ft_uint32_t riff_dwsize; //size for current RIFF AVI
	ft_uint32_t AVI; //always 'AVI '
	ft_uint32_t LIST; //always 'LIST' (big endian)
	ft_uint32_t hdrl_dwSize; //update at the end
	ft_uint32_t fcc; //always 'hdrl' (big endian)
	//64bytes
	AVIMAINHEADER avih; //always 64bytes
	VIDEOSTREAMLIST vstrl;
	JUNK_AVI_header junk;
	//reserve space for video super index table: AVI_CONTAINER_SUPER_INDEX_SIZE
	/*AUDIOSTREAMLIST astrl;*/ //Not support audio now
	AVIMOVILIST movi;
} __attribute__((packed)) AVIRIFF;

typedef struct AVI_CHUNK_t {
	ft_uint32_t dwFourCC;
	ft_uint32_t dwSize;
} __attribute__((packed)) AVI_CHUNK;

typedef struct AVI_CONTAINER_IDX1_ENTRY_t{
	ft_uint32_t dwChunkId;
    ft_uint32_t dwFlags;
    ft_uint32_t dwOffset;
    ft_uint32_t dwSize;
} __attribute__((packed)) AVI_CONTAINER_IDX1_ENTRY;

typedef struct AVI_CONTAINER_Frame_Header_t{
	ft_uint32_t frameType;
	ft_uint32_t frameSz;
} AVI_CONTAINER_Frame_Header;

typedef struct AVI_CONTAINER_video_info_t{
	ft_uint32_t smallestChunkSz;
	ft_uint8_t curFrameIncomplete;
}AVI_CONTAINER_video_info;



typedef struct FT_AVI_Container_Attributes_t
{
	AVIRIFF aviriff;

	ft_uint32_t curRiffOffset; //the absolute offset for the current RIFF
	ft_uint32_t totalFileSz;

	//ft_uint32_t		idx1IndexOffset; //idx1 index table in the idx1TableFile file.
	ft_uint32_t		idx1TempIndexValidItems;
	AVI_CONTAINER_IDX1_ENTRY idx1TempIndex[AVI_CONTAINER_IDX1_INDEX_ENTRIES_BUFFER_SIZE];

	ft_uint32_t		totalVideoFramesCount;
	ft_uint32_t		moviListSize;
	ft_uint32_t		curAbsOffset;
	ft_uint32_t		curBuffedAbsOffset;
	ft_uint32_t		curMoviRelativeOffset;//amount of data from the current 'movi'
	ft_uint32_t		curRIFFSz; /* Each 'movi' list in the RIFF will be restricted to 1GB or index table can no longer accomadate more entries. */
	ft_uint32_t		curChunkOffset; //the chunk offset for the current
	ft_uint32_t		curChunkSize; /* keep the current amount of data written to the file, it will be compared to the size reported at the end of the frame */
	ft_uint32_t		lastFrameEndOffset; //the previous offset for dropped frames
	FT_AVI_CONTAINER_STATE state;
	AVI_CONTAINER_video_info vidInfo;
}FT_AVI_Container_Attributes;


#if defined(FT900_PLATFORM)
//void container_avi_frame_postprocessing(RecorderCtxt_t *pRecCtxt, const ft_uint32_t frameSz, FRAME_TYPE frameType){
void container_avi_update_riff_offsets(ft_uint32_t value);
FT_AVI_CONTAINER_STATUS container_avi_frame_postprocessing(FIL *pFile, FIL *vIdxFile, ft_uint32_t frameSz, FRAME_TYPE frameType);
void container_avi_update_riff_offsets(ft_uint32_t value);
void container_avi_reset_chunk_offsets();
void container_avi_truncate_chunk_offsets();
void container_avi_trim_offsets(uint32_t size);
void container_avi_truncate_frame();
void container_avi_word_aligned(FIL *pFile, const ft_uint32_t frameSz);
#elif defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM)

#endif


#endif
