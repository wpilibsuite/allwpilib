/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <stdint.h>

#include "HAL/cpp/priority_mutex.h"

priority_recursive_mutex& spiGetSemaphore(uint8_t port);

extern "C" {
void HAL_SpiInitialize(uint8_t port, int32_t* status);
int32_t HAL_SpiTransaction(uint8_t port, uint8_t* dataToSend,
                           uint8_t* dataReceived, uint8_t size);
int32_t HAL_SpiWrite(uint8_t port, uint8_t* dataToSend, uint8_t sendSize);
int32_t HAL_SpiRead(uint8_t port, uint8_t* buffer, uint8_t count);
void HAL_SpiClose(uint8_t port);
void HAL_SpiSetSpeed(uint8_t port, uint32_t speed);
void HAL_SpiSetOpts(uint8_t port, int msb_first, int sample_on_trailing,
                    int clk_idle_high);
void HAL_SpiSetChipSelectActiveHigh(uint8_t port, int32_t* status);
void HAL_SpiSetChipSelectActiveLow(uint8_t port, int32_t* status);
int32_t HAL_SpiGetHandle(uint8_t port);
void HAL_SpiSetHandle(uint8_t port, int32_t handle);

void HAL_SpiInitAccumulator(uint8_t port, uint32_t period, uint32_t cmd,
                            uint8_t xfer_size, uint32_t valid_mask,
                            uint32_t valid_value, uint8_t data_shift,
                            uint8_t data_size, bool is_signed, bool big_endian,
                            int32_t* status);
void HAL_SpiFreeAccumulator(uint8_t port, int32_t* status);
void HAL_SpiResetAccumulator(uint8_t port, int32_t* status);
void HAL_SpiSetAccumulatorCenter(uint8_t port, int32_t center, int32_t* status);
void HAL_SpiSetAccumulatorDeadband(uint8_t port, int32_t deadband,
                                   int32_t* status);
int32_t HAL_SpiGetAccumulatorLastValue(uint8_t port, int32_t* status);
int64_t HAL_SpiGetAccumulatorValue(uint8_t port, int32_t* status);
uint32_t HAL_SpiGetAccumulatorCount(uint8_t port, int32_t* status);
double HAL_SpiGetAccumulatorAverage(uint8_t port, int32_t* status);
void HAL_SpiGetAccumulatorOutput(uint8_t port, int64_t* value, uint32_t* count,
                                 int32_t* status);
}
