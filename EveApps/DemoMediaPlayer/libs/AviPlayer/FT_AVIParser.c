/* AVI parser implementation */

/* Application inclusions */
#include "FT_Buffer.h"
#include "FT_MQueue.h"
#include "FT_AVIParser.h"
#include "FT_Platform.h"
#include "FT_Audio.h"

//#define SHOWPARSERPRINTS 1

volatile ft_uint32_t parsingAudio;
volatile ft_uint32_t parsingVideo;

#if defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM) 
#include <stdio.h>
//#define SHOWPARSERPRINTS 1
#define printf printf
#else
//#define SHOWPARSERPRINTS 1
#if 0
#define printf(fmt, ...) do {\
	ft_printf(fmt, ##__VA_ARGS__);\
}while(0)
#else
#define ft_printf printf
#endif

#endif


#define LOG_TAG "AviPlayer  "

#define FT_Error(fmt, ...) do {\
	ft_printf("E/" fmt "\n\r", ##__VA_ARGS__);\
}while(0)


#define FT_Info(fmt, ...) do {\
	ft_printf("I/" fmt "\n\r", ##__VA_ARGS__);\
}while(0)

#define FT_Debug(fmt, ...) do {\
	ft_printf("D/" fmt "\n\r", ##__VA_ARGS__);\
}while(0)

/* AVI parser design */
/*
 * heres the general layout of an AVI riff file (new format)
 *
 * RIFF (3F??????) AVI       <- not more than 1 GB in size
 *     LIST (size) hdrl
 *         avih (0038)
 *         LIST (size) strl
 *             strh (0038)
 *             strf (????)
 *             indx (3ff8)   <- size may vary, should be sector sized
 *         LIST (size) strl
 *             strh (0038)
 *             strf (????)
 *             indx (3ff8)   <- size may vary, should be sector sized
 *         LIST (size) odml
 *             dmlh (????)
 *         JUNK (size)       <- fill to align to sector - 12
 *     LIST (7f??????) movi  <- aligned on sector - 12
 *         00dc (size)       <- sector aligned
 *         01wb (size)       <- sector aligned
 *         ix00 (size)       <- sector aligned
 *     idx1 (00??????)       <- sector aligned
 * RIFF (7F??????) AVIX
 *     JUNK (size)           <- fill to align to sector -12
 *     LIST (size) movi
 *         00dc (size)       <- sector aligned
 * RIFF (7F??????) AVIX      <- not more than 2GB in size
 *     JUNK (size)           <- fill to align to sector - 12
 *     LIST (size) movi
 *         00dc (size)       <- sector aligned
 *
 *-===================================================================*/
#if defined(FT900_PLATFORM)
#define F_READ(f,b,s,ret, err) err = f_read((f), b, s, &ret)
#define F_CLOSE(f) f_close(f)
#define F_SEEK(f,pos) f_lseek((f), pos)
#define F_TELL(f) f_tell((f))
#define F_EOF(f) f_eof(f)
#else
#define F_READ(f,b,s,ret, err) (err = ret = fread(b, 1, s, (f)))
#define F_CLOSE(f) fclose(f)
#define F_SEEK(f,pos) fseek(f, pos, SEEK_SET)
#define F_TELL(f) ftell(f)
#define F_EOF(f) feof(f)
#endif

static FT_AVIParser_t *g_aviparser;

/* API to check if sufficient data is available in buffer */
ft_int32_t FT_AVIPCheckSz(FT_NBuffer_t *pnb,ft_int32_t OutLength)
{
	ft_int32_t	ValidLen;

	ValidLen = pnb->pEnd - pnb->pCurr;

	/* check if there is sufficient data in ring buffer, if not then read data into ring buffer */
	if(ValidLen < OutLength)
	{
		return 1;
	}
	return 0;
}


/* Layer for parsing - as of now using ringbuffer as scratch buffer implementation, later to be modified to Nbuffer for parallel */
//return status in case of EOF or fatal error etc
/* it is assumed that ringbuffer is filled when ever the valid size is 0, so write pointer is always pointing to the starting */
ft_int32_t FT_AVIPRead(FT_AVIParser_t *pAviP,ft_uint8_t *pOutBuffer,ft_int32_t OutLength, ft_int32_t *pReadLength)
{
	FT_NBuffer_t *pbuff;
	ft_int32_t	ValidLen;
	FT_AVIPARSER_STATUS Status = FT_AVIPARSER_OK;
#if defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM) 
	FILE* file = pAviP->File;
#else
	FIL* file = &pAviP->File;
#endif
	pbuff = &pAviP->SNbuff;
	ValidLen = pbuff->pEnd - pbuff->pCurr;
	/* check if there is sufficient data in ring buffer, if not then read data into ring buffer */
	if(ValidLen <= OutLength)
	{

		/* first read the partial chunk followed by next partial chunk */
		ft_int32_t FirstPartial, SecondPartial;
		ft_int32_t fResult = 0;

		FirstPartial = ValidLen;
		SecondPartial = OutLength - FirstPartial;

		//printf("Reload of next buffer %d %d %d %d %d %d\n",FirstPartial,SecondPartial,pbuff->pCurr,pbuff->pEnd,pbuff->Length,pbuff->pStart);
		//printf("Cur pos: %d %d \n\r", F_TELL(file), pbuff->Length);
		if(FirstPartial > 0)
			memcpy(pOutBuffer,pbuff->pCurr,FirstPartial);

		pOutBuffer += FirstPartial;//increment wrt first partial
		*pReadLength = FirstPartial;

		/* fill the buffer */
		F_READ(file, pbuff->pStart, pbuff->Length, FirstPartial, fResult);
		pbuff->pCurr = pbuff->pStart;
		pbuff->pEnd = pbuff->pStart + FirstPartial;

		if(FirstPartial < pbuff->Length)
		{
			if (F_EOF(file)) {
				printf("EOF indicator detected.\n");
				Status = FT_AVIPARSER_EOF;
			}
		}
		else if (FirstPartial <= 0) {
			Status = FT_AVIPARSER_FILEERR;
		}

		/* Fill the second partial data */
		if(SecondPartial > 0)
			memcpy(pOutBuffer,pbuff->pCurr,SecondPartial);

		pbuff->pCurr += SecondPartial;
		*pReadLength += SecondPartial;
		//printf("Reload done %d %d %d %d %x %d\n", FirstPartial, SecondPartial, pbuff->pCurr, pbuff->pEnd, *(ft_uint32_t*)pbuff->pStart,
			//F_TELL(file));
		printf("Reload done %d %d %d %d %x\n", FirstPartial, SecondPartial, pbuff->pCurr, pbuff->pEnd, *(ft_uint32_t*)pbuff->pStart);
	}
	else if(pbuff->pCurr)
	{
		memcpy(pOutBuffer,pbuff->pCurr,OutLength);
		pbuff->pCurr += OutLength;
		*pReadLength = OutLength;
	}

	return Status;
}

ft_uint8_t FT_AVIPRead8(FT_AVIParser_t *pAviP)
{
	FT_NBuffer_t *pbuff;
	pbuff = &pAviP->SNbuff;

	if((pbuff->pEnd - pbuff->pCurr) <= 1)
	{
		ft_uint8_t ReadByte;
		ft_int32_t Bytesread;

		FT_AVIPRead(pAviP,&ReadByte,1, &Bytesread);
		return (ReadByte);
	}
	else
	{

		return (*pbuff->pCurr++);
	}
}

ft_uint16_t FT_AVIPReadL16(FT_AVIParser_t *pAviP)
{
	ft_uint16_t Read16 = 0;

	ft_int32_t Bytesread;
	FT_AVIPRead(pAviP,(ft_uint8_t*)&Read16,2, &Bytesread);
	return (Read16);
}

ft_uint32_t FT_AVIPReadL32(FT_AVIParser_t *pAviP)
{
	ft_uint32_t Read32 = 0;
	ft_int32_t Bytesread;
		
	if (FT_AVIPARSER_EOF == FT_AVIPRead(pAviP, (ft_uint8_t*)&Read32, 4, &Bytesread)) {
		return FT_AVIPARSER_EOF;
	}
	//printf("READ32L %x \n",Read32);
	return (Read32);
}

#if 0
//Un-needed functions
ft_uint16_t FT_AVIPReadB16(FT_AVIParser_t *pAviP)
{
	FT_NBuffer_t *pbuff;
	ft_uint16_t Read16;

	pbuff = &pAviP->SNbuff;

	if((pbuff->pEnd - pbuff->pCurr) <= 2)
	{
		ft_uint8_t ReadArr[2];
		ft_int32_t Bytesread;

		FT_AVIPRead(pAviP,ReadArr,2, &Bytesread);
		Read16 = ((ReadArr[0]<<8) | (ReadArr[1]));
	}
	else
	{
		Read16 = ((*pbuff->pCurr++) << 8);
		Read16 |= *pbuff->pCurr++;
	}
	return (Read16);
}

ft_uint32_t FT_AVIPReadB32(FT_AVIParser_t *pAviP)
{
	FT_NBuffer_t *pbuff;
	ft_uint32_t Read32;

	pbuff = &pAviP->SNbuff;

	if((pbuff->pEnd - pbuff->pCurr) <= 4)
	{
		ft_uint8_t ReadArr[4];
		ft_int32_t Bytesread;

		FT_AVIPRead(pAviP,ReadArr,4, &Bytesread);
		Read32 = ((ReadArr[0]<<24) | (ReadArr[1]<<16) | (ReadArr[2]<<8) | ReadArr[3]);
	}
	else
	{
		Read32 = ((*pbuff->pCurr++) << 24);
		Read32 |= ((*pbuff->pCurr++) << 16);
		Read32 |= ((*pbuff->pCurr++) << 8);
		Read32 |= *pbuff->pCurr++;
	}
	return (Read32);
}
#endif //if 0

/* Fetch of fresh data from file - also taking care of sector alignment */
ft_int32_t FT_AVIPFetch(FT_AVIParser_t *pAviP,ft_uint32_t FileOffset)
{
	FT_AVIPARSER_STATUS Status = FT_AVIPARSER_OK;
	ft_int32_t	Flushsector,freadsize;
	FT_NBuffer_t *pbuff;
	ft_int32_t fResult = 0;
#if defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM) 
	FILE* file = pAviP->File;
#else
	FIL* file = &pAviP->File;
#endif
	
	pbuff = &pAviP->SNbuff;
	/* set current and end pointer respectively */
	pbuff->pCurr = pbuff->pStart;
	pbuff->pEnd = pbuff->pStart;

	Flushsector = (FileOffset & (~(FT_AVIP_SECTOR_SIZE - 1)));//assuming sector size if 2 power n

	//printf("AVIPFetch %d %d %d\n",ValidLen,FileOffset,Flushsector);
	fResult = F_SEEK(file, Flushsector);
	if (fResult != 0) {
		Status = FT_AVIPARSER_FSEEKERR;
	}
	F_READ(file, pbuff->pStart, pbuff->Length, freadsize, fResult);
	if (freadsize != pbuff->Length) {
		Status = FT_AVIPARSER_FREADERR;
	}

	pbuff->pCurr += (FileOffset - Flushsector);//change the offset to current position
	pbuff->pEnd = pbuff->pStart + freadsize;
	//printf("AVIPFetch done %d %d %d %d \n",Flushsector,pbuff->pCurr,f_tell(&pAviP->File),*pbuff->pCurr);
	
	return Status;
}
/* API to flush data from local buffer and increment wrt file */
ft_int32_t FT_AVIPFlush(FT_AVIParser_t *pAviP,ft_int32_t FlushLength)
{
	FT_NBuffer_t *pbuff;
	ft_int32_t	ValidLen,Flushsector;
	FT_AVIPARSER_STATUS Status = FT_AVIPARSER_OK;
	ft_int32_t fResult = 0;
#if defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM) 
	FILE* file = pAviP->File;
#else
	FIL* file = &pAviP->File;
#endif

	pbuff = &pAviP->SNbuff;
	ValidLen = pbuff->pEnd - pbuff->pCurr;

	//printf("AVIPFlush %d %d %d \n",ValidLen,pbuff->pCurr,pbuff->pEnd);
	/* Check if flush size is within the present buffer, if yes then increment the current pointer, if not then flush whole and seek to sector aligned location and read the whole buffer and point the current buffer to valid location */
	if(FlushLength <= ValidLen)
	{

		pbuff->pCurr += FlushLength;
	}
	else
	{
		ft_int32_t freadsize = 0;
		/* Check how much is to be flushed */
		FlushLength -= ValidLen;
		pbuff->pCurr = pbuff->pStart;
		pbuff->pEnd = pbuff->pStart;

		Flushsector = (FlushLength & (~(FT_AVIP_SECTOR_SIZE - 1)));//assuming sector size if 2 power n

		//printf("AVIPFlush %d %d %d %d\n",ValidLen,FlushLength,Flushsector,Flushsector + f_tell(&pAviP->File));
		fResult = F_SEEK(file, Flushsector + F_TELL(file));
		if (fResult != 0) {
			Status = FT_AVIPARSER_FSEEKERR;
		}
		F_READ(file, pbuff->pStart, pbuff->Length, freadsize, fResult);
		if (freadsize != pbuff->Length) {
			Status = FT_AVIPARSER_FREADERR;
		}
		pbuff->pCurr += (FlushLength - Flushsector);//change the offset to current position
		pbuff->pEnd = pbuff->pStart + freadsize;
		//printf("AVIPFlush done %d %d %d %d %d\n",Flushsector,pbuff->pCurr,f_tell(&pAviP->File),*pbuff->pCurr,freadsize);
	}

	return Status;
}
/* Initialize AVI parser */
ft_int32_t	FT_AVIParser_Init(FT_AVIParser_t *pCtxt)
{
	/* Initialize all the parameters of parser context */

	FT_AVIPARSER_STATUS Status = FT_AVIPARSER_OK;
	g_aviparser = pCtxt;
	pCtxt->NumRiff = 0;
	pCtxt->NumMedia = 0;
	pCtxt->CurrMediaType = 0;
	pCtxt->FileSz = 0;
	pCtxt->Flags = 0;
	pCtxt->CurrRiff;
	pCtxt->VideoStream = FT_AVI_INVALID_STREAM;
	pCtxt->AudioStream = FT_AVI_INVALID_STREAM;
	pCtxt->curFrame=0;

	/* Buffer initialization */
	pCtxt->SNbuff.Length = 0;
	pCtxt->SNbuff.pCurr = NULL;
	pCtxt->SNbuff.pEnd = NULL;
	pCtxt->SNbuff.pNext = NULL;
	pCtxt->SNbuff.pStart = NULL;

	pCtxt->CurrStat.IndxOff = 0;
	pCtxt->CurrStat.MoviOff = 0;

	memset(pCtxt->Riff,sizeof(FT_AVIRiff_t)*FT_AVIP_MAXRIFFHDLS,0);
	/* File pointer - specific to a file system */
	//pCtxt->File = NULL;
	pCtxt->SVid.CurrFrame = 0;
	pCtxt->SVid.TotFrames = 0;
	pCtxt->SVid.Flags = 0;
	pCtxt->SVid.IndexSubType=0;
	pCtxt->SVid.ValidSuperIndexEntries=0;
	pCtxt->SVid.SuperIndexOffset=0;
	
	pCtxt->SAud.CurrFrame = 0;
	pCtxt->SAud.TotFrames = 0;
	pCtxt->SAud.packet_size = 0;
	pCtxt->SAud.IndexSubType=0;
	pCtxt->SAud.ValidSuperIndexEntries=0;
	pCtxt->SAud.SuperIndexOffset=0;
	
	return Status;
}

/* Set Nbuffer parameters */
ft_int32_t	FT_AVIParser_SetBuff(FT_AVIParser_t *pCtxt,ft_uint8_t *pbuffer,ft_int32_t BuffLen)
{
	FT_NBuffer_t *pnb;

	if(BuffLen <= 0)
	{
		return FT_AVIPARSER_INVALIDPARAMS;
	}

	pnb = &pCtxt->SNbuff;

	pnb->Length = BuffLen;
	pnb->pStart = pbuffer;
	pnb->pCurr = pbuffer;
	pnb->pEnd = pbuffer;

	pnb->pNext = NULL;//hack - as of now N buffers are not used, only 1 buffer is used

	return FT_AVIPARSER_OK;
}

/* Set file pointer - open the  */
ft_int32_t	FT_AVIParser_SetOpenFile(FT_AVIParser_t *pCtxt,ft_char8_t *pFilename)
{
	ft_int32_t fResult = 0;

#if defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM) 
	pCtxt->File = fopen(pFilename, "rb");

	if(pCtxt->File != NULL){
		printf("Opened: %s!\n",pFilename);

	}
	else{
		printf("Unable to open: %s\n",pFilename);
		return FT_AVIPARSER_FILENOTFOUND;
	}
#elif defined(FT900_PLATFORM)
	pCtxt->Flags = 0;
	//pCtxt->Flags |= FT_AVIPARSER_DROPAUDIO;
	//pCtxt->Flags |= FT_AVIPARSER_DROPVIDEO;
	fResult = f_open(&pCtxt->File, pFilename, FA_READ);
	if(0 != fResult)
	{
		printf("Unable to open: %s  Return code: %d\n",pFilename, fResult);
		return FT_AVIPARSER_FILENOTFOUND;
	}
#endif

	pCtxt->NumRiff = 0;
#if defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM) 

	fseek(pCtxt->File, 0L, SEEK_END);
	pCtxt->FileSz = ftell(pCtxt->File);
	fseek(pCtxt->File, 0L, SEEK_SET);
#elif defined(FT900_PLATFORM)
	pCtxt->FileSz = f_size(&pCtxt->File);
#endif
	/*printf("Open file %s successful s=%d\n\r", pFilename, pCtxt->FileSz);*/
	/* Update the file related parameters in the AVIparser context */
	return FT_AVIPARSER_OK;
}

/* Close file, reset buffer pointers etc */
ft_int32_t	FT_AVIParser_Reset(FT_AVIParser_t *pCtxt,ft_char8_t *pFilename)
{
	ft_int32_t fResult = 0;
#if defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM) 
	FILE* file = pCtxt->File;
#else
	FIL* file = &pCtxt->File;
#endif

	FT_AVIParser_Init(pCtxt);

	fResult = F_CLOSE(file);
	if (fResult != 0) {
		return FT_AVIPARSER_FILENOTFOUND;
	}
	return FT_AVIPARSER_OK;
}
//set audio only, video only, fast seek, drop video frames, fakesync
ft_int32_t	FT_AVIParser_SetProp(FT_AVIParser_t *pCtxt){

#if defined(FT900_PLATFORM)
	//TODO: FIXME open this later
	//Audio_DefaultSetup();
#endif
}


//properties of overall stream, audio, video, text etc,
ft_int32_t	FT_AVIParser_GetProp(FT_AVIParser_t *pCtxt);

//parse the whole avi file - to be done at the starting of the playback
ft_int32_t	FT_AVIParser_ParseAvi(FT_AVIParser_t *pCtxt)
{
/*
#define SHOWPARSERPRINTS 1
*/
	FT_AVIPARSER_STATUS Status = FT_AVIPARSER_OK;
	ft_uint32_t Next4cc, ChunkSz, AviS,List4cc, bufferLastRead=0xFFFFFFFF;
	FT_NBuffer_t *pnb;
	ft_uint8_t parsed=0;
#if defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM) 
	FILE* file = pCtxt->File;
#else
	FIL* file = &pCtxt->File;
#endif
	
	//printf("Start parsing avi!\n\r");
	/* For safer side make sure the current file pointer is at the begining of the file */
	F_SEEK(file, 0);
	printf("Seek ok\n\r");
	Next4cc = FT_AVIPReadL32(pCtxt);
	ChunkSz = FT_AVIPReadL32(pCtxt);
	AviS = FT_AVIPReadL32(pCtxt);
	pCtxt->FileSz -= 16;//read 3 words

	printf("New file %x %x %x\n",Next4cc,ChunkSz,AviS);
	pnb = &pCtxt->SNbuff;

	/* First check for RIFF chunk, followed by parsing of all the media properties */
	if((FT_AVI_4CC_RIFF != Next4cc) || (FT_AVI_4CC_AVI != AviS))
	{
		printf("The input doesn't appear to be an AVI file: %x %x %x\n",Next4cc,ChunkSz,AviS);
		return FT_AVIPARSER_NOTAVIFILE;
	}

	pCtxt->Riff[pCtxt->NumRiff].RiffOff = F_TELL(file) - (pnb->pEnd - pnb->pCurr);
	pCtxt->Riff[pCtxt->NumRiff].RiffSize = ChunkSz - 4;//already avi is consumed
	/* update the riff handles in parser */
	pCtxt->NumRiff++;
	/* Parse all the headers, log the values, update the avi parser context */

	//while(pCtxt->FileSz > 0)//will not work in case of streaming
	while(1)//will not work in case of streaming
	{
		if (F_EOF(file)) {

			if(pCtxt->SNbuff.pEnd - pCtxt->SNbuff.pCurr){
				if(pCtxt->SNbuff.pEnd - pCtxt->SNbuff.pCurr == bufferLastRead){
					printf("EOF reached \n");
					return Status;
				}
				else{
					bufferLastRead = pCtxt->SNbuff.pEnd - pCtxt->SNbuff.pCurr;
				}
			}
			else{
				printf("EOF reached \n");
				return Status;
			}
			//goto parser_error;  ///original
		}
		Next4cc = FT_AVIPReadL32(pCtxt);
		ChunkSz = FT_AVIPReadL32(pCtxt);

		/* pad odd sized lists and structures */
		if(ChunkSz & 0x00000001){
			ChunkSz+=1;
		}
		//printf("New chunk/list %x %x\n",Next4cc,ChunkSz);
		switch(Next4cc)
		{
			case FT_AVI_4CC_RIFF:
			{
				
				/*
				parsed = 1; //added, some transcoder added extra stuffs at the end
				if(parsed == 1)
					return Status;
					*/
					
				/* check for AVIX */
				//printf("RIFF AVIX\n");
				AviS = FT_AVIPReadL32(pCtxt);
				if(AviS != FT_AVI_4CC_AVIX)
				{
					goto parser_error;
				}
				pCtxt->Riff[pCtxt->NumRiff].RiffOff = F_TELL(file) - (pnb->pEnd - pnb->pCurr);
				pCtxt->Riff[pCtxt->NumRiff].RiffSize = ChunkSz - 4;//avix is already read from buffer
				printf("RIFF AVIX %x %x\n",pCtxt->Riff[pCtxt->NumRiff].RiffOff,pCtxt->Riff[pCtxt->NumRiff].RiffSize);

				/* increment the riff index */
				pCtxt->NumRiff++;
#if defined(SHOWPARSERPRINTS)
				printf("Video now contains: %u riffs.\n",pCtxt->NumRiff);
#endif
				break;
			}
			case FT_AVI_4CC_LIST:
			{
				ft_uint32_t list4cc = FT_AVIPReadL32(pCtxt);
#if defined(SHOWPARSERPRINTS)
				printf("New list ele %x\n",list4cc);
#endif
				switch(list4cc)
				{
					case FT_AVI_4CC_hdrl:
					{
						/* next parse avih */
#if defined(SHOWPARSERPRINTS)
						printf("hdrl list\n");
#endif
						break;
					}
					case FT_AVI_4CC_strl:
					{
#if defined(SHOWPARSERPRINTS)
						printf("strl list\n");
#endif
						pCtxt->CurrMediaType = 0;
						/* check if audio or video */
						break;
					}
					case FT_AVI_4CC_odml:
					{
#if defined(SHOWPARSERPRINTS)
						printf("odml list SIZE:%u\n", ChunkSz);
#endif
						/* AVI 2.0 */
						/* skip all the info list */
						//FT_AVIPFlush(pCtxt,ChunkSz - 4);  //original
						break;
					}
					case FT_AVI_4CC_INFO:
					{
#if defined(SHOWPARSERPRINTS)
						printf("info list\n");
#endif
						/* skip all the info list */
						FT_AVIPFlush(pCtxt,ChunkSz - 4);
						break;
					}
					case FT_AVI_4CC_movi:
					{
						FT_AVIRiff_t *pmovi;
#if defined(SHOWPARSERPRINTS)
#if defined(FT900_PLATFORM)
						printf("movi list %x %x %x\n",f_tell(&pCtxt->File),pnb->pEnd,pnb->pCurr);
#endif
#endif
						pmovi = &pCtxt->Riff[pCtxt->NumRiff - 1];//need to ensure that RIFF is parsed before movi
						pmovi->MoviOff = F_TELL(file) - (pnb->pEnd - pnb->pCurr);//absolute value of movi in 4 gb file
						//pmovi->MoviSize = ChunkSz - 4;  //original
						pmovi->MoviSize = ChunkSz; 
						//pmovi->MoviOff = 0;//this is relative to movi
						if(pmovi->MoviSize == 0){
							goto parser_error;
						}
#if defined(SHOWPARSERPRINTS)
						printf("movi list %x %x %x \n",pCtxt->NumRiff,pmovi->MoviOff,pmovi->MoviSize);
#endif


						FT_AVIPFlush(pCtxt,ChunkSz - 4);
						break;
					}
					default:
					{
#if defined(SHOWPARSERPRINTS)
						printf("Error default for list %x\n",list4cc);
#endif
						FT_AVIPFlush(pCtxt,ChunkSz - 8);
						break;
					}
				}
				break;
			}
			case FT_AVI_4CC_avih:
			{
#if 0
				FT_AVIPReadL32(pCtxt);
				FT_AVIPReadL32(pCtxt);
				FT_AVIPReadL32(pCtxt);
#endif
				FT_AVIPFlush(pCtxt, 12);
				pCtxt->SVid.Flags = FT_AVIPReadL32(pCtxt);
#if 0
				FT_AVIPReadL32(pCtxt);
				FT_AVIPReadL32(pCtxt);
				FT_AVIPReadL32(pCtxt);
				FT_AVIPReadL32(pCtxt);
#endif
				FT_AVIPFlush(pCtxt, 16);
				pCtxt->SVid.Width = FT_AVIPReadL32(pCtxt);
				pCtxt->SVid.Height = FT_AVIPReadL32(pCtxt);
#if 0
				FT_AVIPReadL32(pCtxt);
				FT_AVIPReadL32(pCtxt);
				FT_AVIPReadL32(pCtxt);
				FT_AVIPReadL32(pCtxt);
#endif
				FT_AVIPFlush(pCtxt, 16);
#if defined(SHOWPARSERPRINTS)
				printf("avih width height %d %d\n",pCtxt->SVid.Width,pCtxt->SVid.Height);
#endif
				break;
			}
			case FT_AVI_4CC_strh:
			{
				ft_uint32_t fcctype,fcchandler;
#if defined(SHOWPARSERPRINTS)
				printf("strh \n");
#endif
				fcctype = FT_AVIPReadL32(pCtxt);
				{
					fcchandler = FT_AVIPReadL32(pCtxt);
#if 0
					FT_AVIPReadL32(pCtxt);
					FT_AVIPReadL32(pCtxt);
					FT_AVIPReadL32(pCtxt);
#endif
					FT_AVIPFlush(pCtxt, 12);
					if(fcctype == FT_AVI_4CC_vids)
					{
						if(pCtxt->VideoStream == FT_AVI_INVALID_STREAM){//default is to play the first video stream
							pCtxt->VideoStream = (ft_uint16_t)pCtxt->NumMedia;
							pCtxt->VideoStreamFound = 1;
						}
						else{
							//play the specified video stream
							pCtxt->VideoStreamFound = 1;
						}
#if defined(SHOWPARSERPRINTS)
						printf("Video stream found. Stream: %u\n",pCtxt->VideoStream);
#endif
						pCtxt->NumMedia++;
						pCtxt->SVid.TimeScale = FT_AVIPReadL32(pCtxt);
						pCtxt->SVid.DataRate = FT_AVIPReadL32(pCtxt);
#if 0
						FT_AVIPReadL32(pCtxt);
#endif
						FT_AVIPFlush(pCtxt, 4);
						pCtxt->SVid.TotFrames = FT_AVIPReadL32(pCtxt);
#if defined(SHOWPARSERPRINTS)
						printf("Total Frames: %u\n", pCtxt->SVid.TotFrames);
#endif
#if 0
						FT_AVIPReadL32(pCtxt);
						FT_AVIPReadL32(pCtxt);
#endif
						FT_AVIPFlush(pCtxt, 8);
						pCtxt->SVid.SampleSize = FT_AVIPReadL32(pCtxt);
#if 0
						FT_AVIPReadL32(pCtxt);
						FT_AVIPReadL32(pCtxt);
#endif
						FT_AVIPFlush(pCtxt, 8);
						pCtxt->CurrMediaType = 1;
#if defined(SHOWPARSERPRINTS)
						printf("strh video %x %x %x %x \n",pCtxt->SVid.TimeScale,pCtxt->SVid.DataRate,pCtxt->SVid.TotFrames,pCtxt->SVid.SampleSize);
#endif
					}
					else if(fcctype == FT_AVI_4CC_auds)
					{
#if defined(SHOWPARSERPRINTS)
						printf("strh audio\n");
#endif
						if(pCtxt->AudioStream == FT_AVI_INVALID_STREAM){//default is to play the first video stream
							pCtxt->AudioStream = (ft_uint16_t)pCtxt->NumMedia;
							pCtxt->AudioStreamFound = 1;
						}
						else{
							pCtxt->AudioStreamFound = 1;
						}
#if defined(SHOWPARSERPRINTS)
						printf("Audio stream found. Stream: %u\n",pCtxt->AudioStream);
#endif
						pCtxt->NumMedia++;
						pCtxt->SAud.TimeScale = FT_AVIPReadL32(pCtxt);
						pCtxt->SAud.DataRate = FT_AVIPReadL32(pCtxt);
#if 0
						FT_AVIPReadL32(pCtxt);
#endif
						FT_AVIPFlush(pCtxt, 4);
						pCtxt->SAud.TotFrames = FT_AVIPReadL32(pCtxt);
#if 0
						FT_AVIPReadL32(pCtxt);
						FT_AVIPReadL32(pCtxt);
#endif
						FT_AVIPFlush(pCtxt, 8);
						pCtxt->SAud.SampleSize = FT_AVIPReadL32(pCtxt);
#if 0
						FT_AVIPReadL32(pCtxt);
						FT_AVIPReadL32(pCtxt);
#endif
						FT_AVIPFlush(pCtxt, 8);
						pCtxt->CurrMediaType = 2;
#if defined(SHOWPARSERPRINTS)
						printf("strh audio %x %x %x %x \n",pCtxt->SAud.TimeScale,pCtxt->SAud.DataRate,pCtxt->SAud.TotFrames,pCtxt->SAud.SampleSize);
#endif
					}
					else
					{
#if defined(SHOWPARSERPRINTS)
						printf("strh txt or midi \n");
#endif
						/* could be text or midi */
#if 0
						FT_AVIPReadL32(pCtxt);
						FT_AVIPReadL32(pCtxt);
						FT_AVIPReadL32(pCtxt);
						FT_AVIPReadL32(pCtxt);
						FT_AVIPReadL32(pCtxt);
						FT_AVIPReadL32(pCtxt);
						FT_AVIPReadL32(pCtxt);
						FT_AVIPReadL32(pCtxt);
						FT_AVIPReadL32(pCtxt);
#endif
						FT_AVIPFlush(pCtxt, 36);
					}
				}
				break;
			}
			case FT_AVI_4CC_strf:
			{
#if defined(SHOWPARSERPRINTS)
				printf("strf stream format detected. chunk size: %u\n",ChunkSz);
#endif
				if(1 == pCtxt->CurrMediaType)
				{
					ft_uint32_t strsize = 0;
					pCtxt->CurrMediaType = 0;
					/* parse bitmap header */
					strsize = FT_AVIPReadL32(pCtxt);
#if defined(SHOWPARSERPRINTS)
					printf("video detected\n");
#endif
#if 0
					FT_AVIPReadL32(pCtxt);
					FT_AVIPReadL32(pCtxt);
					FT_AVIPReadL32(pCtxt);
					FT_AVIPReadL32(pCtxt);
					FT_AVIPReadL32(pCtxt);
					FT_AVIPReadL32(pCtxt);
					FT_AVIPReadL32(pCtxt);
					FT_AVIPReadL32(pCtxt);
					FT_AVIPReadL32(pCtxt);
#endif
					FT_AVIPFlush(pCtxt, 36);
					if((strsize - 4*10) != 0)
					{
#if defined(SHOWPARSERPRINTS)
						printf("video hdr has extra data %d %d\n",strsize);
#endif
						FT_AVIPFlush(pCtxt,strsize - 4*10);
					}
					pCtxt->tempstore = F_TELL(file) - (pCtxt->SNbuff.pEnd - pCtxt->SNbuff.pCurr);
				}
				else if(2 == pCtxt->CurrMediaType)
				{
					ft_uint32_t cbSize=0,strsize = FT_AVIPReadL16(pCtxt);//wave format
					pCtxt->CurrMediaType = 0;
#if defined(SHOWPARSERPRINTS)
					printf("strf audio waveformat %x\n",strsize);
#endif

					if((strsize >= FT_WAVE_FORMAT_PCM) && (strsize <= FT_WAVE_FORMAT_ADPCM_IMA_WAV))
					{
#if defined(SHOWPARSERPRINTS)
						printf("Audio detected: pcm/mulaw\n");
#endif
						/* Parse as wave WAVEFORMATEX */
						pCtxt->SAud.AudioFormat = strsize;
						pCtxt->SAud.NumChannels = FT_AVIPReadL16(pCtxt);
						pCtxt->SAud.SamplingFreq = FT_AVIPReadL32(pCtxt);
#if 0
						FT_AVIPReadL32(pCtxt);
						FT_AVIPReadL16(pCtxt);
#endif
						FT_AVIPFlush(pCtxt, 6);
						pCtxt->SAud.BitsPerSample = FT_AVIPReadL16(pCtxt);
						/*
						 The following audio formats generated by FFmpeg requires the following section:
						  - FT_WAVE_FORMAT_MULAW
						 */
						
						if(pCtxt->SAud.AudioFormat == FT_WAVE_FORMAT_MULAW ||
								pCtxt->SAud.AudioFormat == FT_WAVE_FORMAT_ADPCM_IMA_WAV)
						{
							cbSize = FT_AVIPReadL16(pCtxt);
							if(cbSize > 0)
							{
								FT_AVIPFlush(pCtxt,cbSize);
							}
						}
						
					}
					if(FT_WAVE_FORMAT_MPEGLAYER3 == strsize)
					{
#if defined(SHOWPARSERPRINTS)
						printf("strf audio mp3 %x\n",strsize);
#endif
						/* parse mp3 header */
						pCtxt->SAud.NumChannels = FT_AVIPReadL16(pCtxt);
						pCtxt->SAud.SamplingFreq = FT_AVIPReadL32(pCtxt);
#if 0
						FT_AVIPReadL32(pCtxt);
						FT_AVIPReadL16(pCtxt);
#endif
						FT_AVIPFlush(pCtxt, 6);
						pCtxt->SAud.BitsPerSample = FT_AVIPReadL16(pCtxt);

						cbSize = FT_AVIPReadL16(pCtxt);

#if 0
						FT_AVIPReadL16(pCtxt);
						FT_AVIPReadL32(pCtxt);
						FT_AVIPReadL16(pCtxt);
						FT_AVIPReadL16(pCtxt);
						FT_AVIPReadL16(pCtxt);
#endif
						FT_AVIPFlush(pCtxt, 12);
#if defined(SHOWPARSERPRINTS)
						printf("audio mp3 %d %d %d\n",pCtxt->SAud.NumChannels,pCtxt->SAud.SamplingFreq,pCtxt->SAud.BitsPerSample);
#endif
						//cbSize = FT_AVIPReadL16(pCtxt);
					}
				}

				break;
			}
			case FT_AVI_4CC_idx1:
			{
				/* Update index entry points for seek purpose */
				FT_AVIRiff_t *pidx1;
#if defined(SHOWPARSERPRINTS)
				//printf("idx1 index detected.  Offset: %x\n", ftell(pCtxt->File) - (pnb->pEnd - pnb->pCurr));
#endif

				pidx1 = &pCtxt->Riff[pCtxt->NumRiff - 1];//need to ensure that RIFF is parsed before movi
				pidx1->indxOff = F_TELL(file) - (pnb->pEnd - pnb->pCurr);//absolute value of movi in 4 gb file
				pidx1->indxSize = ChunkSz;

				FT_AVIPFlush(pCtxt,ChunkSz);
				break;
			}
			case FT_AVI_4CC_indx:
			{
				ft_uint8_t IndexSubType=0;
				ft_uint32_t ValidSuperIndexEntries=0, ChunkID;

#if 0
				FT_AVIPReadL16(pCtxt);  //wLongsPerEntry
#endif
				FT_AVIPFlush(pCtxt, 2);
				IndexSubType=FT_AVIPRead8(pCtxt);	//bIndexSubType
#if 0
				FT_AVIPRead8(pCtxt); //bIndexType
#endif
				FT_AVIPFlush(pCtxt, 1);
				ValidSuperIndexEntries = FT_AVIPReadL32(pCtxt);  //nEntriesInUse
				ChunkID = FT_AVIPReadL32(pCtxt);	//dwChunkID
#if 0
				FT_AVIPReadL32(pCtxt);	//dwReserved[0]
				FT_AVIPReadL32(pCtxt);	//dwReserved[1]
				FT_AVIPReadL32(pCtxt);	//dwReserved[2]
#endif
				FT_AVIPFlush(pCtxt, 12);

				if(ChunkID == (FT_AVI_4CC_COMPRESSED_VIDEO | ((((ft_uint32_t)pCtxt->VideoStream) & 0xFF) << 8) | ((((ft_uint32_t)pCtxt->VideoStream) & 0xFF00) >> 8))){
					pCtxt->SVid.IndexSubType = IndexSubType;
					pCtxt->SVid.ValidSuperIndexEntries = ValidSuperIndexEntries;
					pCtxt->SVid.SuperIndexOffset = F_TELL(file) - (pnb->pEnd - pnb->pCurr);
				}
				else if(ChunkID == (FT_AVI_4CC_AUDIO | ((((ft_uint32_t)pCtxt->AudioStream) & 0xFF) << 8) | ((((ft_uint32_t)pCtxt->AudioStream) & 0xFF00) >> 8))){
					pCtxt->SAud.IndexSubType = IndexSubType;
					pCtxt->SAud.ValidSuperIndexEntries = ValidSuperIndexEntries;
					pCtxt->SAud.SuperIndexOffset = F_TELL(file) - (pnb->pEnd - pnb->pCurr);//absolute value of movi in 4 gb file
				}
				FT_AVIPFlush(pCtxt,ChunkSz - 24);
				break;
			}
			case FT_AVI_4CC_dmlh:
			{
				pCtxt->SVid.TotFrames = FT_AVIPReadL32(pCtxt);

#if defined(SHOWPARSERPRINTS)
				printf("Opendml spec. Total video frames: %u chunkSz:%x\n",pCtxt->SVid.TotFrames, ChunkSz);

#endif
				if((ChunkSz-4) > 0){
					FT_AVIPFlush(pCtxt,ChunkSz-4);
				}
				break;
			}
			case FT_AVI_4CC_JUNK:
			case FT_AVI_4CC_vprp:
			case FT_AVI_4CC_strn:
			case FT_AVI_4CC_strd:
			case FT_AVI_4CC_rec:
			case FT_AVI_4CC_isft:
			{

#if defined(SHOWPARSERPRINTS)
				printf("extra indx/junk/vprp \n");
#endif

				FT_AVIPFlush(pCtxt,ChunkSz);
				break;
			}
			default:
			{
#if defined(SHOWPARSERPRINTS)
				printf("Error default for 4cc %x\n",Next4cc);
#endif
				FT_AVIPFlush(pCtxt,ChunkSz);
				break;
			}
		}
	}

	if(pCtxt->NumMedia == 0)
	{
		Status = FT_AVIPARSER_INVALIDFILE;
	}


	return Status;

parser_error:
	return FT_AVIPARSER_UNKNOWNERR;
}


/* Set the RIFF to be played */
ft_int32_t	FT_AVIParser_SetRiff(FT_AVIParser_t *pCtxt,ft_uint8_t RiffNo)
{
	
	if(RiffNo > pCtxt->NumRiff)
	{
		return FT_AVIPARSER_OK;
	}
	
	/* Set all the parameters for current RIFF - need to add support for multiple riffs during runtime */
	pCtxt->CurrRiff = RiffNo;
	pCtxt->CurrStat.MoviOff = pCtxt->Riff[pCtxt->CurrRiff].MoviOff;
	pCtxt->CurrStat.IndxOff = pCtxt->Riff[pCtxt->CurrRiff].indxOff;  //Should have no effect on seeking, AVI 1.0 has a fixed idx1 list and Opendml AVI has a stream specific super index
	//pCtxt->CurrStat.MoviOff = 0;//current riff chunks movi offset  //original
	//pCtxt->CurrStat.IndxOff = 0;//current riff chunks idx1 offset  //original
	
	/* Initialize the Nbuffer with first video data */
	FT_AVIPFetch(pCtxt,pCtxt->Riff[pCtxt->CurrRiff].MoviOff);
	
//	printf("Aviriff set %d %x %x \n",pCtxt->CurrRiff,pCtxt->Riff[pCtxt->CurrRiff].MoviOff,pCtxt->Riff[pCtxt->CurrRiff].MoviSize);
	/* make sure all the audio/video metadata and data are flushed properly */
	
	return FT_AVIPARSER_OK;
}

ft_int32_t FT_AVIFillData(FT_AVIParser_t *pCtxt, Ft_Gpu_Hal_Context_t *pHHal, Fifo_t *pFifo, FT_RingBuffer_t *pRBuff, ft_uint32_t Data2Copy)
{
	ft_int32_t numbyteswritten = 0;
	ft_uint32_t currSz;
	ft_uint32_t wrsz;
	ft_uint32_t fResult;
#if defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM) 
	FILE* file = pCtxt->File;
#else
	FIL* file = &pCtxt->File;
#endif

	if (Data2Copy <= 0) {
		return FT_AVIPARSER_OK;
	}
	while (Data2Copy > 0) {
		numbyteswritten = 0;
		currSz = pCtxt->SNbuff.pEnd - pCtxt->SNbuff.pCurr;
		wrsz = currSz < Data2Copy ? currSz : Data2Copy;
		if (pFifo != NULL) {
			//APP_DBG("Video Fifo write: addr = %u", wrsz);
			numbyteswritten = Fifo_Write(pHHal, pFifo,pCtxt->SNbuff.pCurr, wrsz);
		} else {
			// APP_DBG("Audio Ring buffer write: addr = %u", numbyteswritten);
			FT_RingBuffer_Write(pRBuff,pCtxt->SNbuff.pCurr, wrsz, &numbyteswritten);
			if(numbyteswritten != wrsz)
			{
				printf("Error1 whole data is not copied into rb %d %d\n",wrsz,numbyteswritten);
				return FT_AVIPARSER_COPYERROR;
			}
		}
		Data2Copy -= numbyteswritten;
		//Check if we need to read data
		if (currSz == numbyteswritten) {
			F_READ(file, pCtxt->SNbuff.pStart, pCtxt->SNbuff.Length, numbyteswritten, fResult);
			if (numbyteswritten <= 0 && fResult < 0) {
				return FT_AVIPARSER_FREADERR;
			}
			pCtxt->SNbuff.pCurr = pCtxt->SNbuff.pStart;
			pCtxt->SNbuff.pEnd = pCtxt->SNbuff.pCurr + numbyteswritten;
		} else {
			pCtxt->SNbuff.pCurr += numbyteswritten;
		}
	}
	if(pFifo != NULL && pFifo->fifo_wp & 0x03)
	{
		ft_uint8_t paddlen = pFifo->fifo_wp & 0x03;
		ft_uint32_t padddata = 0;

		Fifo_Write(pHHal,pFifo,(ft_uint8_t*)&padddata,paddlen);
	}
	return FT_AVIPARSER_OK;
}

/* API to download data into ft81x input buffer directly - fifo ring buffer mechanism */
/* No avi parsing done, blindly tries to copy data from cache buffer/file into FT81x memory */
#define FT_AVIFillVdata(pCtxt, pvdp, VDatatocopy) FT_AVIFillData(pCtxt, pvdp->phalctxt, pvdp->pfifo, NULL, VDatatocopy)
//ft_int32_t FT_AVIFillVdata(FT_AVIParser_t *pCtxt,FT_VidDataPath_t *pvdp,ft_uint32_t VDatatocopy)

/* API to download data input audio input buffer - ring buffer mechanism */
/* No avi parsing done, blindly tries to copy data from cache buffer/file into FT81x memory */
#define FT_AVIFillAdata(pCtxt,padp,ADatatocopy) FT_AVIFillData(pCtxt, NULL, NULL, padp->pairb, ADatatocopy)
//ft_int32_t FT_AVIFillAdata(FT_AVIParser_t *pCtxt,FT_AudDataPath_t *padp,ft_uint32_t ADatatocopy)

/* Check for partial buffer case and fill data */
#if 0
ft_int32_t FT_AVIPVidChkFillParBuff(FT_AVIParser_t *pCtxt,FT_VidDataPath_t *pvdp)
{
	FT_RingQueue_t *prqv;
	ft_int32_t Status = FT_AVIPARSER_OK;
	Ft_Gpu_Hal_Context_t *pHHal;
	Fifo_t *pVFifo;
	static ft_uint32_t cursz=0;

	prqv = pvdp->pvirq;
	pHHal = pvdp->phalctxt;
	pVFifo = pvdp->pfifo;
	/* Check for skip frame flag from player */
	/* Copy data from parser to video ring buffer in case of partial buffer */
	if(prqv->ValidEle > 0 )
	{
		ft_int32_t VInputRingb,VDatatocopy = 0;
		FT_MetaV_t SMetaV;

		FT_RQueue_PeekELatest(prqv,&SMetaV);
		//printf("Videochk partial %x %x %x %x %x %x %x %x %x\n",SMetaV.Length,SMetaV.PartialSz,SMetaV.Flags,prqv->WriteIdx,prqv->ValidEle,Ft_Gpu_Hal_Rd32(pHHal,REG_MEDIAFIFO_READ),Ft_Gpu_Hal_Rd32(pHHal,REG_MEDIAFIFO_WRITE),Ft_Gpu_Hal_Rd32(pHHal,REG_CMD_READ),Ft_Gpu_Hal_Rd32(pHHal,REG_CMD_WRITE));
		/* In case of partial frame, first download the partial frame */
		if(SMetaV.Flags & FT_AVIPMETAD_PARTIALBUFFER)
		{

#if defined(SHOWPARSERPRINTS)
		//printf("partial Video chunk. L:%u P:%u FS:%u\n",SMetaV.Length, SMetaV.PartialSz, Fifo_GetFreeSpace(pHHal,pVFifo));
#endif
			Status = FT_AVIPARSER_PARTIALBUFFERUSECASE;

			/* Get the available free size in FT81x */
			VInputRingb = Fifo_GetFreeSpace(pHHal,pVFifo);
			//VInputRingb = FT_AVIPARSER_TESTSTUB_VIDBUFFMAXSZ;//hack for video fake sync
			//printf("Videochk partial %x %x %x %x\n",SMetaV.Length,SMetaV.PartialSz,SMetaV.Flags,VInputRingb);
			/* Partial buffer case where input buffer has no space, so the chunk size present in meta should be used */
			VDatatocopy = SMetaV.Length - SMetaV.PartialSz;
			if(VInputRingb < VDatatocopy)
			{
				/* again partial scenarios, so copy partial and re update the metadata element */
				VDatatocopy = VInputRingb;
				SMetaV.PartialSz += VInputRingb;
				//Status = FT_AVIPARSER_PARTIALBUFFERUSECASE;  //added
#if defined(SHOWVIDEOPARSINGPRINTS)
				printf("PV. P:%u S:%u C:%u.\n", SMetaV.PartialSz, SMetaV.Length, SMetaV.Frame);
#endif
			}
			else
			{
				SMetaV.Flags &= (~FT_AVIPMETAD_PARTIALBUFFER);
				SMetaV.PartialSz = 0;
				//printf("FV(1): %u  \n",SMetaV.Length);
				//Status = FT_AVIPARSER_OK; //added
#if defined(SHOWVIDEOPARSINGPRINTS)
				printf("PV-LV.\n");
#endif
			}
			if(VDatatocopy > 0)
			{
				Status = FT_AVIFillVdata(pCtxt, pvdp,VDatatocopy);
				if(Status != FT_AVIPARSER_OK)
				{
					return Status;
				}
			}

			SMetaV.DeviceWrtPtr = pVFifo->fifo_wp;
			FT_RQueue_WriteEInplace(prqv,&SMetaV);


			/* return partial buffer usecase or error from fillvdata */
			//return Status;//added
			return FT_AVIPARSER_PARTIALBUFFERUSECASE;  //original
		}
	}
	return Status;
}

/* Check for partial buffer case and fill data */
ft_int32_t FT_AVIPAudChkFillParBuff(FT_AVIParser_t *pCtxt,FT_AudDataPath_t *padp)
{
	FT_RingQueue_t *prqa;
	ft_int32_t Status = FT_AVIPARSER_OK;


	prqa = padp->pairq;

	/* Copy data from parser to audio input ring buffer in case of partial buffer */
	/* Check for skip frame flag settings */
	if(prqa->ValidEle > 0 )
	{
		ft_int32_t AInputRingb,ADatatocopy = 0;
		FT_MetaA_t SMetaA;

		FT_RQueue_PeekELatest(prqa,&SMetaA);
		//printf("Audiochk partial %x %x %x %x %x\n",SMetaA.Length,SMetaA.PartialSz,SMetaA.Flags,prqa->WriteIdx,prqa->ValidEle);
		/* In case of partial frame, first download the partial frame */
		if(SMetaA.Flags & FT_AVIPMETAD_PARTIALBUFFER)
		{
#if defined(SHOWPARSERPRINTS)
		//printf("partial audio chunk. L:%u P:%u FS:%u\n", SMetaA.Length, SMetaA.PartialSz, FT_RingBuffer_FreeSpace(padp->pairb));
#endif
			Status = FT_AVIPARSER_PARTIALBUFFERUSECASE; //original

			/* Get the free size from pcm/mp3 codec */
			//printf("Audiochk partial %x %x %x %x %x\n",SMetaA.Length,SMetaA.PartialSz,SMetaA.Flags,prqa->WriteIdx,prqa->ValidEle);
			AInputRingb = FT_RingBuffer_FreeSpace(padp->pairb);
			//AInputRingb = FT_AVIPARSER_TESTSTUB_AUDBUFFMAXSZ;//hack for audio fake sync

			/* Partial buffer case where input buffer has no space, so the chunk size present in meta should be used */
			ADatatocopy = SMetaA.Length - SMetaA.PartialSz;
			if(AInputRingb < ADatatocopy)
			{
				/* again partial scenarios, so copy partial and re update the metadata element */
				ADatatocopy = AInputRingb;
				SMetaA.PartialSz += ADatatocopy;

				//Status = FT_AVIPARSER_PARTIALBUFFERUSECASE; //added
			}
			else
			{
				SMetaA.Flags &= (~FT_AVIPMETAD_PARTIALBUFFER);
				SMetaA.PartialSz = 0;
				//printf("FA(1):%u  \n",SMetaA.Length);
				//Status = FT_AVIPARSER_OK; //added
			}
			if(ADatatocopy > 0)
			{
				Status = FT_AVIFillAdata(pCtxt, padp,ADatatocopy);
				if(Status != FT_AVIPARSER_OK)
				{
					printf("Audparser filladata error %d\n",Status);
					return Status;
				}

			}

			Status = FT_RQueue_WriteEInplace(prqa,&SMetaA);
			if(FT_AVIPARSER_OK != Status)
			{
				printf("Audparser error %d\n",Status);
			}

			/* return after partial chunk */
			return FT_AVIPARSER_PARTIALBUFFERUSECASE; //original
			//return Status; //added
		}
	}
	
	return Status;
}
#endif //if 0

ft_int32_t FT_AVIPVidFillChunk(FT_AVIParser_t *pCtxt,FT_VidDataPath_t *pvdp,ft_uint32_t VBuffSz, int isPartial)
{
	ft_uint32_t VidHdr;
	FT_MetaV_t SMetaV;
	ft_int32_t VDatatocopy;
	FT_RingQueue_t *prqv;
	FT_AviV_t *paviv;
	Fifo_t *pVFifo;
	Ft_Gpu_Hal_Context_t *pHHal;
	ft_int32_t Status = FT_AVIPARSER_OK;

	/* video specific local pointers */
	prqv = pvdp->pvirq;
	paviv = &pCtxt->SVid;

	/* FT81x specific pointers */
	pHHal = pvdp->phalctxt;
	pVFifo = pvdp->pfifo;

	//FT_MetaV_Init(&SMetaV);
	memset(&SMetaV, 0, sizeof SMetaV);
	if (!isPartial) {
		SMetaV.Frame = pCtxt->curFrame-1;
		/* Check for skip frames in case of video */
		if ((pCtxt->Flags & (FT_AVIPARSER_DROP_VIDEO_ONE_FRAME | FT_AVIPARSER_DROPVIDEO | FT_AVIPARSER_DROPALLMEDIA | FT_AVIPARSER_FAKESYNC)))
		{
			//printf("Videochk flush %x %x \n",chksz,pCtxt->Flags);
			FT_AVIPFlush(pCtxt,VBuffSz);//fake sync

			SMetaV.DeviceWrtPtr = pVFifo->fifo_wp;
			SMetaV.Flags |= (FT_AVIPMETAD_KEYFRAME | FT_AVIPMETAD_NEWFRAME);
			SMetaV.Pts = (paviv->CurrFrame*paviv->TimeScale*1000)/paviv->DataRate;//calculation of timestamp based on the fileformat - player needs to change this according to the playback timestamp
			SMetaV.Curr = pVFifo->fifo_wp;
			SMetaV.Length = 0;

			FT_RQueue_WriteE(prqv,&SMetaV);//incorrect in case of partial frames present in chunk
			paviv->CurrFrame++;//incorrect in case of partial frames present in chunk
			if (pCtxt->Flags & FT_AVIPARSER_DROP_VIDEO_ONE_FRAME)
				pCtxt->Flags &= (~FT_AVIPARSER_DROP_VIDEO_ONE_FRAME);
			return FT_AVIPARSER_OK;//do not insert any metadata
		}
	}
	if (isPartial && prqv->ValidEle <= 0) {
		return FT_AVIPARSER_OK;
	}
	VDatatocopy = 0;
	if(isPartial || VBuffSz > 0)
	{
		FT_NBuffer_t *pnbuff;
		ft_uint32_t VInputRingb;

		pnbuff = &pCtxt->SNbuff;
		if (isPartial) {
			FT_RQueue_PeekELatest(prqv,&SMetaV);
			if (!(SMetaV.Flags & FT_AVIPMETAD_PARTIALBUFFER))
				return FT_AVIPARSER_OK;
			VBuffSz = SMetaV.Length - SMetaV.PartialSz;
			/*printf("sz: %d\n\r", VBuffSz);*/
			Status = FT_AVIPARSER_PARTIALBUFFERUSECASE;
		} else {
			/* In case of video chunk - hack, check for MJPEG header in the chunk, if not found then flag as partial frame */
			VidHdr = (pnbuff->pCurr[0]) | ((pnbuff->pCurr[1]) << 8);//need to check boundary of ring buffer

			/* hack for type of frame - whole frame or partial by checking the jpg header */
			if(FT_AVIP_JPGFRAMEHDR == VidHdr)
			{
				SMetaV.Flags |= (FT_AVIPMETAD_KEYFRAME | FT_AVIPMETAD_NEWFRAME);
				SMetaV.Pts = (paviv->CurrFrame*paviv->TimeScale*1000)/paviv->DataRate;//calculation of timestamp based on the fileformat - player needs to change this according to the playback timestamp
				SMetaV.Curr = pVFifo->fifo_wp;
				SMetaV.Length = VBuffSz;
				//printf("Video movi chk %x %x %x %d\n",movichk,chksz,VidHdr,SMetaV.Pts);
				/* reset the partial buffer flag of the previous chunk if any */
			}
			else
			{
				/* Partial frame, fetch the earlier queue element and set the partial frame flag */
				//printf("Video movi chk partial %x %x %x\n",movichk,chksz,VidHdr);
				FT_RQueue_PeekELatest(prqv,&SMetaV);
				SMetaV.Length += VBuffSz;
				SMetaV.Flags |= (FT_AVIPMETAD_PARTIALFRAME);
				SMetaV.Flags &= ((~FT_AVIPMETAD_NEWFRAME));//chunks other then first chunk of the frame
			}
		}

		/* get the available data in FT81x */
		VInputRingb = Fifo_GetFreeSpace(pHHal,pVFifo);
		//VInputRingb = FT_AVIPARSER_TESTSTUB_VIDBUFFMAXSZ;//hack for fake sync

		/* Download data into FT81x */
		if(VInputRingb < VBuffSz)
		{
			/* again partial scenarios, so copy partial and re update the metadata element */

			VDatatocopy = VInputRingb;
			SMetaV.PartialSz += VDatatocopy;
			SMetaV.Flags |= FT_AVIPMETAD_PARTIALBUFFER;
			//printf("Video fstpar chk %x %x %x %x\n",movichk,chksz,VInputRingb,SMetaV.Flags);
		}
		else
		{
			VDatatocopy = VBuffSz;
			SMetaV.PartialSz = 0;
			SMetaV.Flags &= (~FT_AVIPMETAD_PARTIALBUFFER);
		}
	}
	else
	{
		SMetaV.Length = 0;
		SMetaV.PartialSz = 0;
		SMetaV.Pts = ((paviv->CurrFrame*paviv->TimeScale*1000)/paviv->DataRate);//calculation of timestamp based on the fileformat - player needs to change this according to the playback timestamp
		SMetaV.Curr = pVFifo->fifo_wp;
		SMetaV.Flags = FT_AVIPMETAD_NEWFRAME;
	}
	//printf("bfrvfill %x %x\n",Ft_Gpu_Hal_Rd32(pvdp->phalctxt,REG_MEDIAFIFO_READ),Ft_Gpu_Hal_Rd32(pvdp->phalctxt,REG_MEDIAFIFO_WRITE));
	if (VDatatocopy > 0) {
		Status = FT_AVIFillVdata(pCtxt,pvdp,VDatatocopy);
		if (Status != FT_AVIPARSER_OK)
			return Status;
	}
	//FT_AVIPFlush(pCtxt,VBuffSz);//fake sync

	SMetaV.DeviceWrtPtr = pVFifo->fifo_wp;
	if (!isPartial && (SMetaV.Flags & (FT_AVIPMETAD_NEWFRAME)))
	{
		FT_RQueue_WriteE(prqv,&SMetaV);
		//printf("Video meta %x %x %x %x %x %x %x %d %d %d\n",SMetaV.Length,SMetaV.PartialSz,SMetaV.Pts,SMetaV.Curr,SMetaV.Flags,Ft_Gpu_Hal_Rd32(pHHal,REG_MEDIAFIFO_READ),Ft_Gpu_Hal_Rd32(pHHal,REG_MEDIAFIFO_WRITE),prqv->ReadIdx,prqv->WriteIdx,prqv->ValidEle);
		paviv->CurrFrame++;
	}
	else
	{
		FT_RQueue_WriteEInplace(prqv,&SMetaV);
	}

	if (isPartial) {
		return FT_AVIPARSER_PARTIALBUFFERUSECASE;
	}
	return Status;
}

ft_int32_t FT_AVIPAudFillChunk(FT_AVIParser_t *pCtxt,FT_AudDataPath_t *padp,ft_uint32_t ABuffSz, int isPartial)
{
	ft_uint32_t AInputRingb,ADatatocopy = 0;
	FT_RingQueue_t *prqa;
	FT_RingBuffer_t *prba;
	FT_AviA_t *pavia;
	FT_MetaA_t SMetaA;
	ft_int32_t Status = FT_AVIPARSER_OK;

	/* as of now only video buffer size is loaded */
	prqa = padp->pairq;
	prba = padp->pairb;
	pavia = &pCtxt->SAud;

	//FT_MetaA_Init(&SMetaA);
	memset(&SMetaA, 0, sizeof SMetaA);
	if (!isPartial) {
		SMetaA.Frame = pCtxt->curFrame > 0 ? pCtxt->curFrame-1 : 0;
		/* Check for skip frames in case of video */
		if((pCtxt->Flags & (FT_AVIPARSER_DROPAUDIO | FT_AVIPARSER_DROPALLMEDIA | FT_AVIPARSER_FAKESYNC)))
		{
			FT_AVIPFlush(pCtxt,ABuffSz);//fake sync
			FT_RQueue_WriteE(prqa,&SMetaA);//incorrect in case of partial frames present in chunk
			pavia->CurrFrame++;//incorrect in case of partial frames present in chunk
			//printf("Audiochk flush %x %x \n",ABuffSz,pCtxt->Flags);
			return FT_AVIPARSER_OK;//do not insert any metadata
		}
	}

	if (isPartial && prqa->ValidEle <= 0) {
		return FT_AVIPARSER_OK;
	}
	printf("Audio time stamp: %u %u %u %u %u\n",(pavia->CurrFrame*pavia->TimeScale*pavia->packet_size*1000)/pavia->DataRate, pavia->CurrFrame, pavia->TimeScale, pavia->packet_size, pavia->DataRate);
//	printf("%u %u %u %u\n",pavia->CurrFrame, pavia->TimeScale, pavia->packet_size, pavia->DataRate);
	if(isPartial || ABuffSz > 0)
	{

		if (isPartial) {
			FT_RQueue_PeekELatest(prqa,&SMetaA);
			if (!(SMetaA.Flags & FT_AVIPMETAD_PARTIALBUFFER))
				return FT_AVIPARSER_OK;
			ABuffSz = SMetaA.Length - SMetaA.PartialSz;
			Status = FT_AVIPARSER_PARTIALBUFFERUSECASE; //original
		} else {
			/* hack for pcm and mp3 assuming that chunk is independently decodable */
			SMetaA.Flags |= (FT_AVIPMETAD_KEYFRAME | FT_AVIPMETAD_NEWFRAME);
			SMetaA.Pts = ((pavia->CurrFrame*pavia->TimeScale*1000)/pavia->DataRate);//calculation of timestamp based on the fileformat - player needs to change this according to the playback timestamp
			SMetaA.Curr = prba->Write;
			SMetaA.Length = ABuffSz;
		}
		//printf("Audio movi chk %x %x %d\n",movichk,chksz,SMetaA.Pts);

		/* get the available data in FT81x */
		AInputRingb = FT_RingBuffer_FreeSpace(prba);  //original
		//AInputRingb = FT_AVIPARSER_TESTSTUB_AUDBUFFMAXSZ;//hack for audio fake sync

		/* Download data into FT81x */
		if(AInputRingb < ABuffSz)
		{
			/* again partial scenarios, so copy partial and re update the metadata element */
			ADatatocopy = AInputRingb;
			SMetaA.PartialSz += ADatatocopy;
			SMetaA.Flags |= FT_AVIPMETAD_PARTIALBUFFER;
		}
		else
		{
			ADatatocopy = ABuffSz;
			SMetaA.PartialSz = 0;
			SMetaA.Flags &= (~FT_AVIPMETAD_PARTIALBUFFER);
		}
	}
	else
	{
		SMetaA.Length = 0;
		SMetaA.PartialSz = 0;
		//in case of pcm the packet_size is chunk size
		SMetaA.Pts = ((pavia->CurrFrame*pavia->TimeScale*1000)/pavia->DataRate);//calculation of timestamp based on the fileformat - player needs to change this according to the playback timestamp
		SMetaA.Curr = prba->Write;
		SMetaA.Flags = FT_AVIPMETAD_NEWFRAME;
	}
	if (ADatatocopy > 0) {
		//APP_DBG("FT_AVIFillAdata copying audio buffer size %d", ADatatocopy);
		Status = FT_AVIFillAdata(pCtxt, padp,ADatatocopy);
		if (Status != FT_AVIPARSER_OK)
			return Status;
	}
	//FT_AVIPFlush(pCtxt,ABuffSz);//fake sync
	if(!isPartial && (SMetaA.Flags & (FT_AVIPMETAD_NEWFRAME)))
	{
		//printf("Audio chk write %x %x %x %x\n",SMetaA.Length,SMetaA.PartialSz,ADatatocopy,SMetaA.Flags);
		FT_RQueue_WriteE(prqa,&SMetaA);
		//pavia->CurrFrame++;
		pavia->CurrFrame += SMetaA.Length;
	}
	else
	{
		/* In case of partial buffer and partial frame - should not hit partial frame in audio case */
		FT_RQueue_WriteEInplace(prqa,&SMetaA);
	}
	if (isPartial) {
		return FT_AVIPARSER_PARTIALBUFFERUSECASE;
	}
	return Status;
}

//fill one chunk/frame for all the media - need to ensure 1 frame worth of data is available for each of the media
ft_int32_t FT_AVIParser_FillChunk(FT_AVIParser_t *pCtxt,FT_VidDataPath_t *pvdp,FT_AudDataPath_t *padp)
{
	FT_AVIPARSER_STATUS Status = FT_AVIPARSER_OK;
	volatile ft_uint32_t movichk,chksz = 0,VDatatocopy;
	FT_RingQueue_t *prqa,*prqv;
	FT_RingBuffer_t *prba;
	FT_AviV_t *paviv;
	FT_AviA_t *pavia;
	
#if defined(SHOWPARSERPRINTS)
	printf("FillChunk %d %d\n\r", pCtxt->CurrStat.MoviOff, pCtxt->Riff[pCtxt->CurrRiff].MoviSize);
#endif
	/* Each time check for the end of movi chunk */
	/* Check if parser is in play state or avi header state */
	if((pCtxt->CurrStat.MoviOff >= (pCtxt->Riff[pCtxt->CurrRiff].MoviSize + pCtxt->Riff[pCtxt->CurrRiff].MoviOff)))
	{
//		printf("riffmoving %x %x %x %x\n",pCtxt->CurrRiff,pCtxt->CurrStat.MoviOff,pCtxt->Riff[pCtxt->CurrRiff].MoviSize, pCtxt->Riff[pCtxt->CurrRiff].MoviOff);
		/* Roll over of riff chunk */
		pCtxt->CurrRiff++;
		if(pCtxt->CurrRiff >= pCtxt->NumRiff)
		{
//			printf("Riffeof %x %x %x %x\n",pCtxt->CurrRiff,pCtxt->CurrStat.MoviOff,pCtxt->Riff[pCtxt->CurrRiff].MoviSize, pCtxt->Riff[pCtxt->CurrRiff].MoviOff);
			pCtxt->CurrRiff = pCtxt->NumRiff;//protection
			return FT_AVIPARSER_EOF;
		}
		/* Fetch new data into local buffer  */
		FT_AVIParser_SetRiff(pCtxt,pCtxt->CurrRiff);
	}
	
	/* as of now only video buffer size is loaded */	
	prqa = padp->pairq;
	prba = padp->pairb;
	pavia = &pCtxt->SAud;
	
	/* video specific local pointers */
	prqv = pvdp->pvirq;
	paviv = &pCtxt->SVid;
	
	/* Check for all the control flags of parser */
	/* Check for partial flag present in the current queue element */
	if((prqv->ValidEle >= prqv->TotEle) || (prqa->ValidEle >= prqa->TotEle))
	{
#if defined(SHOWPARSERPRINTS)
		//printf("parchk exceed valideles %d %d %d %d \n",prqv->ValidEle,prqv->TotEle,prqa->ValidEle,prqa->TotEle);
#endif
		/* Exceeded the limit */		
		return FT_AVIPARSER_BQEFULL;
	}

	//parse any partial video chunks
	parsingVideo = 1;
	//Merge PartialFill to one function
	Status = (FT_AVIPARSER_STATUS)FT_AVIPVidFillChunk(pCtxt,pvdp,0,1);
	parsingVideo = 0;

	if(Status == FT_AVIPARSER_PARTIALBUFFERUSECASE)   ///
	{
		return FT_AVIPARSER_OK;
	}

	//parse any partial audio chunks
	parsingAudio = 1;
	Status = (FT_AVIPARSER_STATUS)FT_AVIPAudFillChunk(pCtxt,padp, 0, 1);

	parsingAudio = 0;
	#if 1
	if(Status == FT_AVIPARSER_PARTIALBUFFERUSECASE)
	{
		return FT_AVIPARSER_OK;
	}
	#else  //added
	//if(Status == FT_AVIPARSER_PARTIALBUFFERUSECASE)  
	if(Status != FT_AVIPARSER_OK)
	{
		return Status;
	}
	#endif


	
	/* Parse a new chunk */
	/*printf("Parchk %d \n", f_tell(&pCtxt->File));*/
	movichk = FT_AVIPReadL32(pCtxt);
	if (FT_AVIPARSER_EOF == movichk) {
		return FT_AVIPARSER_EOF;
	}
	chksz = FT_AVIPReadL32(pCtxt);
	
	/* hack for any video convertor */
	if(chksz & 0x00000001)
	{
		//printf("Chunk is not multiple of 2. movichk:%x, chksz:%x ",movichk,chksz);
		chksz = chksz + 1;
	}

	/* Stats maintained by parser - make sure to take care movioff during seek implementation */
	pCtxt->CurrStat.MoviOff += chksz + 8;//two dwords from the above
#if defined(SHOWPARSERPRINTS)
	printf("Parchk %d %x %x \n", pCtxt->CurrStat.MoviOff, movichk,chksz);
#endif

	/* Check for audio or video or text/midi chunks */

	if(movichk == (FT_AVI_4CC_COMPRESSED_VIDEO | ((((ft_uint32_t)pCtxt->VideoStream) & 0xFF) << 8) | ((((ft_uint32_t)pCtxt->VideoStream) & 0xFF00) >> 8))){
		//timer_profilecountstart(&timercount,&timervalue);
#if defined(SHOWPARSERPRINTS)
		//printf("Video chunk.\n");
#endif
		pCtxt->curFrame++;
		parsingVideo = 1;
		Status = (FT_AVIPARSER_STATUS)FT_AVIPVidFillChunk(pCtxt,pvdp,chksz,0);
		parsingVideo = 0;
	}

	/*else if(movichk == (FT_AVI_4CC_UNCOMPRESSED_VIDEO | ((((ft_uint32_t)pCtxt->VideoStream) & 0xFF) << 8) | ((((ft_uint32_t)pCtxt->VideoStream) & 0xFF00) >> 8))){
		Status = FT_AVIPFlush(pCtxt,chksz);
	}*/
#if 1
	//First integration to Cleo, no audio now!
	else if(!(pCtxt->Flags & FT_AVIPARSER_DROPAUDIO) &&
			movichk == (FT_AVI_4CC_AUDIO | ((((ft_uint32_t)pCtxt->AudioStream) & 0xFF) << 8) | ((((ft_uint32_t)pCtxt->AudioStream) & 0xFF00) >> 8))){
		//printf("Audio chunk size: %u , movioffset:%u.\n",chksz,pCtxt->CurrStat.MoviOff);
#if defined(SHOWPARSERPRINTS)
		//printf("Audio chunk.\n");
#endif
		parsingAudio = 1;
		Status = (FT_AVIPARSER_STATUS)FT_AVIPAudFillChunk(pCtxt,padp,chksz,0);
		parsingAudio = 0;
	}
#endif
	else
	{
#if defined(SHOWPARSERPRINTS)
		printf("!AV chk %x %x\n",movichk,chksz);
#endif
		/* Flush this chunk and go for another chunk */
		Status = (FT_AVIPARSER_STATUS)FT_AVIPFlush(pCtxt,chksz);
	}

	/* Copy data from parser to input audio buffer */	
	return Status;	
}


//free up every thing and exit
ft_int32_t	FT_AVIParser_Exit(FT_AVIParser_t *pCtxt)
{
	/* CLose any hanldes or context opened in avi parser init or steady state */
#if defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM) 
	FILE* file = pCtxt->File;
#else
	FIL* file = &pCtxt->File;
#endif
	F_CLOSE(file);
	pCtxt->Flags = 0;


	return FT_AVIPARSER_OK;
}


typedef struct AVI_CONTAINER_IDX1_ENTRY_t{
	ft_uint32_t dwChunkId;
    ft_uint32_t dwFlags;
    ft_uint32_t dwOffset;
    ft_uint32_t dwSize;
#if defined(FT900_PLATFORM)
} __attribute__((packed)) AVI_CONTAINER_IDX1_ENTRY;
#else
} AVI_CONTAINER_IDX1_ENTRY;
#endif
extern ft_uint32_t FT_AVIParser_Seek_To(ft_uint8_t percent, FT_AVIParser_t *pParser);

ft_uint32_t FT_AVIParser_Seek_To(ft_uint8_t percent, FT_AVIParser_t *pParser)
{
	ft_uint32_t fourcc, approximateFrame = (pParser->SVid.TotFrames * percent) / 100;
	FT_AVIRiff_t *pidx1 = &pParser->Riff[0/*pParser->NumRiff - 1*/];
	ft_int32_t fResult, bytesRead, seekFileOffset, idx1Offset = ((pidx1->indxSize * percent) / 1600) * 16 + 8, finalOffset;
	ft_int32_t TotalDuration, chunkIndexOffset, offset = pParser->SVid.SuperIndexOffset + 12, value, frames = 0, prevFrameCount = 0;
	ft_int32_t RelativeOffset, EntriesInUse;
	ft_int8_t i, lengthPerEntry;
	ft_int16_t length;
	ft_uint32_t ret = 0;

	printf("SeekTo(): per=%d %d %d %d %d", percent, pParser->SVid.TotFrames, pidx1->indxOff, pidx1->indxSize, idx1Offset);
#if defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM) 
	if(pParser->VideoStreamFound && !(pParser->Flags & FT_AVIPARSER_DROPVIDEO) && pParser->SVid.ValidSuperIndexEntries) {
		offset=pParser->SVid.SuperIndexOffset+12;
		for(i=0; i<pParser->SVid.ValidSuperIndexEntries; i++, offset+=16) {
			prevFrameCount = frames;
			fseek(pParser->File, offset, SEEK_SET);
			fread(&value, 1, 4, pParser->File);
			frames += value;
			if(approximateFrame < frames) {
				offset -= 12;
				break;
			}
		}
		fseek(pParser->File, offset, SEEK_SET);
		fread(&value, 1, 4, pParser->File);
		if(fseek(pParser->File, (long int)value, SEEK_SET) != 0) {
			printf("Unable to seek.\n");
		}
		chunkIndexOffset = value;

		fseek(pParser->File, chunkIndexOffset+12, SEEK_SET);
		fread(&value, 1, 4, pParser->File);
		EntriesInUse = value;
		fread(&value, 1, 4, pParser->File);
		fread(&value, 1, 4, pParser->File);	//64bit size but take only 32bit for fatfs
		RelativeOffset = value;
		if(pParser->SVid.IndexSubType == 0) { //standard index
			lengthPerEntry = 8;
		}
		else {
			lengthPerEntry = 12;
		}
		fseek(pParser->File, chunkIndexOffset+40+(approximateFrame - prevFrameCount)*(lengthPerEntry), SEEK_SET);

		fread(&value, 1, 4, pParser->File);
		if(fseek(pParser->File, RelativeOffset + value - 8, SEEK_SET) != 0) {
			printf("Unable to see. Error encountered during seeking.\n");
		}

	}
	else if(pParser->AudioStreamFound && !(pParser->Flags & FT_AVIPARSER_DROPAUDIO) && pParser->SAud.ValidSuperIndexEntries) {
		offset=pParser->SAud.SuperIndexOffset+12;
		for(i=0; i<pParser->SAud.ValidSuperIndexEntries; i++, offset+=16) {
			prevFrameCount = frames;
			fseek(pParser->File, offset, SEEK_SET);
			fread(&value, 1, 4, pParser->File);
			frames += value;
			if(approximateFrame < frames) {
				break;
			}
		}
		fseek(pParser->File, offset-12, SEEK_SET);
		fread(&value, 1, 4, pParser->File);
		fseek(pParser->File, value, SEEK_SET);
		chunkIndexOffset = value;

		fseek(pParser->File, chunkIndexOffset+12, SEEK_SET);
		fread(&value, 1, 4, pParser->File);
		EntriesInUse = value;
		fread(&value, 1, 4, pParser->File);
		fread(&value, 1, 4, pParser->File); //64bit size but take only 32bit for fatfs
		RelativeOffset = value;
		if(pParser->SAud.IndexSubType == 0) { //standard index
			lengthPerEntry = 8;
		}
		else {
			lengthPerEntry = 12;
		}
		fseek(pParser->File, chunkIndexOffset+40+(approximateFrame - prevFrameCount)*(lengthPerEntry), SEEK_SET);

		fread(&value, 1, 4, pParser->File);
		fseek(pParser->File, RelativeOffset + value - 8, SEEK_SET);
	}
	else {
		fseek(pParser->File, pidx1->indxOff + idx1Offset, SEEK_SET);
		fread(&fourcc, 1, 4, pParser->File);

		fResult = fseek(pParser->File, pidx1->MoviOff + fourcc - 4, SEEK_SET);
		if(fResult < 0) {
			printf("File operation error: unable to seek in file.\n");
		}
		finalOffset = fourcc;
		bytesRead = fread(&fourcc, 1, 4, pParser->File);
		if (bytesRead != 4) {
			printf("File operation error: unable to read file content.\n");
		}

		//check to see whether the offset is relative to 'movi' chunk
		if((fourcc & 0xFFFF3030) == FT_AVI_4CC_COMPRESSED_VIDEO || (fourcc & 0xFFFF3030) == FT_AVI_4CC_UNCOMPRESSED_VIDEO || (fourcc & 0xFFFF3030) == FT_AVI_4CC_AUDIO || (fourcc & 0xFFFF3030) == FT_AVI_4CC_TEXT || (fourcc & 0xFFFF3030) == FT_AVI_4CC_PALETTE_CHANGE) {
			fseek(pParser->File, pidx1->MoviOff + finalOffset - 4, SEEK_SET);
			printf("Seeking relative to movi chunk.\n");
			return;
		}
		else {
			fResult = fseek(pParser->File, fourcc, SEEK_SET);
			if(fResult < 0) {
				printf("File operation error: unable to seek in file.\n");
			}
			finalOffset = fourcc;
			bytesRead = fread(&fourcc, 1, 4, pParser->File);
			if (bytesRead != 4) {
				printf("File operation error: unable to read file content.\n");
			}
			if((fourcc & 0xFFFF3030) == FT_AVI_4CC_COMPRESSED_VIDEO || (fourcc & 0xFFFF3030) == FT_AVI_4CC_UNCOMPRESSED_VIDEO || (fourcc & 0xFFFF3030) == FT_AVI_4CC_AUDIO || (fourcc & 0xFFFF3030) == FT_AVI_4CC_TEXT || (fourcc & 0xFFFF3030) == FT_AVI_4CC_PALETTE_CHANGE) {
				fseek(pParser->File, finalOffset, SEEK_SET);
				printf("Seeking relative to starting offset.\n");
				return;
			}
			else {
				printf("Error encountered while seeking in the file.\n");
			}
		}
	}
	ret = ftell(pParser->File);
#elif defined(FT900_PLATFORM)

#define ftell f_tell
#define fseek f_lseek

	if (pParser->VideoStreamFound && !(pParser->Flags & FT_AVIPARSER_DROPVIDEO) && pParser->SVid.ValidSuperIndexEntries) {
		offset = pParser->SVid.SuperIndexOffset + 12;
		for (i = 0; i < pParser->SVid.ValidSuperIndexEntries; i++, offset += 16) {
			prevFrameCount = frames;
			fseek(&pParser->File, offset);
			f_read(&pParser->File, &value, 4, &bytesRead);
			frames += value;
			if (approximateFrame < frames) {
				break;
			}
		}
		fseek(&pParser->File, offset - 12);
		f_read(&pParser->File, &value, 4, &bytesRead);
		if (fseek(&pParser->File, value) != FR_OK) {
			printf("Unable to seek to movi index: %x.\n", value);
		}
		chunkIndexOffset = value;

		fseek(&pParser->File, chunkIndexOffset + 12);
		f_read(&pParser->File, &value, 4, &bytesRead);
		EntriesInUse = value;
		f_read(&pParser->File, &value, 4, &bytesRead);
		f_read(&pParser->File, &value, 4, &bytesRead);		//64bit size but take only 32bit for fatfs
		RelativeOffset = value;
		if (pParser->SVid.IndexSubType == 0) { //standard index
			lengthPerEntry = 8;
		} else {
			lengthPerEntry = 12;
		}
		fseek(&pParser->File, chunkIndexOffset + 40 + (approximateFrame - prevFrameCount) * (lengthPerEntry));
		//printf("frame: %u type:%u ao:%x ro:%x ", approximateFrame, pParser->SVid.IndexSubType, chunkIndexOffset + 40 + (approximateFrame - prevFrameCount) * (lengthPerEntry), RelativeOffset);
		f_read(&pParser->File, &value, 4, &bytesRead);
		//printf("dwOffset:%x\n", value);
		fseek(&pParser->File, RelativeOffset + value - 8);
		//printf("Final seek offset:%u\n", RelativeOffset + value - 8);

	} else if (pParser->AudioStreamFound && !(pParser->Flags & FT_AVIPARSER_DROPAUDIO) && pParser->SAud.ValidSuperIndexEntries) {
		offset = pParser->SAud.SuperIndexOffset + 12;
		for (i = 0; i < pParser->SAud.ValidSuperIndexEntries; i++, offset += 16) {
			prevFrameCount = frames;
			fseek(&pParser->File, offset);
			f_read(&pParser->File, &value, 4, &bytesRead);
			frames += value;
			if (approximateFrame < frames) {
				break;
			}
		}
		fseek(&pParser->File, offset - 12);
		f_read(&pParser->File, &value, 4, &bytesRead);
		fseek(&pParser->File, value);
		chunkIndexOffset = value;

		fseek(&pParser->File, chunkIndexOffset + 12);
		f_read(&pParser->File, &value, 4, &bytesRead);
		EntriesInUse = value;
		f_read(&pParser->File, &value, 4, &bytesRead);
		f_read(&pParser->File, &value, 4, &bytesRead);		//64bit size but take only 32bit for fatfs
		RelativeOffset = value;
		if (pParser->SAud.IndexSubType == 0) { //standard index
			lengthPerEntry = 8;
		} else {
			lengthPerEntry = 12;
		}
		fseek(&pParser->File, chunkIndexOffset + 40 + (approximateFrame - prevFrameCount) * (lengthPerEntry));

		f_read(&pParser->File, &value, 4, &bytesRead);
		fseek(&pParser->File, RelativeOffset + value - 8);

	} else {
		AVI_CONTAINER_IDX1_ENTRY idx1Entry;
		volatile int newPos;

		fseek(&pParser->File, pidx1->indxOff + idx1Offset - 8);
		f_read(&pParser->File, &idx1Entry, sizeof(idx1Entry), &bytesRead);

		if (bytesRead != sizeof(idx1Entry)) {
			FT_Error("Could not read idx1 entry! %d", bytesRead);
		}
		/*FT_Debug("At offset: %d 0x%x 0x%x %d %d", pidx1->indxOff + idx1Offset,
				idx1Entry.dwChunkId, idx1Entry.dwFlags, idx1Entry.dwOffset, idx1Entry.dwSize);*/
		//Assume that offset to beginning of file, not movi
		fResult = fseek(&pParser->File, idx1Entry.dwOffset);

		if (fResult != FR_OK) {
			printf(((char [18]){"%d: Error seek.\n"}), __LINE__);
		}
		finalOffset = idx1Entry.dwOffset;
		f_read(&pParser->File, &fourcc, 4, &bytesRead);
		if (bytesRead != 4) {
			printf(((char [18]){"%d: Error read.\n"}), __LINE__);
		}

		//check to see whether the offset is relative to 'movi' chunk
		if ((fourcc & 0xFFFF3030) == FT_AVI_4CC_COMPRESSED_VIDEO || (fourcc & 0xFFFF3030) == FT_AVI_4CC_UNCOMPRESSED_VIDEO || (fourcc & 0xFFFF3030) == FT_AVI_4CC_AUDIO || (fourcc & 0xFFFF3030) == FT_AVI_4CC_TEXT || (fourcc & 0xFFFF3030) == FT_AVI_4CC_PALETTE_CHANGE) {
			//pParser->CurrStat.MoviOff = pidx1->MoviOff + finalOffset - 4;
			/*fseek(&pParser->File, pidx1->MoviOff + finalOffset - 4);*/
			newPos = ftell(&pParser->File);
			fseek(&pParser->File, newPos - 4);
		} else {
			fResult = fseek(&pParser->File, finalOffset + pidx1->MoviOff - 4);
			if (fResult != FR_OK) {
				printf(((char [18]){"%d: Error seek.\n"}), __LINE__);
			}
			f_read(&pParser->File, &fourcc, 4, &bytesRead);
			if (bytesRead != 4) {
				printf(((char [18]){"%d: Error read.\n"}), __LINE__);
			}
			if ((fourcc & 0xFFFF3030) == FT_AVI_4CC_COMPRESSED_VIDEO || (fourcc & 0xFFFF3030) == FT_AVI_4CC_UNCOMPRESSED_VIDEO || (fourcc & 0xFFFF3030) == FT_AVI_4CC_AUDIO || (fourcc & 0xFFFF3030) == FT_AVI_4CC_TEXT || (fourcc & 0xFFFF3030) == FT_AVI_4CC_PALETTE_CHANGE) {
				//pParser->CurrStat.MoviOff = finalOffset;
				newPos = ftell(&pParser->File);
				fseek(&pParser->File, newPos - 4);
			} else {
				printf(((char [18]){"%d: Error seek.\n"}), __LINE__);
			}
		}
	}
	ret = ftell(&pParser->File);
#endif
	return ret;
}

/* Nothing beyond this */
