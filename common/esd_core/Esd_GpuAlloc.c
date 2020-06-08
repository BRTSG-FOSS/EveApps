/*

Allocation mechanism for RAM_G.

*/

#include "ESD_TypeDefs.h"
#include "ESD_GpuAlloc.h"

#include <EVE_Hal.h>

#ifdef ESD_SIMULATION
static int s_ErrorGpuAllocFailed = 0;
#endif

void ESD_GpuAlloc_Reset(ESD_GpuAlloc *ga)
{
	int id, idx;

#ifdef ESD_SIMULATION
	s_ErrorGpuAllocFailed = 0;
#endif

	for (id = 0; id < MAX_NUM_ALLOCATIONS; ++id)
	{
		ga->AllocRefs[id].Idx = MAX_NUM_ALLOCATIONS;
		++ga->AllocRefs[id].Seq; // Seq is always cycled, initial value not important

		// Skip seq 0 to simplify invalid values
		if (ga->AllocRefs[id].Seq == 0)
			ga->AllocRefs[id].Seq = 1;
	}

	for (idx = 0; idx < MAX_NUM_ALLOCATIONS; ++idx)
	{
		ga->AllocEntries[idx].Address = ga->RamGSize;
		ga->AllocEntries[idx].Length = 0;
		ga->AllocEntries[idx].Id = MAX_NUM_ALLOCATIONS;
		ga->AllocEntries[idx].Flags = 0;
	}

	// First allocation entry is unallocated entry of entire RAM_G_SIZE
	ga->AllocEntries[0].Address = 0;
	ga->AllocEntries[0].Length = ga->RamGSize;
	ga->NbAllocEntries = 1;
}

void ESD_GpuAlloc_InsertFree(ESD_GpuAlloc *ga, uint32_t atidx, uint32_t size)
{
	uint32_t idx;

	eve_assert(ga->NbAllocEntries >= 1);

	// First move entries one step forward
	for (idx = (ga->NbAllocEntries - 1); idx >= atidx; --idx)
	{
		int id = ga->AllocEntries[idx].Id;
		if (id < MAX_NUM_ALLOCATIONS)
		{
			// Move reference idx one ahead
			++ga->AllocRefs[id].Idx;
		}
		ga->AllocEntries[idx + 1] = ga->AllocEntries[idx];
	}

	// Set the free space entry
	ga->AllocEntries[atidx].Address = ga->AllocEntries[atidx - 1].Address + ga->AllocEntries[atidx - 1].Length;
	ga->AllocEntries[atidx].Length = size;
	ga->AllocEntries[atidx].Id = MAX_NUM_ALLOCATIONS;
	ga->AllocEntries[atidx].Flags = 0;
	++ga->NbAllocEntries;
}

ESD_GpuHandle ESD_GpuAlloc_Alloc(ESD_GpuAlloc *ga, uint32_t size, uint16_t flags)
{
	uint32_t idx;
	ESD_GpuHandle ret;

	if (ga->NbAllocEntries >= MAX_NUM_ALLOCATIONS)
	{
		goto ReturnInvalidHandle;
	}

	// Always align size to 4 bytes
	size = (size + 3UL) & ~3UL;

	// Scan through the AllocEntries, find the first open allocation that is large enough
	for (idx = 0; idx < ga->NbAllocEntries; ++idx)
	{
		// Check if allocation entry is unallocated and large enough
		if (ga->AllocEntries[idx].Id == MAX_NUM_ALLOCATIONS
		    && ga->AllocEntries[idx].Length >= size)
		{
			// Find an unused handle
			int id;
			for (id = 0; id < MAX_NUM_ALLOCATIONS; ++id)
			{
				if (ga->AllocRefs[id].Idx == MAX_NUM_ALLOCATIONS)
				{
					// Allocate this block
					uint32_t remaining = ga->AllocEntries[idx].Length - size;
					ga->AllocEntries[idx].Length = size;
					ga->AllocEntries[idx].Id = id;
					flags |= GA_USED_FLAG;
					ga->AllocEntries[idx].Flags = flags;
					ga->AllocRefs[id].Idx = idx;
					++ga->AllocRefs[id].Seq;

					// Skip seq 0 to simplify invalid values
					if (ga->AllocRefs[id].Seq == 0)
						ga->AllocRefs[id].Seq = 1;

					// Insert free space entry after
					if (remaining)
					{
						ESD_GpuAlloc_InsertFree(ga, idx + 1, remaining);
					}

#ifdef ESD_SIMULATION
					s_ErrorGpuAllocFailed = 0;
#endif

					// eve_printf_debug("Alloc id %i\n", id);

					// Return the valid gpu ram handle
					ret.Id = id;
					ret.Seq = ga->AllocRefs[id].Seq;
					return ret;
				}
			}

			// No invalid handle found
			goto ReturnInvalidHandle;
		}
	}

	// No space left, return an invalid allocation handle...
ReturnInvalidHandle:

#ifdef ESD_SIMULATION
	if (!s_ErrorGpuAllocFailed)
	{
		ESD_logError("Unable to allocate RAM_G space");
		s_ErrorGpuAllocFailed = 1;
	}
#endif

	ret.Id = MAX_NUM_ALLOCATIONS;
	ret.Seq = 0;
	return ret;
}

uint32_t ESD_GpuAlloc_Get(ESD_GpuAlloc *ga, ESD_GpuHandle handle)
{
	if (handle.Id < MAX_NUM_ALLOCATIONS)
	{
		if (ga->AllocRefs[handle.Id].Seq == handle.Seq)
		{
			uint16_t id = handle.Id;
			uint16_t idx = ga->AllocRefs[id].Idx;
			ga->AllocEntries[idx].Flags |= GA_USED_FLAG;
			return ga->AllocEntries[idx].Address;
		}
	}

	return ~0;
}

void ESD_GpuAlloc_CollapseFree(ESD_GpuAlloc *ga, uint32_t idxat)
{
	int shift = 0;
	if (idxat + 1 < ga->NbAllocEntries && ga->AllocEntries[idxat + 1].Id == MAX_NUM_ALLOCATIONS)
	{
		// Next entry is free, add to collapse
		++shift;
		ga->AllocEntries[idxat].Length += ga->AllocEntries[idxat + 1].Length;
	}
	if (idxat > 0 && ga->AllocEntries[idxat - 1].Id == MAX_NUM_ALLOCATIONS)
	{
		// Previous entry is free, add to collapse
		--idxat;
		++shift;
		ga->AllocEntries[idxat].Length += ga->AllocEntries[idxat + 1].Length;
	}
	if (shift)
	{
		// Collapse indices
		uint32_t idx;
		ga->NbAllocEntries -= shift;
		for (idx = idxat + 1; idx < ga->NbAllocEntries; ++idx)
		{
			int id;
			ga->AllocEntries[idx] = ga->AllocEntries[idx + shift];
			id = ga->AllocEntries[idx].Id;
			if (id < MAX_NUM_ALLOCATIONS)
			{
				ga->AllocRefs[id].Idx = idx;
			}
		}
	}
}

void ESD_GpuAlloc_FreeId(ESD_GpuAlloc *ga, int id)
{
	// eve_printf_debug("Free id %i\n", id);

#ifdef ESD_SIMULATION
	s_ErrorGpuAllocFailed = 0;
#endif

	int idx = ga->AllocRefs[id].Idx;

	// Invalidate handle reference
	ga->AllocRefs[id].Idx = MAX_NUM_ALLOCATIONS;
	++ga->AllocRefs[id].Seq;

	// Skip seq 0 to simplify invalid values
	if (ga->AllocRefs[id].Seq == 0)
		ga->AllocRefs[id].Seq = 1;

	// Free entry
	ga->AllocEntries[idx].Id = MAX_NUM_ALLOCATIONS;
	ga->AllocEntries[idx].Flags = 0;

	// Collapse neighbouring entries
	ESD_GpuAlloc_CollapseFree(ga, idx);
}

void ESD_GpuAlloc_Free(ESD_GpuAlloc *ga, ESD_GpuHandle handle)
{
	int id = handle.Id;
	if (id < MAX_NUM_ALLOCATIONS
	    && ga->AllocRefs[id].Seq == handle.Seq)
	{
		ESD_GpuAlloc_FreeId(ga, id);
	}
}

void ESD_GpuAlloc_Update(ESD_GpuAlloc *ga)
{
	uint32_t idx;
	for (idx = 0; idx < ga->NbAllocEntries; ++idx)
	{
		// Check if allocation entry is allocated
		if (ga->AllocEntries[idx].Id < MAX_NUM_ALLOCATIONS)
		{
			// If GC flag is set and used flag is not set, free this memory
			if (ga->AllocEntries[idx].Flags & GA_GC_FLAG)
			{
				if (!(ga->AllocEntries[idx].Flags & GA_USED_FLAG))
				{
					ESD_GpuAlloc_FreeId(ga, ga->AllocEntries[idx].Id);

					// Due to free collapse, the current idx may become another allocated entry
					--idx;
					continue;
				}
			}

			// Always clear the used flag on update
			ga->AllocEntries[idx].Flags &= ~GA_USED_FLAG;
		}
	}
}

// Get total used GPU RAM
uint32_t ESD_GpuAlloc_GetTotalUsed(ESD_GpuAlloc *ga)
{
	uint32_t total = 0;
	uint32_t idx;
	for (idx = 0; idx < ga->NbAllocEntries; ++idx)
	{
		// Check if allocation entry is allocated
		if (ga->AllocEntries[idx].Id < MAX_NUM_ALLOCATIONS)
		{
			// Add the size of the allocated block to the total
			total += (uint32_t)ga->AllocEntries[idx].Length;
		}
	}
	return total;
}

// Get total GPU RAM
uint32_t ESD_GpuAlloc_GetTotal(ESD_GpuAlloc *ga)
{
	return ga->RamGSize;
}

#ifndef NDEBUG
void ESD_GpuAlloc_Print(ESD_GpuAlloc *ga)
{
	uint32_t idx;
	eve_printf_debug("GpuAlloc:\n");
	for (idx = 0; idx < ga->NbAllocEntries; ++idx)
	{
		eve_printf_debug("%i: id: %i, addr: %li, len: %li, flags: %i\n",
			(int)idx,
		    (int)ga->AllocEntries[idx].Id,
		    (long int)ga->AllocEntries[idx].Address,
		    (long int)ga->AllocEntries[idx].Length,
		    (int)ga->AllocEntries[idx].Flags);
	}
}
#endif

/* end of file */
