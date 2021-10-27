#ifndef _FT_BUFFER_H_
#define _FT_BUFFER_H_
#include "FT_Platform.h"


/* limitations is - pointers are 32 bit and length is 2GB, protection wrt concurrency? */

/* limitations is - pointers are 32 bit and length is 2GB, protection wrt concurrency? */
typedef enum FT_RINGBUFF_STATUS
{
	FT_RINGBUFFER_INVALIDPARAMS = -2,
	FT_RINGBUFFER_ERROR = -1,
	FT_RINGBUFFER_OK = 0,
	FT_RINGBUFFER_WARNING = 1,
	FT_RINGBUFFER_FLUSHLENBIGGER = 2,
	FT_RINGBUFFER_WRITELENBIGGER = 3,
	FT_RINGBUFFER_READLENBIGGER = 4,
	FT_RINGBUFFER_PEEKLENBIGGER = 5,
	FT_RINGBUFFER_NOVALIDDATA = 6,
}FT_RINGBUFF_STATUS;

typedef enum FT_RINGBUFF_TYPE
{
	FT_RINGBUFFER_RAM_BUFFER = 0,
	FT_RINGBUFFER_FT81X_RAM_G_BUFFER = 1,
}FT_RINGBUFF_TYPE;

/* Ring buffer context */
typedef struct FT_RingBuffer
{
	ft_uint32_t Start;			/* Starting address of the ring buffer */
	ft_int32_t  Length;			/* Length of ring buffer */
	ft_uint32_t Read;			/* Address of read pointer */
	ft_uint32_t Write;			/* Address of write pointer */
	ft_int32_t  ValidLength;	/* Valid length of the content in the ring buffer - as read and write pointers can be same */
	FT_RINGBUFF_TYPE BufferType;
	ft_void_t* Option;
}FT_RingBuffer_t;

/* returns errors and also returns number of bytes written or read if the requested to more than the actual */
ft_int32_t FT_RingBuffer_Init(FT_RingBuffer_t *prb, ft_uint32_t BufferAddress,ft_int32_t BufferLength);
ft_int32_t FT_RingBuffer_Read(FT_RingBuffer_t *prb,ft_uint8_t *pOutBuffer,ft_int32_t OutLength, ft_int32_t *pReadLength);
ft_int32_t FT_RingBuffer_Peek(FT_RingBuffer_t *prb,ft_uint8_t *pOutBuffer,ft_int32_t OutLength, ft_int32_t *pPeekLength);
ft_int32_t FT_RingBuffer_Write(FT_RingBuffer_t *prb,ft_uint8_t *pInBuffer,ft_int32_t InLength,ft_int32_t *pWriteLength);
ft_int32_t FT_RingBuffer_FreeSpace(FT_RingBuffer_t *prb);
ft_int32_t FT_RingBuffer_AvailableData(FT_RingBuffer_t *prb);
ft_int32_t FT_RingBuffer_Flush(FT_RingBuffer_t *prb,ft_int32_t flushLength);
ft_int32_t FT_RingBuffer_FlushAll(FT_RingBuffer_t *prb);
ft_int32_t FT_RingBuffer_Set(FT_RingBuffer_t *prb, ft_uint32_t BufferRead,ft_uint32_t BufferWrite,ft_int32_t ValidLength);
/* Return 32 bits from ringbuffer - No error types returned */
ft_uint32_t FT_RingBuffer_Read32(FT_RingBuffer_t *prb);
ft_uint16_t FT_RingBuffer_Read16(FT_RingBuffer_t *prb);
ft_uint8_t FT_RingBuffer_Read8(FT_RingBuffer_t *prb);
ft_int32_t FT_RingBuffer_Exit(FT_RingBuffer_t *prb);//inverse of init functionality


#endif /* #ifndef _FT_BUFFER_H_ */


