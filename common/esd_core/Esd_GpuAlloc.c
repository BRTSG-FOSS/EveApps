/*

Allocation mechanism for RAM_G.

*/

#include "Esd_Base.h"
#include "Esd_GpuAlloc.h"

#ifdef ESD_SIMULATION
static int s_ErrorGpuAllocFailed = 0;
#endif

#ifndef GA_ENABLE_DEFRAG_SAFE
#define GA_ENABLE_DEFRAG_SAFE 1
#endif
// TODO: #ifndef GA_ENABLE_DEFRAG_AGGRESSIVE
// TODO: #define GA_ENABLE_DEFRAG_AGGRESSIVE 1
// TODO: #endif

#if GA_ENABLE_DEFRAG_SAFE
#include "Esd_Context.h"
#endif

ESD_CORE_EXPORT void Esd_GpuAlloc_Reset(Esd_GpuAlloc *ga)
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

ESD_CORE_EXPORT void Esd_GpuAlloc_InsertFree(Esd_GpuAlloc *ga, uint32_t atidx, uint32_t size)
{
	uint32_t idx;

	eve_assert(ga->NbAllocEntries >= 1);
	eve_assert(atidx < MAX_NUM_ALLOCATIONS);

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

ESD_CORE_EXPORT Esd_GpuHandle Esd_GpuAlloc_Alloc(Esd_GpuAlloc *ga, uint32_t size, uint16_t flags)
{
	uint32_t idx;
	Esd_GpuHandle ret;

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
						Esd_GpuAlloc_InsertFree(ga, idx + 1, remaining);
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
		Esd_LogError("Unable to allocate RAM_G space");
		s_ErrorGpuAllocFailed = 1;
	}
#endif

	ret.Id = MAX_NUM_ALLOCATIONS;
	ret.Seq = 0;
	return ret;
}

ESD_CORE_EXPORT uint32_t Esd_GpuAlloc_Get(Esd_GpuAlloc *ga, Esd_GpuHandle handle)
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

ESD_CORE_EXPORT void Esd_GpuAlloc_CollapseFree(Esd_GpuAlloc *ga, uint32_t idxat)
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

ESD_CORE_EXPORT void Esd_GpuAlloc_FreeId(Esd_GpuAlloc *ga, int id)
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
	Esd_GpuAlloc_CollapseFree(ga, idx);
}

ESD_CORE_EXPORT void Esd_GpuAlloc_Free(Esd_GpuAlloc *ga, Esd_GpuHandle handle)
{
	int id = handle.Id;
	if (id < MAX_NUM_ALLOCATIONS
	    && ga->AllocRefs[id].Seq == handle.Seq)
	{
		Esd_GpuAlloc_FreeId(ga, id);
	}
}

ESD_CORE_EXPORT void Esd_GpuAlloc_Update(Esd_GpuAlloc *ga)
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
					Esd_GpuAlloc_FreeId(ga, ga->AllocEntries[idx].Id);

					// Due to free collapse, the current idx may become another allocated entry
					--idx;
					continue;
				}
			}

			// Always clear the used flag on update
			ga->AllocEntries[idx].Flags &= ~GA_USED_FLAG;
		}
	}

#if GA_ENABLE_DEFRAG_SAFE
	for (idx = 0; idx < ga->NbAllocEntries; ++idx)
	{
		// Find the first empty block
		if (ga->AllocEntries[idx].Id == MAX_NUM_ALLOCATIONS)
			break;
	}

	// Memory is fragmented, try to find a small block that can be moved
	// This strategy only works if the first open space is larger than the fragmented blocks
	if (idx != ga->NbAllocEntries - 1)
	{
		// The first empty block is the largest space we can fill to cleanly defragment memory
		uint32_t leftIdx = idx;
		uint32_t leftSpace = ga->AllocEntries[leftIdx].Length;

		uint32_t largestIdx = MAX_NUM_ALLOCATIONS;
		uint32_t largestSpace = 0;

		for (idx = leftIdx + 1; idx < ga->NbAllocEntries; ++idx)
		{
			if (ga->AllocEntries[idx].Id != MAX_NUM_ALLOCATIONS
			    && ga->AllocEntries[idx].Length <= leftSpace
			    && (ga->AllocEntries[idx].Flags & GA_FIXED_FLAG) == 0)
			{
				// This entry can potentially be moved
				uint32_t space = 0;

				// Count space that will be emptied
				// Doesn't count for the first entry, it moves into the same empty space
				if (idx > leftIdx + 1)
					space += ga->AllocEntries[idx].Length;

				// Count empty space on the left
				if (ga->AllocEntries[idx - 1].Id == MAX_NUM_ALLOCATIONS)
					space += ga->AllocEntries[idx - 1].Length;

				// Count empty space on the right
				if (idx + 1 < MAX_NUM_ALLOCATIONS
				    && ga->AllocEntries[idx + 1].Id == MAX_NUM_ALLOCATIONS)
					space += ga->AllocEntries[idx + 1].Length;

				if (space > largestSpace)
				{
					largestIdx = idx;
					largestSpace = space;
				}
			}
		}

		if (largestIdx != MAX_NUM_ALLOCATIONS)
		{
			// Store handle, address, and size
			uint16_t handle = ga->AllocEntries[largestIdx].Id;
			uint32_t addr = ga->AllocEntries[largestIdx].Address;
			uint32_t size = ga->AllocEntries[largestIdx].Length;

			// Esd_GpuAlloc_Print(ga);

			// Get new allocation
			Esd_GpuHandle newHandle = Esd_GpuAlloc_Alloc(ga, size, ga->AllocEntries[largestIdx].Flags);
			idx = ga->AllocRefs[handle].Idx;
			uint32_t newIdx = ga->AllocRefs[newHandle.Id].Idx;
			uint32_t newAddr = ga->AllocEntries[newIdx].Address;

			eve_assert(ga->AllocEntries[idx].Id == handle); // Verify old allocation
			eve_printf_debug("Defragmenting allocation %i with handle id %i and size %i bytes, leaving a free space of %i bytes. Copy %i to %i\n",
			    (int)largestIdx, handle, size, (int)largestSpace, (int)addr, (int)newAddr);
			EVE_CoCmd_memCpy(Esd_GetHost(), newAddr, addr, size);
			if (!EVE_Cmd_waitFlush(Esd_GetHost()))
			{
				eve_printf_debug("Failed to copy. Defragmentation failed");
			}
			else
			{
				// Swap the handles
				ga->AllocEntries[newIdx].Id = handle;
				ga->AllocEntries[idx].Id = newHandle.Id; // New handle will be discarded
				ga->AllocRefs[handle].Idx = newIdx;
				ga->AllocRefs[newHandle.Id].Idx = idx; // New handle will be discarded

				ga->AllocEntries[newIdx].Flags &= ~GA_USED_FLAG; // Don't flag the new one as used
				ga->AllocEntries[idx].Flags |= GA_GC_FLAG; // Force GC on the old address
			}

			// Esd_GpuAlloc_Print(ga);
		}
	}
#endif
}

// Get total used GPU RAM
ESD_CORE_EXPORT uint32_t Esd_GpuAlloc_GetTotalUsed(Esd_GpuAlloc *ga)
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
ESD_CORE_EXPORT uint32_t Esd_GpuAlloc_GetTotal(Esd_GpuAlloc *ga)
{
	return ga->RamGSize;
}

#ifndef NDEBUG
ESD_CORE_EXPORT void Esd_GpuAlloc_Print(Esd_GpuAlloc *ga)
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
