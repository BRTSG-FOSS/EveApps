#include "Common.h"

EVE_HalContext *phost;
Fifo_t MediaFifo;

typedef struct {
	FILE *pfile; //Only active 1 file per times
	uint32_t file_len; /* Len not read remain */
	uint32_t FILE_SIZE; /* Size of file */
	uint32_t played_len; /* Num of buffers already loaded to RAM */
}File_t;
File_t MediaFile;

#define MEDIA_FIFO_BUFFER	(512*1024)
#define MEDIA_FIFO_ADDR		(RAM_G_SIZE - MEDIA_FIFO_BUFFER)
#define MUSIC_VOLUME         255

void Utils_Load_File2Fifo(uint32_t size) {
	uint8_t *pbuff;
	static uint8_t first = 1;
	pbuff = (uint8_t *)malloc(size);
	fseek(MediaFile.pfile, MediaFile.FILE_SIZE - MediaFile.file_len, SEEK_SET);
	if (size > MediaFile.file_len) {
		size = MediaFile.file_len;
	}
	fread(pbuff, 1, size, MediaFile.pfile);
	if (first == 0) {
		Gpu_Hal_Wr32(phost, REG_MEDIAFIFO_READ, Gpu_Hal_Rd32(phost, REG_PLAYBACK_READPTR) - MediaFifo.fifo_buff);
	} else {
		first = 0;
	}
	MediaFile.file_len -= Fifo_Write(phost, &MediaFifo, pbuff, size);
	free(pbuff);
}

void Audio_Playback(EVE_HalContext *host)
{
    phost = host;
	Fifo_Init(&MediaFifo, MEDIA_FIFO_ADDR, MEDIA_FIFO_BUFFER, REG_MEDIAFIFO_READ, REG_MEDIAFIFO_WRITE);
	Gpu_CoCmd_MediaFifo(phost, MEDIA_FIFO_ADDR, MEDIA_FIFO_BUFFER);
	printf("Mediafifo: Start address and length %d %d\n", MEDIA_FIFO_ADDR, MEDIA_FIFO_BUFFER);
	App_Flush_Co_Buffer(phost);
	Gpu_Hal_WaitCmdfifo_empty(phost);
	MediaFile.pfile = fopen("../../../relaxed-inspiration-ig-version-short-30s-6579.raw", "rb");
	if (MediaFile.pfile == NULL) {
        return;
    }
    
    fseek(MediaFile.pfile, 0, SEEK_END);
     MediaFile.FILE_SIZE = MediaFile.file_len = ftell(MediaFile.pfile);
    MediaFile.played_len = 0;
    fseek(MediaFile.pfile, 0, SEEK_SET);
    Gpu_Hal_Wr32(phost, REG_PLAYBACK_READPTR, MediaFifo.fifo_buff);
    MediaFifo.fifo_rp = 0;
    MediaFifo.fifo_wp = 0;
    /* Open and load first data to mediafifo */
    Utils_Load_File2Fifo(MEDIA_FIFO_BUFFER);

    /* Register playback - Music play from this point */
    Gpu_Hal_Wr32(phost, REG_PLAYBACK_FREQ, 44100);
    Gpu_Hal_Wr32(phost, REG_PLAYBACK_START, MediaFifo.fifo_buff);
    Gpu_Hal_Wr32(phost, REG_PLAYBACK_FORMAT, ADPCM_SAMPLES);
    Gpu_Hal_Wr32(phost, REG_PLAYBACK_LENGTH, MediaFifo.fifo_len);
    Gpu_Hal_Wr32(phost, REG_PLAYBACK_LOOP, 1);
    Gpu_Hal_Wr8(phost, REG_VOL_PB, MUSIC_VOLUME); /* Volume */
    Gpu_Hal_Wr8(phost, REG_PLAYBACK_PLAY, 1);    
}

/* end of file */
