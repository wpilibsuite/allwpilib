/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <stdint.h>

extern "C" {
void HAL_InitializeSPI(uint8_t port, int32_t* status);
int32_t HAL_TransactionSPI(uint8_t port, uint8_t* dataToSend,
                           uint8_t* dataReceived, uint8_t size);
int32_t HAL_WriteSPI(uint8_t port, uint8_t* dataToSend, uint8_t sendSize);
int32_t HAL_ReadSPI(uint8_t port, uint8_t* buffer, uint8_t count);
void HAL_CloseSPI(uint8_t port);
void HAL_SetSPISpeed(uint8_t port, uint32_t speed);
void HAL_SetSPIOpts(uint8_t port, int msb_first, int sample_on_trailing,
                    int clk_idle_high);
void HAL_SetSPIChipSelectActiveHigh(uint8_t port, int32_t* status);
void HAL_SetSPIChipSelectActiveLow(uint8_t port, int32_t* status);
int32_t HAL_GetSPIHandle(uint8_t port);
void HAL_SetSPIHandle(uint8_t port, int32_t handle);

void HAL_InitSPIAccumulator(uint8_t port, uint32_t period, uint32_t cmd,
                            uint8_t xfer_size, uint32_t valid_mask,
                            uint32_t valid_value, uint8_t data_shift,
                            uint8_t data_size, bool is_signed, bool big_endian,
                            int32_t* status);
void HAL_FreeSPIAccumulator(uint8_t port, int32_t* status);
void HAL_ResetSPIAccumulator(uint8_t port, int32_t* status);
void HAL_SetSPIAccumulatorCenter(uint8_t port, int32_t center, int32_t* status);
void HAL_SetSPIAccumulatorDeadband(uint8_t port, int32_t deadband,
                                   int32_t* status);
int32_t HAL_GetSPIAccumulatorLastValue(uint8_t port, int32_t* status);
int64_t HAL_GetSPIAccumulatorValue(uint8_t port, int32_t* status);
uint32_t HAL_GetSPIAccumulatorCount(uint8_t port, int32_t* status);
double HAL_GetSPIAccumulatorAverage(uint8_t port, int32_t* status);
void HAL_GetSPIAccumulatorOutput(uint8_t port, int64_t* value, uint32_t* count,
                                 int32_t* status);
}
