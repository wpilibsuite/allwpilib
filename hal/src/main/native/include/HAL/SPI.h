/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <stdint.h>

#include "HAL/Types.h"

enum HAL_SPIPort : int32_t {
  HAL_SPI_kOnboardCS0 = 0,
  HAL_SPI_kOnboardCS1,
  HAL_SPI_kOnboardCS2,
  HAL_SPI_kOnboardCS3,
  HAL_SPI_kMXP
};

#ifdef __cplusplus
extern "C" {
#endif

void HAL_InitializeSPI(HAL_SPIPort port, int32_t* status);
int32_t HAL_TransactionSPI(HAL_SPIPort port, uint8_t* dataToSend,
                           uint8_t* dataReceived, int32_t size);
int32_t HAL_WriteSPI(HAL_SPIPort port, uint8_t* dataToSend, int32_t sendSize);
int32_t HAL_ReadSPI(HAL_SPIPort port, uint8_t* buffer, int32_t count);
void HAL_CloseSPI(HAL_SPIPort port);
void HAL_SetSPISpeed(HAL_SPIPort port, int32_t speed);
void HAL_SetSPIOpts(HAL_SPIPort port, HAL_Bool msbFirst,
                    HAL_Bool sampleOnTrailing, HAL_Bool clkIdleHigh);
void HAL_SetSPIChipSelectActiveHigh(HAL_SPIPort port, int32_t* status);
void HAL_SetSPIChipSelectActiveLow(HAL_SPIPort port, int32_t* status);
int32_t HAL_GetSPIHandle(HAL_SPIPort port);
void HAL_SetSPIHandle(HAL_SPIPort port, int32_t handle);

void HAL_InitSPIAccumulator(HAL_SPIPort port, int32_t period, int32_t cmd,
                            int32_t xferSize, int32_t validMask,
                            int32_t validValue, int32_t dataShift,
                            int32_t dataSize, HAL_Bool isSigned,
                            HAL_Bool bigEndian, int32_t* status);
void HAL_FreeSPIAccumulator(HAL_SPIPort port, int32_t* status);
void HAL_ResetSPIAccumulator(HAL_SPIPort port, int32_t* status);
void HAL_SetSPIAccumulatorCenter(HAL_SPIPort port, int32_t center,
                                 int32_t* status);
void HAL_SetSPIAccumulatorDeadband(HAL_SPIPort port, int32_t deadband,
                                   int32_t* status);
int32_t HAL_GetSPIAccumulatorLastValue(HAL_SPIPort port, int32_t* status);
int64_t HAL_GetSPIAccumulatorValue(HAL_SPIPort port, int32_t* status);
int64_t HAL_GetSPIAccumulatorCount(HAL_SPIPort port, int32_t* status);
double HAL_GetSPIAccumulatorAverage(HAL_SPIPort port, int32_t* status);
void HAL_GetSPIAccumulatorOutput(HAL_SPIPort port, int64_t* value,
                                 int64_t* count, int32_t* status);
#ifdef __cplusplus
}
#endif
