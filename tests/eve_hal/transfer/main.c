
#include <stdio.h>
#include <EVE_Hal.h>

#pragma warning(disable: 4996)

static EVE_HalContext s_Host;
static bool s_Running = true;
#if !defined(FT9XX_PLATFORM) && !defined(RP2040_PLATFORM)
uint8_t s_BufferWr[16 * 1024 * 1024];
uint8_t s_BufferRd[16 * 1024 * 1024];
#endif
uint32_t s_BufferCmdWr[1024];
uint32_t s_BufferCmdRd[1024];

inline uint32_t wangHash(uint32_t a)
{
	a = (a ^ 61) ^ (a >> 16);
	a = a + (a << 3);
	a = a ^ (a >> 4);
	a = a * 0x27d4eb2d;
	a = a ^ (a >> 15);
	return a;
}

bool loop(EVE_HalContext *phost)
{
	uint32_t i, j;
	int failures = 0;
	uint32_t hk = 0;
#if !defined(FT9XX_PLATFORM) && !defined(RP2040_PLATFORM)
	EVE_Hal_displayMessageZ(phost, "Testing RAM_G reads and writes at various sizes...");
	printf("Testing RAM_G reads and writes at various sizes...\n");
	for (i = 0; i < 4096; ++i) /* 4096 rounds */
	{
		uint32_t a;
		uint32_t b;
		uint32_t idx;
		uint32_t sz;
		/* Random test on RAM_G */
		if (i % 32 == 0)
		{
			if (i != 0) printf("\n");
			printf("[%i] ", i);
		}
		a = wangHash(++hk) & (RAM_G_SIZE - 1);
		b = wangHash(++hk) & (RAM_G_SIZE - 1);
		idx = min(a, b);
		sz = max(a, b) - idx;
		for (j = 0; j < sz; ++j)
			s_BufferWr[j] = wangHash(++hk) & 0xFF;
		memset(s_BufferRd, 0xBABABABA, sizeof(s_BufferRd));
		EVE_Hal_wrMem(phost, idx, s_BufferWr, sz);
		printf("O");
		EVE_Hal_rdMem(phost, s_BufferRd, idx, sz);
		printf("I");

		for (j = 0; j < sz; ++j)
		{
			if (s_BufferWr[j] != s_BufferRd[j])
			{
				printf("Failed RAM_G test %i, addr %i with sz %i\n", i, idx, sz);
				++failures;
				eve_debug_break();
			}
		}
	}
	printf("\n");
	printf("\n");

	EVE_Hal_displayMessageZ(phost, "Testing RAM_G reads and multiple writes at various sizes...");
	printf("Testing RAM_G reads and multiple writes at various sizes...\n");
	for (i = 0; i < 4096; ++i) /* 4096 rounds */
	{
		uint32_t a;
		uint32_t b;
		uint32_t idx;
		uint32_t sz;
		uint32_t cj;
		uint32_t cur;
		uint32_t rem;

		/* Random test on RAM_G */
		if (i % 32 == 0)
		{
			if (i != 0) printf("\n");
			printf("[%i] ", i);
		}
		a = wangHash(++hk) & (RAM_G_SIZE - 1);
		b = wangHash(++hk) & (RAM_G_SIZE - 1);
		idx = min(a, b);
		sz = max(a, b) - idx;
		for (j = 0; j < sz; ++j)
			s_BufferWr[j] = wangHash(++hk) & 0xFF;
		memset(s_BufferRd, 0xBABABABA, sizeof(s_BufferRd));
		cj = 0;
		cur = idx;
		rem = sz;
		EVE_Hal_startTransfer(phost, EVE_TRANSFER_WRITE, idx);
		do
		{
			uint32_t w = wangHash(++hk) % (rem + 1);
			EVE_Hal_transferMem(phost, NULL, &s_BufferWr[cj], w);
			rem -= w;
			cur += w;
			cj += w;
		} while (rem);
		EVE_Hal_endTransfer(phost);
		printf("O");
		EVE_Hal_rdMem(phost, s_BufferRd, idx, sz);
		printf("I");

		for (j = 0; j < sz; ++j)
		{
			if (s_BufferWr[j] != s_BufferRd[j])
			{
				printf("Failed RAM_G test %i, addr %i with sz %i\n", i, idx, sz);
				++failures;
				eve_debug_break();
			}
		}
	}
	printf("\n");
	printf("\n");

	EVE_Hal_displayMessageZ(phost, "Testing RAM_CMD reads and writes at various sizes...");
	printf("Testing RAM_CMD reads and writes at various sizes...\n");
	for (i = 0; i < 4096; ++i) /* 4096 rounds */
	{
		uint32_t a;
		uint32_t b;
		uint32_t idx;
		uint32_t sz;
		/* Random test on RAM_CMD */
		if (i % 32 == 0)
		{
			if (i != 0) printf("\n");
			printf("[%i] ", i);
		}
		a = wangHash(++hk) & ((128 * 1024) - 1) & ~3ULL;
		b = wangHash(++hk) & ((128 * 1024) - 1) & ~3ULL;
		idx = min(a, b);
		sz = max(a, b) - idx;
		for (j = 0; j < sz; ++j)
			s_BufferWr[j] = wangHash(++hk) & 0xFF;
		memset(s_BufferRd, 0xBABABABA, sizeof(s_BufferRd));
		EVE_Hal_wrMem(phost, RAM_CMD + (idx & (4096 - 1)), s_BufferWr, sz);
		printf("O");
		EVE_Hal_rdMem(phost, s_BufferRd, RAM_CMD, 4096);
		printf("I");

		/* IMPORANT: Reads and writes on RAM_CMD loop around.
		Only the last 4096 will be present, but must be valid! */
		for (j = (sz > 4096 ? sz - 4096 : 0); j < sz; ++j)
		{
			uint32_t rdi = (idx + j) & (4096 - 1);
			if (s_BufferWr[j] != s_BufferRd[rdi])
			{
				printf("Failed RAM_G test %i, addr %i with sz %i\n", i, idx, sz);
				++failures;
				eve_debug_break();
			}
		}
	}
	printf("\n");
	printf("\n");

	EVE_Hal_displayMessageZ(phost, "Testing RAM_CMD reads and multiple writes at various sizes...");
	printf("Testing RAM_CMD reads and multiple writes at various sizes...\n");
	for (i = 0; i < 4096; ++i) /* 4096 rounds */
	{
		uint32_t a;
		uint32_t b;
		uint32_t idx;
		uint32_t sz;
		/* Random test on RAM_CMD */
		if (i % 32 == 0)
		{
			if (i != 0) printf("\n");
			printf("[%i] ", i);
		}
		a = wangHash(++hk) & ((128 * 1024) - 1) & ~3ULL;
		b = wangHash(++hk) & ((128 * 1024) - 1) & ~3ULL;
		idx = min(a, b);
		sz = max(a, b) - idx;
		for (j = 0; j < sz; ++j)
			s_BufferWr[j] = wangHash(++hk) & 0xFF;
		memset(s_BufferRd, 0xBABABABA, sizeof(s_BufferRd));
		EVE_Hal_startTransfer(phost, EVE_TRANSFER_WRITE, RAM_CMD + (idx & (4096 - 1)));
		for (j = 0; j < sz; ++j)
			EVE_Hal_transfer8(phost, s_BufferWr[j]);
		EVE_Hal_endTransfer(phost);
		printf("O");
		EVE_Hal_rdMem(phost, s_BufferRd, RAM_CMD, 4096);
		printf("I");

		/* IMPORANT: Reads and writes on RAM_CMD loop around.
		Only the last 4096 will be present, but must be valid! */
		for (j = (sz > 4096 ? sz - 4096 : 0); j < sz; ++j)
		{
			uint32_t rdi = (idx + j) & (4096 - 1);
			if (s_BufferWr[j] != s_BufferRd[rdi])
			{
				printf("Failed RAM_G test %i, addr %i with sz %i\n", i, idx, sz);
				++failures;
				eve_debug_break();
			}
		}
	}
	printf("\n");
	printf("\n");

#endif
	if (EVE_Hal_supportCmdB(phost))
	{
		uint16_t wp;

		EVE_Hal_displayMessageZ(phost, "Testing RAM_CMDB...");
		printf("Testing RAM_CMDB...\n");

		wp = EVE_Cmd_wp(phost);
		for (i = 0; i < 1023; ++i)
		{
			s_BufferCmdWr[i] = COLOR_RGB(wangHash(++hk) & 0xFF, wangHash(++hk) & 0xFF, wangHash(++hk) & 0xFF);
			EVE_Hal_wr32(phost, REG_CMDB_WRITE, s_BufferCmdWr[i]);
		}
		printf("O");

		EVE_Cmd_waitFlush(phost);
		// EVE_Hal_rdMem(phost, (void *)s_BufferCmdRd, RAM_CMD, 4096);
		EVE_Hal_startTransfer(phost, EVE_TRANSFER_READ, RAM_CMD);
		for (i = 0; i < 1024; ++i)
			s_BufferCmdRd[i] = EVE_Hal_transfer32(phost, 0);
		/*
		// TODO: Verify on hardware if reading wraps too, it doesn't in the emulator
		EVE_Hal_startTransfer(phost, EVE_TRANSFER_READ, RAM_CMD + 2048);
		for (int i = 0; i < 1024; ++i)
			s_BufferCmdRd[(i + 512) & (1024 - 1)] = EVE_Hal_transfer32(phost, 0);
		*/
		EVE_Hal_endTransfer(phost);
		printf("I");
		for (i = 0; i < 1023; ++i)
		{
			if (s_BufferCmdWr[i] != s_BufferCmdRd[((wp + i * 4) & (4096 - 1)) / 4])
			{
				printf("Failed RAM_CMDB test at idx %i\n", i);
				++failures;
				eve_debug_break();
			}
		}
		printf("\n");
		printf("\n");
	}

	sprintf((char *)(void *)s_BufferCmdWr, "Testing done, %i failures", failures);
	printf("%s", (char *)(void *)s_BufferCmdWr);
	EVE_Hal_displayMessageZ(phost, (void *)s_BufferCmdWr);
	printf("\n");
	return false;
}

int main()
{
	EVE_HalContext *phost = &s_Host;

	/* Initialize HAL */
	EVE_Hal_initialize();

	/* Open the device interactively */
	// if (EVE_Util_openDeviceInteractive(phost, L"Flash.bin"))
	if (EVE_Util_openDeviceInteractive(phost, NULL))
	{

		if (EVE_Util_bootupConfigInteractive(phost, EVE_DISPLAY_DEFAULT))
		{
			while (s_Running && phost->Status != EVE_STATUS_CLOSED) // TODO: Deal with emulator closing (EVE_STATUS_CLOSED/EVE_STATUS_ERROR?)
			{
				if (!loop(phost))
					s_Running = false;
			}
			if (phost->Status == EVE_STATUS_CLOSED)
			{
				printf("Device closed unexpectedly\n");
			}
			EVE_Util_shutdown(phost);
		}
		else
		{
			printf("Failed to bootup the device\n");
		}

		EVE_Hal_close(phost);
	}
	else
	{
		printf("Failed to open HAL\n");
	}
	EVE_Hal_release();
}

/* end of file */
