#ifndef ESD_CO_CMD__H
#define ESD_CO_CMD__H

#include "FT_Gpu_Hal.h"
#include "Gpu_Hal.h"
#include "EVE_Platform.h"

#include "Gpu_CoCmd.h"

Flash_Cmd_Status_t FlashHelper_Update(EVE_HalContext *phost, uint32_t dest_flash, uint32_t src_ram, uint32_t num);
Flash_Cmd_Status_t FlashHelper_Read(EVE_HalContext *phost, uint32_t dest_ram, uint32_t src_flash, uint32_t num, uint8_t *read_data);
ft_void_t FlashHelper_Erase(EVE_HalContext *phost);
uint8_t FlashHelper_GetState(EVE_HalContext *phost);
ft_void_t FlashHelper_ClearCache(EVE_HalContext *phost);
Flash_Cmd_Status_t FlashHelper_Write(EVE_HalContext *phost, uint32_t dest_flash, uint32_t num, const uint8_t *write_data);
uint32_t FlashHelper_SwitchState(EVE_HalContext *phost, uint8_t nextState);
uint32_t FlashHelper_SwitchFullMode(EVE_HalContext *phost);
ft_void_t FlashHelper_flashWriteExt(EVE_HalContext *phost, uint32_t dest, uint32_t num, const uint8_t *data);

ft_bool_t Esd_Calibrate(EVE_HalContext *phost);

ft_void_t Ft_QueueCmd(uint32_t cmd);
ft_void_t Ft_QueueString(const char *s);
ft_void_t Ft_FlushCmd(EVE_HalContext *phost);
ft_void_t Ft_Gpu_HorizontalScanoutFilter(EVE_HalContext *phost, uint32_t physical_W, uint32_t physical_H);
ft_void_t Ft_Gpu_CoCmd_FlashWriteExt(EVE_HalContext *phost, uint32_t dest, uint32_t num, const uint8_t *data);

void fadeout(EVE_HalContext *phost);
void fadein(EVE_HalContext *phost);

void Fifo_Init(Fifo_t *pFifo, uint32_t StartAddress, uint32_t Length, uint32_t HWReadRegAddress, uint32_t HWWriteRegAddress);
void Fifo_Update(EVE_HalContext *host, Fifo_t *pFifo);
uint32_t Fifo_Write(EVE_HalContext *host, Fifo_t *pFifo, const uint8_t *buffer, uint32_t NumbytetoWrite);
void Fifo_WriteWait(EVE_HalContext *host, Fifo_t *pFifo, const uint8_t *buffer, uint32_t Numbyte);
void Fifo_Write32(EVE_HalContext *host, Fifo_t *pFifo, uint32_t WriteWord);
uint32_t Fifo_GetFreeSpace(EVE_HalContext *host, Fifo_t *pFifo);

void App_WrDl_Buffer(Gpu_Hal_Context_t *phost, uint32_t cmd);
void App_Flush_DL_Buffer(const Gpu_Hal_Context_t *phost);
void App_Set_DlBuffer_Index(uint32_t index);
void GPU_DLSwap(Gpu_Hal_Context_t *phost, uint8_t DL_Swap_Type);
int32_t FlashHelper_GetSizeMB(Gpu_Hal_Context_t* host);

#endif /* ESD_CO_CMD__H */
/* end of file */
