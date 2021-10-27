/* Ring buffer, N buffer implementaiton */

#include "FT_Platform.h"
#include "FT_Buffer.h"



/*****************************************************************************/
/*                     Ring buffer implementation                            */
/*****************************************************************************/

/* returns errors and also returns number of bytes written or read if the requested to more than the actual */
ft_int32_t FT_RingBuffer_Init(FT_RingBuffer_t *prb, ft_uint32_t BufferAddress,ft_int32_t BufferLength)
{
	/* Initialize all the parameters of the context */
	prb->Start = BufferAddress;
	prb->Length = BufferLength;

	/* point to starting of the memory and make the valid length to 0 */
	prb->ValidLength = 0;
	prb->Read = BufferAddress;
	prb->Write = BufferAddress;
	prb->BufferType = FT_RINGBUFFER_FT81X_RAM_G_BUFFER;
	return FT_RINGBUFFER_OK;
}

/* Read data from ring buffer */
ft_int32_t FT_RingBuffer_Read(FT_RingBuffer_t *prb,ft_uint8_t *pOutBuffer,ft_int32_t OutLength, ft_int32_t *pReadLength)
{
	FT_RINGBUFF_STATUS Status = FT_RINGBUFFER_OK;
	ft_int32_t Length = OutLength;

	/* Sanity check for valid length */
	if(0 == prb->ValidLength)
	{
		return FT_RINGBUFFER_NOVALIDDATA;
	}

	if(Length > prb->ValidLength)
	{
		Length = prb->ValidLength;
		Status = FT_RINGBUFFER_READLENBIGGER;
	}

	/* Split use case */
	if((prb->Read + Length) >= (prb->Start + prb->Length))
	{
		ft_uint32_t FirstPartial, SecondPartial;

		FirstPartial = prb->Start + prb->Length - prb->Read;
		SecondPartial = Length - FirstPartial;

#ifdef FT900_PLATFORM
		/* First copy the first partial and later copy the second partial */
		memcpy(pOutBuffer,prb->Read,FirstPartial);
		pOutBuffer += FirstPartial;
		if(SecondPartial > 0)
		{
			memcpy(pOutBuffer, prb->Start,SecondPartial);
		}
#endif
#if defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM)
		//memcpy(prb->Read,inBuffer,FirstPartial);
		memcpy(pOutBuffer, prb->Read, FirstPartial);
		//inBuffer += FirstPartial;
		pOutBuffer += FirstPartial;
		if(SecondPartial > 0)
		{
			//memcpy(prb->Start,inBuffer,SecondPartial);
			memcpy(pOutBuffer, prb->Start, SecondPartial);
		}
#endif
#ifdef FT900_PLATFORM
	interrupt_disable_globally();
#endif
		prb->Read = prb->Start + SecondPartial;
		prb->ValidLength -= Length;
#ifdef FT900_PLATFORM
	interrupt_enable_globally();
#endif
	}
	else
	{
#ifdef FT900_PLATFORM
		/* First copy the first partial and later copy the second partial */
		memcpy(pOutBuffer,prb->Read,Length);
#endif
#if defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM)
		//memcpy(prb->Read,inBuffer,Length);
		memcpy(pOutBuffer, prb->Read, Length);
#endif

#ifdef FT900_PLATFORM
	interrupt_disable_globally();
#endif
		prb->Read += Length;
		prb->ValidLength -= Length;
#ifdef FT900_PLATFORM
	interrupt_enable_globally();
#endif


	}


	/* sanity check - make sure valid length => 0 */

	/* return the number of bytes written into the ring buffer */
	*pReadLength = Length;

	return Status;
}

/* Read the data without incrementing the read pointers */
ft_int32_t FT_RingBuffer_Peek(FT_RingBuffer_t *prb,ft_uint8_t *pOutBuffer,ft_int32_t OutLength, ft_int32_t *pPeekLength)
{
	FT_RINGBUFF_STATUS Status = FT_RINGBUFFER_OK;
	ft_int32_t Length = OutLength;

	if(Length > prb->ValidLength)
	{
		Length = prb->ValidLength;
		Status = FT_RINGBUFFER_PEEKLENBIGGER;
	}

	/* Buffer split use case */
	if((prb->Read + Length) >= (prb->Start + prb->Length))
	{
		ft_uint32_t FirstPartial, SecondPartial;

		FirstPartial = prb->Start + prb->Length - prb->Read;
		SecondPartial = Length - FirstPartial;

#ifdef FT900_PLATFORM
		/* First copy the first partial and later copy the second partial */
		memcpy(pOutBuffer,prb->Read,FirstPartial);
		pOutBuffer += FirstPartial;
		if(SecondPartial > 0)
		{
			memcpy(pOutBuffer, prb->Start,SecondPartial);
		}
#endif
#if defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM)
		//memcpy(prb->Read,inBuffer,FirstPartial);
		memcpy(pOutBuffer, prb->Read, FirstPartial);
		//inBuffer += FirstPartial;
		pOutBuffer += FirstPartial;
		if(SecondPartial > 0)
		{
			//memcpy(prb->Start,inBuffer,SecondPartial);
			memcpy(pOutBuffer, prb->Start, SecondPartial);
		}
#endif
	}
	else
	{
#ifdef FT900_PLATFORM
		/* First copy the first partial and later copy the second partial */
		memcpy(pOutBuffer,prb->Read,Length);
#endif
#if defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM)
		//memcpy(prb->Read,inBuffer,Length);
		memcpy(pOutBuffer, prb->Read, Length);
#endif
	}

	/* return the number of bytes written into the ring buffer */
	*pPeekLength = Length;

	return Status;
}

/* Write data into ring buffer - integration of c library for faster throughput */
ft_int32_t FT_RingBuffer_Write(FT_RingBuffer_t *prb,ft_uint8_t *pInBuffer,ft_int32_t InLength,ft_int32_t *pWriteLength)
{
	
	FT_RINGBUFF_STATUS Status = FT_RINGBUFFER_OK;
#if defined(FT_81X_ENABLE) || defined(BT81XA_ENABLE) || defined(BT_81X_ENABLE)
	Ft_Gpu_Hal_Context_t *pHalContext = (Ft_Gpu_Hal_Context_t *)prb->Option;
#endif
	ft_int32_t Length = InLength;

	if(Length > FT_RingBuffer_FreeSpace(prb))
	{
		Length = FT_RingBuffer_FreeSpace(prb);
		Status = FT_RINGBUFFER_WRITELENBIGGER;
	}

	/* Buffer split use case */
	if((prb->Write + Length) >= (prb->Start + prb->Length))
	{
		ft_uint32_t FirstPartial, SecondPartial;

		FirstPartial = prb->Start + prb->Length - prb->Write;
		SecondPartial = Length - FirstPartial;

#if defined(FT900_PLATFORM)
		/* First copy the first partial and later copy the second partial */
		if(prb->BufferType == FT_RINGBUFFER_RAM_BUFFER){
			memcpy(prb->Write,pInBuffer,FirstPartial);  //original
			//printf("ram buffer - first.\n");
		}
#if defined(FT_81X_ENABLE) || defined(BT81XA_ENABLE) || defined(BT_81X_ENABLE)
		else if(prb->BufferType == FT_RINGBUFFER_FT81X_RAM_G_BUFFER){
			Ft_Gpu_Hal_WrMem(pHalContext,prb->Write, pInBuffer, FirstPartial);
			//printf("ram g buffer - first.\n");
		}
#endif
		//memcpy((ft_uint8_t *)prb->Write,pInBuffer,FirstPartial);
		pInBuffer += FirstPartial;
		if(SecondPartial > 0)
		{
			if(prb->BufferType == FT_RINGBUFFER_RAM_BUFFER){
				memcpy(prb->Start,pInBuffer,SecondPartial);  //original
				//memcpy((ft_uint8_t *)prb->Start,pInBuffer,SecondPartial);
				//printf("ram buffer - second.\n");
			}
#if defined(FT_81X_ENABLE) || defined(BT81XA_ENABLE) || defined(BT_81X_ENABLE)
			else if(prb->BufferType == FT_RINGBUFFER_FT81X_RAM_G_BUFFER){
				Ft_Gpu_Hal_WrMem(pHalContext,prb->Start, pInBuffer, SecondPartial);
				//printf("ram g buffer - second.\n");
			}
#endif
		}
#elif defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM)

		if(prb->BufferType == FT_RINGBUFFER_RAM_BUFFER){
			memcpy(prb->Write,pInBuffer,FirstPartial);
		}
#if defined(FT_81X_ENABLE) || defined(BT81XA_ENABLE) || defined(BT_81X_ENABLE)
		else if(prb->BufferType == FT_RINGBUFFER_FT81X_RAM_G_BUFFER){
			Ft_Gpu_Hal_WrMem(pHalContext,prb->Write, pInBuffer, FirstPartial);
			//printf("ram g buffer - first.\n");
		}
#endif
		pInBuffer += FirstPartial;
		if(SecondPartial > 0)
		{
			if(prb->BufferType == FT_RINGBUFFER_RAM_BUFFER){
				memcpy(prb->Start,pInBuffer,SecondPartial);
			}
#if defined(FT_81X_ENABLE) || defined(BT81XA_ENABLE) || defined(BT_81X_ENABLE)
			else if(prb->BufferType == FT_RINGBUFFER_FT81X_RAM_G_BUFFER){
				Ft_Gpu_Hal_WrMem(pHalContext,prb->Start, pInBuffer, SecondPartial);
			}
#endif
		}
#endif

#ifdef FT900_PLATFORM
	interrupt_disable_globally();
#endif
		prb->Write = prb->Start + SecondPartial;
		/* Assuming processor is 32bit atomic */
		prb->ValidLength += Length;
#ifdef FT900_PLATFORM
	interrupt_enable_globally();
#endif

	}
	else
	{
#ifdef FT900_PLATFORM
		/* First copy the first partial and later copy the second partial */
		if(prb->BufferType == FT_RINGBUFFER_RAM_BUFFER){
			memcpy(prb->Write,pInBuffer,Length);  //original
			//memcpy((ft_uint8_t *)prb->Write,pInBuffer,Length);
			//printf("ram buffer - whole.\n");
		}
#if defined(FT_81X_ENABLE) || defined(BT81XA_ENABLE) || defined(BT_81X_ENABLE)
		else if(prb->BufferType == FT_RINGBUFFER_FT81X_RAM_G_BUFFER){
			Ft_Gpu_Hal_WrMem(pHalContext,prb->Write, pInBuffer, Length);
			//printf("ram g buffer - whole.\n");
		}
#endif
#elif defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM)
		//memcpy(prb->Write,inBuffer,Length);
		if(prb->BufferType == FT_RINGBUFFER_RAM_BUFFER){
			memcpy(prb->Write, pInBuffer, Length);
		}
#if defined(FT_81X_ENABLE) || defined(BT81XA_ENABLE) || defined(BT_81X_ENABLE)
		else if(prb->BufferType == FT_RINGBUFFER_FT81X_RAM_G_BUFFER){
			Ft_Gpu_Hal_WrMem(pHalContext,prb->Write, pInBuffer, Length);
			//printf("ram g buffer - whole.\n");
		}
#endif
#endif

#ifdef FT900_PLATFORM
	interrupt_disable_globally();
#endif
		prb->Write += Length;
		/* Assuming processor is 32bit atomic */
		prb->ValidLength += Length;
#ifdef FT900_PLATFORM
	interrupt_enable_globally();
#endif

	}

	/* return the number of bytes written into the ring buffer */
	*pWriteLength = Length;

	return Status;
}


/* Return the total free space available in ring buffer */
ft_int32_t FT_RingBuffer_FreeSpace(FT_RingBuffer_t *prb)
{
	return (prb->Length - prb->ValidLength);
}

/* return available data in ring buffer */
ft_int32_t FT_RingBuffer_AvailableData(FT_RingBuffer_t *prb)
{
	return prb->ValidLength;
}

/* Flush FlushLength amount of data */
ft_int32_t FT_RingBuffer_Flush(FT_RingBuffer_t *prb,ft_int32_t FlushLength)
{
	ft_uint32_t Length = FlushLength;

	if(FlushLength > prb->ValidLength)
	{
		/* Requested flush length is greater than valid length */
		FT_RingBuffer_FlushAll(prb);

		return FT_RINGBUFFER_FLUSHLENBIGGER;
	}

	/* Flush the data and point the read pointer to respective location */
	if((prb->Read + Length) >= (prb->Start + prb->Length))
	{
		ft_uint32_t FirstPartial, SecondPartial;

		FirstPartial = prb->Start + prb->Length - prb->Read;
		SecondPartial = Length - FirstPartial;
		prb->Read = prb->Start + SecondPartial;
	}
	else
	{
		prb->Read += Length;
	}
	/* Assuming accessing 32 bit is atomic from processor point of view */
	/* Reduce the valid length */
	prb->ValidLength -= Length;

	return FT_RINGBUFFER_OK;
}

/* Flush all the data - write pointer remains the same and read pointer is updated to write address */
ft_int32_t FT_RingBuffer_FlushAll(FT_RingBuffer_t *prb)
{
	/* Flush all data and make the validlength to be 0 */
	prb->ValidLength = 0;
	prb->Read = prb->Write;

	return FT_RINGBUFFER_OK;
}

/* Reset context parameters to default values */
ft_int32_t FT_RingBuffer_Set(FT_RingBuffer_t *prb, ft_uint32_t BufferRead,ft_uint32_t BufferWrite,ft_int32_t ValidLength)
{
	ft_uint32_t BufferEnd = prb->Start + prb->Length;

	/* Sanity checks for set params */
	if(ValidLength >prb->Length)
	{
		return FT_RINGBUFFER_INVALIDPARAMS;
	}

	if((BufferRead > BufferEnd) || (BufferWrite > BufferEnd))
	{
		return FT_RINGBUFFER_INVALIDPARAMS;
	}

	/* Set read/write pointers and respective valid size */
	prb->ValidLength = ValidLength;
	prb->Read = BufferRead;
	prb->Write = BufferWrite;

	return FT_RINGBUFFER_OK;
}

/* Return 32 bits from ringbuffer - No error types returned */
ft_uint32_t FT_RingBuffer_Read32(FT_RingBuffer_t *prb)
{
	ft_uint32_t Read32,ReadLength;

	/* Read 4 bytes from ring buffer */
	FT_RingBuffer_Read(prb,&Read32,4,&ReadLength);

	return Read32;
}
/* Return 16 bits from ringbuffer - No error types returned */
ft_uint16_t FT_RingBuffer_Read16(FT_RingBuffer_t *prb)
{
	ft_uint32_t ReadLength;
	ft_uint16_t Read16;

	/* Read 4 bytes from ring buffer */
	FT_RingBuffer_Read(prb,&Read16,2,&ReadLength);

	return Read16;
}
/* Return 32 bits from ringbuffer - No error types returned */
ft_uint8_t FT_RingBuffer_Read8(FT_RingBuffer_t *prb)
{
	ft_uint32_t ReadLength;
	ft_uint8_t Read8;

	/* Read 4 bytes from ring buffer */
	FT_RingBuffer_Read(prb,&Read8,1,&ReadLength);

	return Read8;
}


