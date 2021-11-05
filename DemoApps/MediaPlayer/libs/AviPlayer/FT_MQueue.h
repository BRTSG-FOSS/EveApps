#ifndef _FT_MQUEUE_H_
#define _FT_MQUEUE_H_

/* Queue implemented in a fixed size queue element and ring implementation */
/* Limits: 255 elements,  */
/* Should we use this or just the ring buffer? */

/* Queue implemented in a fixed size queue element and ring implementation */
typedef enum FT_RINGQUEUE_STATUS
{
	FT_RINGQUEUE_NOSPACETOWRITE = -3,
	FT_RINGQUEUE_INVALIDPARAMS = -2,
	FT_RINGQUEUE_ERROR = -1,
	FT_RINGQUEUE_OK = 0,
	FT_RINGQUEUE_WARNING = 1,
	FT_RINGQUEUE_FLUSHLENBIGGER = 2,
	FT_RINGQUEUE_WRITELENBIGGER = 3,
	FT_RINGQUEUE_READLENBIGGER = 4,
	FT_RINGQUEUE_PEEKLENBIGGER = 5,
	FT_RINGQUEUE_NOVALIDDATA = 6,
}FT_RINGQUEUE_STATUS;

/* Limits: 255 elements,  */
/* Should we use this or just the ring buffer? */
/* Should we have a range from min to max? currently from 0 to TotEle */
typedef struct FT_RingQueue
{
	ft_uint8_t  TotEle;			/* Total number of elements in the queue */
	ft_uint8_t  ReadIdx;		/* Current valid element */
	ft_uint8_t  WriteIdx;		/* Oldest invalid element */
	ft_uint8_t  ValidEle;		/* Total number of valid elements - as read and write indexes can be same */
	ft_uint8_t	*pBuff;			/* Starting address of queue buffer */
	ft_uint32_t EleSize;		/* Size of each element */
}FT_RingQueue_t;


typedef struct FT_RingQueueE
{
	ft_uint32_t EleSize;
	ft_void_t	*pEle;			/* Element structure */
}FT_RingQueueE_t;


ft_int32_t FT_RQueue_Init(FT_RingQueue_t *prq,ft_uint8_t TotElements,ft_uint8_t *pEle,ft_uint32_t EleSz);
ft_int32_t FT_RQueue_ReadE(FT_RingQueue_t *prq,ft_void_t *pEle);
ft_int32_t FT_RQueue_PeekE(FT_RingQueue_t *prq,ft_void_t *pEle,ft_uint8_t Ele);
ft_int32_t FT_RQueue_PeekELatest(FT_RingQueue_t *prq,ft_void_t *pEle);
ft_int32_t FT_RQueue_WriteE(FT_RingQueue_t *prq,ft_void_t *pEle);
ft_int32_t FT_RQueue_WriteEInplace(FT_RingQueue_t *prq,ft_void_t *pEle);
ft_int32_t FT_RQueue_WriteEInplace_Oldest(FT_RingQueue_t *prq,ft_void_t *pEle);
ft_int32_t FT_RQueue_WriteE_Update(FT_RingQueue_t *prq, ft_void_t *pEle, ft_uint8_t Ele);
ft_int32_t FT_RQueue_NumFreeE(FT_RingQueue_t *prq,ft_uint32_t *pNumEle);//number of free elements
ft_int32_t FT_RQueue_Flush(FT_RingQueue_t *prq,ft_uint8_t Numidx);//flush only one element
ft_int32_t FT_RQueue_FlushAll(FT_RingQueue_t *prq);//flush all the elements
ft_int32_t FT_RQueue_Set(FT_RingQueue_t *prq, ft_uint8_t Writeidx,ft_uint8_t Readidx,ft_uint8_t ValidEles);//custom set


/* N buffer design */
/* Not a circular buffer, so End is always greater than Curr */
typedef struct FT_NBuffer
{
	ft_uint8_t 				*pStart;			/* Starting address of buffer */
	ft_int32_t  			Length;			/* Length of buffer */
	ft_uint8_t 				*pCurr;			/* Address of read pointer */
	ft_uint8_t 				*pEnd;			/* Pointer to Address of write pointer */
	struct FT_NBuffer 		*pNext;			/* Pointer to next buffer element */
}FT_NBuffer_t;

typedef struct FT_NBuffer_Ctxt
{
	FT_NBuffer_t 	*pStart;
	FT_NBuffer_t 	*pRead;
	FT_NBuffer_t 	*pWrite;
	ft_uint8_t	 	NumEle;
	ft_uint32_t  	TotSz;			/* Total size of N buffers */
	ft_uint32_t  	TotValid;			/* Total valid length of all the N buffers */
}FT_NBuffer_Ctxt_t;


ft_int32_t FT_NBuff_Init(FT_NBuffer_Ctxt_t *pnbctxt, ft_uint8_t NumEle);
ft_int32_t FT_NBuff_SetBuff(FT_NBuffer_t *pbuff,ft_uint32_t BufferAddress,ft_int32_t BufferLength);
ft_int32_t FT_NBuff_AddBuff(FT_NBuffer_Ctxt_t *pnbctxt, FT_NBuffer_t *pbuff);
ft_int32_t FT_NBuff_RemBuff(FT_NBuffer_Ctxt_t *pnbctxt, FT_NBuffer_t *pbuff);
ft_int32_t FT_NBuff_Set(FT_NBuffer_Ctxt_t *pnbctxt, FT_NBuffer_t *pRead, FT_NBuffer_t *pWrite);
ft_int32_t FT_NBuff_Read(FT_NBuffer_Ctxt_t *pnbctxt,ft_uint8_t *pOutBuffer,ft_int32_t OutLength, ft_int32_t *pReadLength);
ft_int32_t FT_NBuff_Peek(FT_NBuffer_Ctxt_t *pnbctxt,ft_uint8_t *pOutBuffer,ft_int32_t OutLength, ft_int32_t *pPeekLength);
ft_int32_t FT_NBuff_Write(FT_NBuffer_Ctxt_t *pnbctxt,ft_uint8_t *pInBuffer,ft_int32_t InLength,ft_int32_t *pWriteLength);
ft_int32_t FT_NBuff_ValidSize(FT_NBuffer_Ctxt_t *pnbctxt);
ft_int32_t FT_NBuff_Flush(FT_NBuffer_Ctxt_t *pnbctxt,ft_int32_t flushLength);
ft_int32_t FT_NBuff_FlushAll(FT_NBuffer_Ctxt_t *pnbctxt);
ft_int32_t FT_NBuff_Exit(FT_NBuffer_Ctxt_t *pnbctxt);


#endif

