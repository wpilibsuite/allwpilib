/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <stdint.h>

#include "hal/AnalogTrigger.h"
#include "hal/Types.h"

// clang-format off
/**
 * The DMA Read Status.
 */
HAL_ENUM(HAL_DMAReadStatus ) {
  HAL_DMA_OK = 1,
  HAL_DMA_TIMEOUT = 2,
  HAL_DMA_ERROR = 3,
};
// clang-format on

struct HAL_DMASample {
  uint32_t readBuffer[64];
  uint32_t channelOffsets[20];
  uint64_t timeStamp;
  uint32_t captureSize;
  uint8_t triggerChannels;
};

#ifdef __cplusplus
extern "C" {
#endif

HAL_DMAHandle HAL_InitializeDMA(int32_t* status);
void HAL_FreeDMA(HAL_DMAHandle handle);

void HAL_SetDMAPause(HAL_DMAHandle handle, HAL_Bool pause, int32_t* status);
void HAL_SetDMARate(HAL_DMAHandle handle, int32_t cycles, int32_t* status);

void HAL_AddDMAEncoder(HAL_DMAHandle handle, HAL_EncoderHandle encoderHandle,
                       int32_t* status);
void HAL_AddDMACounter(HAL_DMAHandle handle, HAL_CounterHandle counterHandle,
                       int32_t* status);
void HAL_AddDMADigitalSource(HAL_DMAHandle handle,
                             HAL_Handle digitalSourceHandle, int32_t* status);
void HAL_AddDMAAnalogInput(HAL_DMAHandle handle,
                           HAL_AnalogInputHandle aInHandle, int32_t* status);

void HAL_SetDMAExternalTrigger(HAL_DMAHandle handle,
                               HAL_Handle digitalSourceHandle,
                               HAL_AnalogTriggerType analogTriggerType,
                               HAL_Bool rising, HAL_Bool falling,
                               int32_t* status);

void HAL_StartDMA(HAL_DMAHandle handle, int32_t queueDepth, int32_t* status);
void HAL_StopDMA(HAL_DMAHandle handle, int32_t* status);

enum HAL_DMAReadStatus HAL_ReadDMA(HAL_DMAHandle handle,
                                   HAL_DMASample* dmaSample,
                                   int32_t timeoutMs, int32_t* remainingOut,
                                   int32_t* status);

// Sampling Code
double HAL_GetDMASampleTimestamp(const HAL_DMASample* dmaSample,
                                 int32_t* status);
uint64_t HAL_GetDMASampleTime(const HAL_DMASample* dmaSample,
                              int32_t* status);

int32_t HAL_GetDMASampleEncoder(const HAL_DMASample* dmaSample,
                                HAL_EncoderHandle encoderHandle,
                                int32_t* status);
int32_t HAL_GetDMASampleEncoderRaw(const HAL_DMASample* dmaSample,
                                   HAL_EncoderHandle encoderHandle,
                                   int32_t* status);
int32_t HAL_GetDMASampleCounter(const HAL_DMASample* dmaSample,
                                HAL_CounterHandle counterHandle,
                                int32_t* status);
HAL_Bool HAL_GetDMASampleDigitalSource(const HAL_DMASample* dmaSample,
                                       HAL_Handle dSourceHandle,
                                       int32_t* status);
int32_t HAL_GetDMASampleAnalogInputRaw(const HAL_DMASample* dmaSample,
                                       HAL_AnalogInputHandle aInHandle,
                                       int32_t* status);
double HAL_GetDMASampleAnalogInputVoltage(const HAL_DMASample* dmaSample,
                                          HAL_AnalogInputHandle aInHandle,
                                          int32_t* status);

#ifdef __cplusplus
}  // extern "C"
#endif
