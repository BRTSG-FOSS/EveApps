/* Message queue implementation */

#include "FT_Platform.h"
#include "FT_MQueue.h"



/* Implementation of ring queue */
/* returns errors and also returns number of bytes written or read if the requested to more than the actual */
ft_int32_t FT_RQueue_Init(FT_RingQueue_t *prq,ft_uint8_t TotElements,ft_uint8_t *pEle,ft_uint32_t EleSz)
{
	FT_RINGQUEUE_STATUS Status = FT_RINGQUEUE_OK;

	/* Initialize all the parameters of queue context */
	prq->TotEle = TotElements;
	prq->ReadIdx = 0;
	prq->WriteIdx = 0;
	prq->ValidEle = 0;
	prq->pBuff = pEle;
	prq->EleSize = EleSz;

	return Status;
}

/* Need to protect in case of multi entrance - integrate osal here? */
/* Assuming that buffer allocated is aligned wrt multiple of uint32 as max datatype in FT_RingQueue_t is uint32 */
ft_int32_t FT_RQueue_ReadE(FT_RingQueue_t *prq,ft_void_t *pEle)
{
	/* Check for number of valid elements */
	if(prq->ValidEle <=0)
	{
		prq->ValidEle = 0;//workaround to ensure no bus are inserted
		return FT_RINGQUEUE_NOVALIDDATA;
	}

	memcpy(pEle,prq->pBuff + (prq->EleSize * prq->ReadIdx),prq->EleSize);

#ifdef FT900_PLATFORM
	interrupt_disable_globally();
#endif

	/* Increment the readindex and also decrement the valid elements count */
	prq->ReadIdx++;
	/* Check for boundary scenario */
	if(prq->ReadIdx >= prq->TotEle)
	{
		prq->ReadIdx = 0;
	}
	prq->ValidEle--;
#ifdef FT900_PLATFORM
	interrupt_enable_globally();
#endif

	return FT_RINGQUEUE_OK;
}
ft_int32_t FT_RQueue_PeekE(FT_RingQueue_t *prq,ft_void_t *pEle,ft_uint8_t Ele)
{
	/* Check for number of valid elements */
	if(prq->ValidEle <=0)
	{
		return FT_RINGQUEUE_NOVALIDDATA;
	}

	memcpy(pEle,prq->pBuff + (prq->EleSize * Ele),prq->EleSize);

	return FT_RINGQUEUE_OK;
}

ft_int32_t FT_RQueue_PeekELatest(FT_RingQueue_t *prq,ft_void_t *pEle)
{
	ft_int32_t peekele = (ft_int32_t)prq->WriteIdx - 1;
	/* Check for number of valid elements */
	if(prq->ValidEle <=0)
	{
		return FT_RINGQUEUE_NOVALIDDATA;
	}

	if(peekele < 0)//hack as WriteIdx is uint8
	{
		peekele += prq->TotEle;
	}

	memcpy(pEle,prq->pBuff + (prq->EleSize * peekele),prq->EleSize);

	return FT_RINGQUEUE_OK;
}
ft_int32_t FT_RQueue_WriteE(FT_RingQueue_t *prq,ft_void_t *pEle)
{
	/* Check for number of valid elements */
	if(prq->ValidEle >= prq->TotEle)
	{
		return FT_RINGQUEUE_NOSPACETOWRITE;
	}

	memcpy(prq->pBuff + (prq->EleSize * prq->WriteIdx),pEle,prq->EleSize);
#ifdef FT900_PLATFORM
	interrupt_disable_globally();
#endif
	/* Increment the write index and also increment the valid elements count */
	prq->WriteIdx++;
	/* Check for boundary scenario */
	if(prq->WriteIdx >= prq->TotEle)
	{
		prq->WriteIdx = 0;
	}
	prq->ValidEle++;
#ifdef FT900_PLATFORM
	interrupt_enable_globally();
#endif

	return FT_RINGQUEUE_OK;
}
/* Write the values in place */
ft_int32_t FT_RQueue_WriteEInplace(FT_RingQueue_t *prq,ft_void_t *pEle)
{
	ft_int32_t peekele = (ft_int32_t)prq->WriteIdx - 1;
	/* Check for number of valid elements */
	if(prq->ValidEle <= 0)
	{
		return FT_RINGQUEUE_NOVALIDDATA;
	}

	if(peekele < 0)//hack for uint8 loopback
	{
		peekele += prq->TotEle;
	}
#ifdef FT900_PLATFORM
	interrupt_disable_globally();
#endif

	memcpy(prq->pBuff + (prq->EleSize * peekele),pEle,prq->EleSize);

#ifdef FT900_PLATFORM
	interrupt_enable_globally();
#endif

	return FT_RINGQUEUE_OK;
}
///update the oldest element
ft_int32_t FT_RQueue_WriteEInplace_Oldest(FT_RingQueue_t *prq,ft_void_t *pEle)
{
	/* Check for number of valid elements */
	if(prq->ValidEle <= 0)
	{
		return FT_RINGQUEUE_NOVALIDDATA;
	}
#ifdef FT900_PLATFORM
	interrupt_disable_globally();
#endif

	memcpy(prq->pBuff + (prq->EleSize * prq->ReadIdx),pEle,prq->EleSize);
#ifdef FT900_PLATFORM
	interrupt_enable_globally();
#endif

	return FT_RINGQUEUE_OK;
}

//update the specified element
ft_int32_t FT_RQueue_WriteE_Update(FT_RingQueue_t *prq, ft_void_t *pEle, ft_uint8_t Ele)
{
	/* Check for number of valid elements */
	if(prq->ValidEle <=0)
	{
		return FT_RINGQUEUE_NOVALIDDATA;
	}

#ifdef FT900_PLATFORM
	interrupt_disable_globally();
#endif

	memcpy(prq->pBuff + (prq->EleSize * Ele), pEle, prq->EleSize);
#ifdef FT900_PLATFORM
	interrupt_enable_globally();
#endif
	return FT_RINGQUEUE_OK;
}

//number of free elements
ft_int32_t FT_RQueue_NumFreeE(FT_RingQueue_t *prq,ft_uint32_t *pNumEle)
{
	*pNumEle = prq->TotEle - prq->ValidEle;

	return FT_RINGQUEUE_OK;
}
//flush N elements
ft_int32_t FT_RQueue_Flush(FT_RingQueue_t *prq,ft_uint8_t NumEles)
{
	FT_RINGQUEUE_STATUS Status = FT_RINGQUEUE_OK;
#ifdef FT900_PLATFORM
	interrupt_disable_globally();
#endif

	if(NumEles > prq->ValidEle)
	{
		NumEles = prq->ValidEle;
		Status = FT_RINGQUEUE_FLUSHLENBIGGER;
	}

	if((NumEles + prq->ReadIdx) > prq->TotEle)
	{
		prq->ReadIdx = NumEles + prq->ReadIdx - prq->TotEle;
	}
	else
	{
		prq->ReadIdx += NumEles;
	}
	//update the valid elements number
	prq->ValidEle -= NumEles;
#ifdef FT900_PLATFORM
	interrupt_enable_globally();
#endif

	return Status;
}
//flush all the elements - write pointer will remain at same location as earlier
ft_int32_t FT_RQueue_FlushAll(FT_RingQueue_t *prq)
{
#ifdef FT900_PLATFORM
	interrupt_disable_globally();
#endif

	prq->ReadIdx = prq->WriteIdx;
	prq->ValidEle = 0;
#ifdef FT900_PLATFORM
	interrupt_enable_globally();
#endif

	return FT_RINGQUEUE_OK;
}
//custom set
ft_int32_t FT_RQueue_Set(FT_RingQueue_t *prq, ft_uint8_t Writeidx,ft_uint8_t Readidx,ft_uint8_t ValidEles)
{
#ifdef FT900_PLATFORM
	interrupt_disable_globally();
#endif

	prq->WriteIdx = Writeidx;
	prq->ReadIdx = Readidx;
	prq->ValidEle = ValidEles;
#ifdef FT900_PLATFORM
	interrupt_enable_globally();
#endif

	return FT_RINGQUEUE_OK;
}

