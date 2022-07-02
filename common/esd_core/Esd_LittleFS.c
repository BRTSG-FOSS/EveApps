/*
ESD Framework
Copyright (C) 2022  Bridgetek Pte Lte
Author: Jan Boon <jan.boon@kaetemi.be>
*/

/**
* This source code ("the Software") is provided by Bridgetek Pte Ltd
* ("Bridgetek") subject to the licence terms set out
*   http://brtchip.com/BRTSourceCodeLicenseAgreement/ ("the Licence Terms").
* You must read the Licence Terms before downloading or using the Software.
* By installing or using the Software you agree to the Licence Terms. If you
* do not agree to the Licence Terms then do not download or use the Software.
*
* Without prejudice to the Licence Terms, here is a summary of some of the key
* terms of the Licence Terms (and in the event of any conflict between this
* summary and the Licence Terms then the text of the Licence Terms will
* prevail).
*
* The Software is provided "as is".
* There are no warranties (or similar) in relation to the quality of the
* Software. You use it at your own risk.
* The Software should not be used in, or for, any medical device, system or
* appliance. There are exclusions of Bridgetek liability for certain types of loss
* such as: special loss or damage; incidental loss or damage; indirect or
* consequential loss or damage; loss of income; loss of business; loss of
* profits; loss of revenue; loss of contracts; business interruption; loss of
* the use of money or anticipated savings; loss of information; loss of
* opportunity; loss of goodwill or reputation; and/or loss of, damage to or
* corruption of data.
* There is a monetary cap on Bridgetek's liability.
* The Software may have subsequently been amended by another user and then
* distributed by that other user ("Adapted Software").  If so that user may
* have additional licence terms that apply to those amendments. However, Bridgetek
* has no liability in relation to those amendments.
*/

/*
LittleFS for EVE Screen Designer
*/

#include "Esd_LittleFS.h"
#include "Esd_Context.h"
#include "Esd_GpuAlloc.h"

#ifdef ESD_LITTLEFS_FLASH

//#define ESD_LITTLEFS_DIAGNOSTIC
#ifdef ESD_LITTLEFS_DIAGNOSTIC
#define eve_printf_diagnostic(fmt, ...) printf("[DIAGNOSTIC] " fmt, ##__VA_ARGS__)
#else
#define eve_printf_diagnostic(fmt, ...)
#endif

#define LFS_BLOCK_NULL ((lfs_block_t)-1)
#define EVE_FLASH_BLOCK_SIZE EVE_FLASH_UPDATE_ALIGN

#ifdef EVE_LITTLEFS_TESTS
extern const char *lfs_testbd_path;
extern uint32_t lfs_testbd_cycles;
uint32_t power_cycles;
#define EVE_TEST_POWER_CYCLES()                           \
	do                                                    \
	{                                                     \
		if (power_cycles > 0)                             \
		{                                                 \
			power_cycles -= 1;                            \
			if (power_cycles == 0)                        \
			{                                             \
				LFS_ASSERT(Esd_LittleFS_Sync(c) == 0);    \
				if (!lfs_testbd_path)                     \
				{                                         \
					EVE_CoCmd_memSet(phost, 0, 0xFF,      \
					    EVE_FLASH_BLOCK_SIZE * 2);        \
					EVE_CoCmd_flashUpdate(phost,          \
					    EVE_FLASH_FIRMWARE_SIZE,          \
					    0, EVE_FLASH_BLOCK_SIZE * 2);     \
					LFS_ASSERT(EVE_Cmd_waitFlush(phost)); \
				}                                         \
				Esd_Stop(ec);                             \
				Esd_Close(ec);                            \
				Esd_Release();                            \
				exit(33);                                 \
			}                                             \
		}                                                 \
	} while (false)
#else
#define EVE_TEST_POWER_CYCLES() \
	do                          \
	{                           \
	} while (false)
#endif

/** When the coprocessor gets reset, or when any of the flushing functions here fail, and there is unflushed data,
all open lfs files which have the DIRTY flag set should be flagged as ERRED. */
static void Esd_LittleFs_Erred()
{
	Esd_Context *ec = Esd_CurrentContext;
	eve_printf_diagnostic("Marking all dirty files as erred\n");
	lfs_t *lfs = &ec->Lfs;
	struct lfs_config *config = &Esd_CurrentContext->LfsConfig;
	for (lfs_file_t *f = (lfs_file_t*)lfs->mlist; f; f = f->next)
	{
		if (f->type != LFS_TYPE_REG)
			continue;
		if (f->flags & LFS_F_DIRTY)
			f->flags |= LFS_F_ERRED;
	}
	ec->LfsCmdFault = false;
	ec->LfsUnflushed = false;
	ec->LfsEraseFlushed = true;
}

// Sync the state of the underlying block device. Negative error codes are propagated to the user.
static int Esd_LittleFS_SyncImpl(const struct lfs_config *c, bool flush)
{
	Esd_Context *ec = c->context;
	EVE_HalContext *phost = &ec->HalContext;
	Esd_GpuAlloc *ga = &ec->GpuAlloc;

	if (ec->LfsCmdFault)
	{
		eve_printf_debug("Coprocessor fault occured while writing to flash\n");
		Esd_LittleFs_Erred();
		return LFS_ERR_IO;
	}

	if (ec->LfsEraseBlock != LFS_BLOCK_NULL)
	{
		uint32_t addr = Esd_GpuAlloc_Get(ga, ec->LfsEraseHandle);
		if (addr == GA_INVALID)
		{
			eve_printf_debug("Erase and update buffer was lost while syncing\n");
			return LFS_ERR_IO;
		}

		int32_t flashAddr = EVE_FLASH_FIRMWARE_SIZE + (ec->LfsEraseBlock * EVE_FLASH_BLOCK_SIZE);
		eve_printf_diagnostic("Flush an erase buffer, flashAddr %i, addr %i, sz %i\n", flashAddr, (int)addr, (int)EVE_FLASH_BLOCK_SIZE);
		EVE_CoCmd_flashUpdate(phost, flashAddr, addr, EVE_FLASH_UPDATE_ALIGN);
		ec->LfsUnflushed = true;

		ec->LfsEraseBlock = LFS_BLOCK_NULL;
		Esd_GpuAlloc_Free(ga, ec->LfsEraseHandle);
	}

	if (ec->LfsUnflushed && flush)
	{
		eve_printf_diagnostic("Flushing the coprocessor buffer\n");
		if (!EVE_Cmd_waitFlush(phost))
		{
			Esd_LittleFs_Erred();
			return LFS_ERR_IO;
		}
		ec->LfsUnflushed = false;
	}

	return LFS_ERR_OK;
}

// Sync the state of the underlying block device. Negative error codes are propagated to the user.
static int Esd_LittleFS_Sync(const struct lfs_config *c)
{
	eve_printf_diagnostic("LittleFS called Sync\n");
	return Esd_LittleFS_SyncImpl(c, true);
}

// Read a region in a block. Negative error codes are propagated to the user.
static int Esd_LittleFS_Read(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, void *buffer, lfs_size_t size)
{
	Esd_Context *ec = c->context;
	EVE_HalContext *phost = &ec->HalContext;
	Esd_GpuAlloc *ga = &ec->GpuAlloc;

#ifdef ESD_LITTLEFS_READPENDING
	// Read directly from a pending block
	if (ec->LfsEraseBlock == block)
	{
		uint32_t addr = Esd_GpuAlloc_Get(ga, ec->LfsEraseHandle);
		if (addr == GA_INVALID)
		{
			eve_printf_debug("Erase and update buffer was lost while reading\n");
			return LFS_ERR_IO;
		}

		if (!ec->LfsEraseFlushed && ec->LfsUnflushed)
		{
			// Wait for any pending writes, since we write through the coprocessor
			if (!EVE_Cmd_waitFlush(phost))
			{
				Esd_LittleFs_Erred();
				return LFS_ERR_IO;
			}
			ec->LfsEraseFlushed = true;
			ec->LfsUnflushed = false;
		}

		EVE_Hal_rdMem(phost, buffer, addr + off, size);
		if (phost->Status == EVE_STATUS_ERROR)
			return LFS_ERR_IO;
		return LFS_ERR_OK;
	}
#else
	// Sync any pending erase and writes
	if (ec->LfsEraseBlock == block)
	{
		int err = Esd_LittleFS_SyncImpl(c, false);
		if (err)
			return err;
	}
#endif

#ifdef ESD_LITTLEFS_READCACHE
	uint32_t lastProg = ec->LfsLastProg;
	bool matchesLastProg = false;
	if (lastProg != FA_INVALID)
	{
		ec->LfsLastProg = FA_INVALID;
		matchesLastProg = (lastProg == (EVE_FLASH_FIRMWARE_SIZE + (block * EVE_FLASH_BLOCK_SIZE) + off));
	}
	if (((off || size > 64) && !matchesLastProg && (off + size < EVE_FLASH_BLOCK_SIZE)) || block == ec->LfsReadBlock)
	{
		// Find existing cached block, or allocate space
		uint32_t addr = Esd_GpuAlloc_Get(ga, ec->LfsReadHandle);
		if (addr == GA_INVALID)
		{
			eve_printf_diagnostic("Allocating a reading block, %i\n", (int)block);
			ec->LfsReadBlock = LFS_BLOCK_NULL;
			ec->LfsReadHandle = Esd_GpuAlloc_Alloc(ga, EVE_FLASH_BLOCK_SIZE, GA_GC_FLAG);
			addr = Esd_GpuAlloc_Get(ga, ec->LfsReadHandle);
		}

		// We have a whole block
		if (addr != GA_INVALID)
		{
			if (ec->LfsReadBlock != block)
			{
				// Read a whole block
				int32_t flashAddr = EVE_FLASH_FIRMWARE_SIZE + (block * EVE_FLASH_BLOCK_SIZE);
				eve_printf_diagnostic("Read a whole block, %i, flashAddr %i, addr %i, sz %i\n", (int)block, (int)flashAddr, (int)addr, (int)EVE_FLASH_BLOCK_SIZE);
				if (!EVE_CoCmd_flashRead_flush(phost, addr, flashAddr, EVE_FLASH_BLOCK_SIZE))
				{
					eve_printf_debug("Reading failed, block %i\n", (int)block);
					ec->LfsCmdFault = true;
					ec->LfsReadBlock = LFS_BLOCK_NULL;
					Esd_GpuAlloc_Free(ga, ec->LfsReadHandle);
					return LFS_ERR_IO;
				}
				ec->LfsUnflushed = false;
				ec->LfsReadBlock = block;
			}

			// Read from RAM_G to memory
			eve_printf_diagnostic("Read from RAM_G cache, addr %i, size %i (flashAddr %i)\n", (int)(addr + off),
			    (int)size, (int)(EVE_FLASH_FIRMWARE_SIZE + (block * EVE_FLASH_BLOCK_SIZE) + off));
			EVE_Hal_rdMem(phost, buffer, addr + off, size);
			if (phost->Status == EVE_STATUS_ERROR)
				return LFS_ERR_IO;
			return LFS_ERR_OK;
		}
	}
#endif

	// Allocate buffer in RAM_G
	Esd_GpuHandle handle = Esd_GpuAlloc_Alloc(ga, size, GA_FIXED_FLAG);
	uint32_t addr = Esd_GpuAlloc_Get(ga, handle);
	if (addr == GA_INVALID)
		return LFS_ERR_NOMEM;

	// Read to RAM_G
	int res = LFS_ERR_IO;
	int32_t flashAddr = EVE_FLASH_FIRMWARE_SIZE + (block * EVE_FLASH_BLOCK_SIZE) + off;
	eve_printf_diagnostic("Read directly from flash through temporary RAM_G allocation, flashAddr %i, addr %i, sz %i\n", (int)flashAddr, (int)addr, (int)size);
	if (EVE_CoCmd_flashRead_flush(phost, addr, flashAddr, size))
	{
		// Read from RAM_G to memory
		ec->LfsUnflushed = false;
		EVE_Hal_rdMem(phost, buffer, addr, size);
		if (phost->Status != EVE_STATUS_ERROR)
			res = LFS_ERR_OK;
	}
	else
	{
		eve_printf_debug("Reading failed, block %i off %i\n", (int)block, (int)off);
		ec->LfsCmdFault = true;
	}

	// Release RAM_G buffer
	Esd_GpuAlloc_Free(ga, handle);
	return res;
}

// Program a region in a block. The block must have previously been erased. Negative error codes are propagated to the user.
// May return LFS_ERR_CORRUPT if the block should be considered bad.
static int Esd_LittleFS_Prog(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, const void *buffer, lfs_size_t size)
{
	Esd_Context *ec = c->context;
	EVE_HalContext *phost = &ec->HalContext;
	Esd_GpuAlloc *ga = &ec->GpuAlloc;

	if (ec->LfsCmdFault)
	{
		eve_printf_debug("Coprocessor fault occured while writing to flash\n");
		Esd_LittleFs_Erred();
		return LFS_ERR_IO;
	}

#ifdef ESD_LITTLEFS_READCACHE
	// Drop cached block being read
	if (ec->LfsReadBlock == block)
	{
		eve_printf_diagnostic("Drop read cache, block %i\n", (int)block);
		ec->LfsReadBlock = LFS_BLOCK_NULL;
		Esd_GpuAlloc_Free(ga, ec->LfsReadHandle);
	}
#endif

#ifdef ESD_LITTLEFS_READCACHE
	int32_t flashAddr = EVE_FLASH_FIRMWARE_SIZE + (block * EVE_FLASH_BLOCK_SIZE) + off;
	ec->LfsLastProg = flashAddr;
#endif

	// Write to erase and update buffer
	if (block == ec->LfsEraseBlock)
	{
		uint32_t addr = Esd_GpuAlloc_Get(ga, ec->LfsEraseHandle);
		if (addr == GA_INVALID)
		{
			eve_printf_debug("Erase and update buffer was lost while programming\n");
			return LFS_ERR_IO;
		}
		eve_printf_diagnostic("Prog to erase buffer, addr %i, size %i\n", (int)(addr + off), (int)size);
#ifdef ESD_LITTLEFS_READPENDING
		ec->LfsEraseFlushed = false;
#endif
		ec->LfsUnflushed = true;
		EVE_CoCmd_memWrite(phost, addr + off, size);
		if (!EVE_Cmd_wrMem(phost, buffer, size))
			return LFS_ERR_IO;
		EVE_TEST_POWER_CYCLES();
		return LFS_ERR_OK;
	}

	// Write to flash
#ifndef ESD_LITTLEFS_READCACHE
	int32_t flashAddr = EVE_FLASH_FIRMWARE_SIZE + (block * EVE_FLASH_BLOCK_SIZE) + off;
#endif
	eve_printf_diagnostic("Prog directly to flash, block %i, flashAddr %i, sz %i\n", (int)block, (int)flashAddr, (int)size);
	ec->LfsUnflushed = true;
	EVE_CoCmd_flashWrite(phost, flashAddr, size);
	if (!EVE_Cmd_wrMem(phost, buffer, size))
		return LFS_ERR_IO;

	EVE_TEST_POWER_CYCLES();
	return LFS_ERR_OK;
}

// Erase a block. A block must be erased before being programmed.
// The state of an erased block is undefined. Negative error codes are propagated to the user.
// May return LFS_ERR_CORRUPT if the block should be considered bad.
static int Esd_LittleFS_Erase(const struct lfs_config *c, lfs_block_t block)
{
	Esd_Context *ec = c->context;
	EVE_HalContext *phost = &ec->HalContext;
	Esd_GpuAlloc *ga = &ec->GpuAlloc;

	if (ec->LfsCmdFault)
	{
		eve_printf_debug("Coprocessor fault occured while writing to flash\n");
		Esd_LittleFs_Erred();
		return LFS_ERR_IO;
	}

#ifdef ESD_LITTLEFS_READCACHE
	// Drop cached block being read
	if (ec->LfsReadBlock == block)
	{
		eve_printf_diagnostic("Drop read cache, block %i\n", (int)block);
		ec->LfsReadBlock = LFS_BLOCK_NULL;
		Esd_GpuAlloc_Free(ga, ec->LfsReadHandle);
	}
#endif

	// Erase in RAM_G if it's the same block again
	if (ec->LfsEraseBlock == block)
	{
		uint32_t addr = Esd_GpuAlloc_Get(ga, ec->LfsEraseHandle);
		if (addr == GA_INVALID)
			return LFS_ERR_IO;
		eve_printf_diagnostic("Erase existing buffer, block %i, addr %i\n", (int)block, (int)addr);
#ifdef ESD_LITTLEFS_READPENDING
		ec->LfsEraseFlushed = false;
#endif
		ec->LfsUnflushed = true;
		EVE_CoCmd_memSet(phost, addr, 0xFF, EVE_FLASH_UPDATE_ALIGN);
		EVE_TEST_POWER_CYCLES();
		return LFS_ERR_OK;
	}

	// Flush any pending erase and writes
	if (ec->LfsEraseBlock != LFS_BLOCK_NULL)
	{
		int err = Esd_LittleFS_SyncImpl(c, false);
		if (err)
			return err;
	}

	// Allocate an erase buffer in RAM_G
	ec->LfsEraseHandle = Esd_GpuAlloc_Alloc(ga, EVE_FLASH_UPDATE_ALIGN, GA_FIXED_FLAG);
	uint32_t addr = Esd_GpuAlloc_Get(ga, ec->LfsEraseHandle);
	if (addr == GA_INVALID)
	{
		eve_printf_diagnostic("No memory for creating erase buffer, block %i, addr %i\n", (int)block, (int)addr);
		return LFS_ERR_NOMEM;
	}
	eve_printf_diagnostic("Create an erase buffer, block %i, addr %i\n", (int)block, (int)addr);
#ifdef ESD_LITTLEFS_READPENDING
	ec->LfsEraseFlushed = false;
#endif
	ec->LfsUnflushed = true;
	EVE_CoCmd_memSet(phost, addr, 0xFF, EVE_FLASH_UPDATE_ALIGN);
	ec->LfsEraseBlock = block;

	EVE_TEST_POWER_CYCLES();
	return LFS_ERR_OK;
}

static bool Esd_LittleFS_Configure()
{
	EVE_HalContext *phost = Esd_Host;
	Esd_Context *ec = Esd_CurrentContext;
	struct lfs_config *config = &ec->LfsConfig;
	config->context = ec;

	int32_t flashSize = EVE_Hal_rd32(phost, REG_FLASH_SIZE);
	if (!flashSize)
		return false;

	// block device operations
	config->read = Esd_LittleFS_Read;
	config->prog = Esd_LittleFS_Prog;
	config->erase = Esd_LittleFS_Erase;
	config->sync = Esd_LittleFS_Sync;

	// block device configuration
	config->read_size = EVE_FLASH_READ_ALIGN; // read alignment, smallest possible read
	config->prog_size = EVE_FLASH_WRITE_ALIGN; // write alignment, smallest possible write
	config->block_size = EVE_FLASH_BLOCK_SIZE;
	config->block_count = flashSize * (1024 * 1024 / EVE_FLASH_BLOCK_SIZE) - 1;
	config->cache_size = EVE_FLASH_WRITE_ALIGN; // write burst cache for small writes
	config->lookahead_size = sizeof(ec->LfsLookaheadBuffer); // size of in-RAM free blocks lookahead bitmap (cache)
	config->block_cycles = 100;

	// buffers
	config->read_buffer = ec->LfsReadBuffer;
	config->prog_buffer = ec->LfsProgBuffer;
	config->lookahead_buffer = ec->LfsLookaheadBuffer;

	// flags
	config->flags = LFS_M_GROW;

	// reset
	ec->LfsUnflushed = false;
	ec->LfsCmdFault = false;
	
#ifdef EVE_LITTLEFS_TESTS
	// tests
	power_cycles = lfs_testbd_cycles;
#endif

	return true;
}

bool Esd_LittleFS_Mount()
{
	int err;
	Esd_Context *ec = Esd_CurrentContext;
	lfs_t *lfs = &ec->Lfs;
	struct lfs_config *config = &Esd_CurrentContext->LfsConfig;

	if (ec->LfsMounted)
		return true; // Already mounted

	// Configure
	ec->LfsEraseBlock = LFS_BLOCK_NULL;
#ifdef ESD_LITTLEFS_READCACHE
	ec->LfsReadBlock = LFS_BLOCK_NULL;
#endif
	if (!Esd_LittleFS_Configure())
		return false;

	// Mount
	err = lfs_mount(lfs, config);
	ec->LfsMounted = !err;
	return !err;
}

void Esd_LittleFS_Unmount()
{
	Esd_Context *ec = Esd_CurrentContext;
	lfs_t *lfs = &ec->Lfs;
	struct lfs_config *config = &Esd_CurrentContext->LfsConfig;

	if (!ec->LfsMounted)
		return; // Not mounted

	Esd_LittleFS_SyncImpl(config, true);
	lfs_unmount(lfs);
}

bool Esd_LittleFs_ReadMetadataFile(uint8_t *metadata, const char *resourceFile)
{
	Esd_Context *ec = Esd_CurrentContext;
	lfs_t *lfs = &ec->Lfs;
	struct lfs_config *config = &Esd_CurrentContext->LfsConfig;
	int err;

	if (!metadata)
		return false;

	if (!Esd_LittleFS_Mount())
		return false;

	// Generate metafile name
	size_t nameLen = strlen(resourceFile);
	if (nameLen + 6 > 128) // ".esdm" NUL
		return false;
	char metaFile[128];
#pragma warning(push)
#pragma warning(disable : 4996)
	strcpy(metaFile, resourceFile);
	strcpy(&metaFile[nameLen], ".esdm");
#pragma warning(pop)

	// Read file
	lfs_file_t lfsFile;
	uint8_t buf[EVE_FLASH_WRITE_ALIGN];
	struct lfs_file_config fileConfig = { 0 };
	fileConfig.buffer = buf;
	if ((err = lfs_file_opencfg(lfs, &lfsFile, metaFile, LFS_O_RDONLY, &fileConfig)))
	{
		eve_printf_debug("[DIAGNOSTIC] Metadata can't be opened %s (%i, %i) with error %i\n", metaFile, (int)strlen(resourceFile), (int)strlen(metaFile), err);
		return false; // File doesn't exist
	}
	lfs_block_t head;
	bool res = false;
	if ((err = lfs_file_reserved(lfs, &lfsFile, &head)))
	{
		// Regular file
		res = lfs_file_read(lfs, &lfsFile, metadata, min(ESD_METADATA_MAX, lfs_file_size(lfs, &lfsFile))) >= 0;
	}
	else
	{
		// Reserved
		res = Esd_LittleFS_Read(config, head, 0, metadata, min(ESD_METADATA_MAX, lfs_file_size(lfs, &lfsFile))) >= 0;
	}
	lfs_file_close(lfs, &lfsFile);
	return res;
}

#endif

/* end of file */
