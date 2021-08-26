
#ifndef HEAP_4_H_
#define HEAP_4_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

// Configure the statically allocated heap size here
#define configTOTAL_HEAP_SIZE	(uint32_t)(50u * 1024u)



void *pvPortMalloc( size_t xSize ) ;
void vPortFree( void *pv ) ;
void* vPortCalloc(size_t num, size_t siz) ;



#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* HEAP_4_H_ */
