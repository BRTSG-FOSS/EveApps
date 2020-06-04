/*

Allocation mechanism for RAM_G.

Allocation returns a handle, consisting part of the allocation index and a sequence code to invalidate the allocation.
Handle can be turned into an address using ESD_GpuAlloc_Get. This call should be repeated each frame,
since the allocation may be invalidated by a graphics device reset, or the allocation may be moved by a memory
defragmentation process.
Supports up to MAX_NUM_ALLOCATIONS simultaneous allocations.

The regular use pattern is to call ESD_GpuAlloc_Get using a stored handle during the render stage whenever an image is required.
This function may be called with an invalid handle. Valid handles may become invalid between render stages when not in use.
Addresses may thus become invalid between frames, and must not be stored. When the handle is invalid the returned address will be GA_INVALID.
ESD_GpuAlloc_Alloc is then to be used with GA_GC_FLAG to allocate or re-allocate memory space, the new handle must be stored for later use, 
and ESD_GpuAlloc_Get must be called again with the new handle to get the new address in graphics memory.

When not using the GA_GC_FLAG, you must call ESD_GpuAlloc_Free manually when the memory is no longer in use.
Either allocation option does not guarantee that memory will remain persistently allocated. Graphics memory can be reset whenever necessary.

*/

#ifndef ESD_GPUALLOC__H
#define ESD_GPUALLOC__H

#include "EVE_IntTypes.h"

#define MAX_NUM_ALLOCATIONS 64UL

// Set the GC flag. This automatically frees the allocation when the USED flag is not set during Update.
// Using this flag means you must call ESD_GpuAlloc_Get on each frame to keep the allocation alive
#define GA_GC_FLAG 1

// Used flag is set whenever ESD_GpuAlloc_Get is called, reset on every Update
#define GA_USED_FLAG 2

// Low priority flag is set when the allocation may be discarded when low on RAM (not yet implemented).
#define GA_LOW_FLAG 4

// Address which is returned when the allocation is invalid (~0).
#define GA_INVALID ~0UL

// Handle to a gpu memory allocation
typedef struct
{
	/// Id in the allocation reference table
	uint32_t Id : 8;
	/// Sequence number used to invalidate handles
	uint32_t Seq : 24;

} ESD_GpuHandle;

// Internal information about a gpu memory allocation handle
typedef struct
{
	uint32_t Idx : 8;
	uint32_t Seq : 24;

} ESD_GpuAllocRef;

// Internal gpu allocation entry
typedef struct
{
	uint32_t Address;
	uint32_t Length;
	uint16_t Id;
	uint16_t Flags;

} ESD_GpuAllocEntry;

typedef struct
{
	/// Reference to an allocation entry, by allocation Id
	ESD_GpuAllocRef AllocRefs[MAX_NUM_ALLOCATIONS];
	/// Allocation map,
	ESD_GpuAllocEntry AllocEntries[MAX_NUM_ALLOCATIONS];
	/// Number of valid alloc entries
	uint32_t NbAllocEntries;
	/// RAM_G size usable by the allocator. Reset GpuAlloc after modifying
	uint32_t RamGSize;

} ESD_GpuAlloc;

// Initialize or reset gpu ram allocation mechanism
void ESD_GpuAlloc_Reset(ESD_GpuAlloc *ga);

// Run basic garbage collection and any other mechanisms, call first in the Update cycle, after the frame is fully rendered and swapped
void ESD_GpuAlloc_Update(ESD_GpuAlloc *ga);

// Allocate a gpu ram block
ESD_GpuHandle ESD_GpuAlloc_Alloc(ESD_GpuAlloc *ga, uint32_t size, uint16_t flags);

// Free a gpu ram block
void ESD_GpuAlloc_Free(ESD_GpuAlloc *ga, ESD_GpuHandle handle);

// Get ram address from handle. Returns ~0 when invalid.
uint32_t ESD_GpuAlloc_Get(ESD_GpuAlloc *ga, ESD_GpuHandle handle);

// Get total used GPU RAM
uint32_t ESD_GpuAlloc_GetTotalUsed(ESD_GpuAlloc *ga);

// Get total GPU RAM
uint32_t ESD_GpuAlloc_GetTotal(ESD_GpuAlloc *ga);

#ifndef NDEBUG
void ESD_GpuAlloc_Print(ESD_GpuAlloc *ga);
#else
#define ESD_GpuAlloc_Print(ga) \
	do                            \
	{                             \
	} while (0)
#endif

#endif /* #ifndef ESD_GPUALLOC__H */

/* end of file */
