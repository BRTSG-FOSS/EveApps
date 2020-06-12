/*

Allocation mechanism for RAM_G.

Allocation returns a handle, consisting part of the allocation index and a sequence code to invalidate the allocation.
Handle can be turned into an address using Ft_Esd_GpuAlloc_Get. This call should be repeated each frame,
since the allocation may be invalidated by a graphics device reset, or the allocation may be moved by a memory
defragmentation process.
Supports up to MAX_NUM_ALLOCATIONS simultaneous allocations.

The regular use pattern is to call Ft_Esd_GpuAlloc_Get using a stored handle during the render stage whenever an image is required.
This function may be called with an invalid handle. Valid handles may become invalid between render stages when not in use.
Addresses may thus become invalid between frames, and must not be stored. When the handle is invalid the returned address will be GA_INVALID.
Ft_Esd_GpuAlloc_Alloc is then to be used with GA_GC_FLAG to allocate or re-allocate memory space, the new handle must be stored for later use, 
and Ft_Esd_GpuAlloc_Get must be called again with the new handle to get the new address in graphics memory.

When not using the GA_GC_FLAG, you must call Ft_Esd_GpuAlloc_Free manually when the memory is no longer in use.
Either allocation option does not guarantee that memory will remain persistently allocated. Graphics memory can be reset whenever necessary.

*/

#ifndef FT_ESD_COMPATIBILITY_GPUALLOC_H
#define FT_ESD_COMPATIBILITY_GPUALLOC_H

#include "ESD_GpuAlloc.h"

#define Ft_Esd_GpuHandle ESD_GpuHandle
#define Ft_Esd_GpuAllocEntry ESD_GpuAllocEntry
#define Ft_Esd_GpuAllocRef ESD_GpuAllocRef
#define Ft_Esd_GpuAlloc ESD_GpuAlloc

#define Ft_Esd_GpuAlloc_Reset ESD_GpuAlloc_Reset
#define Ft_Esd_GpuAlloc_Update ESD_GpuAlloc_Update
#define Ft_Esd_GpuAlloc_Alloc ESD_GpuAlloc_Alloc
#define Ft_Esd_GpuAlloc_Free ESD_GpuAlloc_Free
#define Ft_Esd_GpuAlloc_Get ESD_GpuAlloc_Get
#define Ft_Esd_GpuAlloc_GetTotalUsed ESD_GpuAlloc_GetTotalUsed
#define Ft_Esd_GpuAlloc_GetTotal ESD_GpuAlloc_GetTotal
#define Ft_Esd_GpuAlloc_Print ESD_GpuAlloc_Print

#endif /* #ifndef FT_ESD_GPUALLOC_H */

/* end of file */
