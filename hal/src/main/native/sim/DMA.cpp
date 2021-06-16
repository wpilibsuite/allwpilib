// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "hal/DMA.h"

extern "C" {
HAL_DMAHandle HAL_InitializeDMA(int32_t* status) {
  return HAL_kInvalidHandle;
}
void HAL_FreeDMA(HAL_DMAHandle handle) {}

void HAL_SetDMAPause(HAL_DMAHandle handle, HAL_Bool pause, int32_t* status) {}
void HAL_SetDMATimedTrigger(HAL_DMAHandle handle, double periodSeconds,
                            int32_t* status) {}
void HAL_SetDMATimedTriggerCycles(HAL_DMAHandle handle, uint32_t cycles,
                                  int32_t* status) {}

void HAL_AddDMAEncoder(HAL_DMAHandle handle, HAL_EncoderHandle encoderHandle,
                       int32_t* status) {}
void HAL_AddDMAEncoderPeriod(HAL_DMAHandle handle,
                             HAL_EncoderHandle encoderHandle, int32_t* status) {
}
void HAL_AddDMACounter(HAL_DMAHandle handle, HAL_CounterHandle counterHandle,
                       int32_t* status) {}
void HAL_AddDMACounterPeriod(HAL_DMAHandle handle,
                             HAL_CounterHandle counterHandle, int32_t* status) {
}
void HAL_AddDMADigitalSource(HAL_DMAHandle handle,
                             HAL_Handle digitalSourceHandle, int32_t* status) {}
void HAL_AddDMAAnalogInput(HAL_DMAHandle handle,
                           HAL_AnalogInputHandle aInHandle, int32_t* status) {}

void HAL_AddDMAAveragedAnalogInput(HAL_DMAHandle handle,
                                   HAL_AnalogInputHandle aInHandle,
                                   int32_t* status) {}

void HAL_AddDMAAnalogAccumulator(HAL_DMAHandle handle,
                                 HAL_AnalogInputHandle aInHandle,
                                 int32_t* status) {}

void HAL_AddDMADutyCycle(HAL_DMAHandle handle,
                         HAL_DutyCycleHandle dutyCycleHandle, int32_t* status) {
}

int32_t HAL_SetDMAExternalTrigger(HAL_DMAHandle handle,
                                  HAL_Handle digitalSourceHandle,
                                  HAL_AnalogTriggerType analogTriggerType,
                                  HAL_Bool rising, HAL_Bool falling,
                                  int32_t* status) {
  return 0;
}

void HAL_ClearDMASensors(HAL_DMAHandle handle, int32_t* status) {}
void HAL_ClearDMAExternalTriggers(HAL_DMAHandle handle, int32_t* status) {}

void HAL_StartDMA(HAL_DMAHandle handle, int32_t queueDepth, int32_t* status) {}
void HAL_StopDMA(HAL_DMAHandle handle, int32_t* status) {}

void* HAL_GetDMADirectPointer(HAL_DMAHandle handle) {
  return nullptr;
}

enum HAL_DMAReadStatus HAL_ReadDMADirect(void* dmaPointer,
                                         HAL_DMASample* dmaSample,
                                         double timeoutSeconds,
                                         int32_t* remainingOut,
                                         int32_t* status) {
  return HAL_DMA_ERROR;
}

enum HAL_DMAReadStatus HAL_ReadDMA(HAL_DMAHandle handle,
                                   HAL_DMASample* dmaSample,
                                   double timeoutSeconds, int32_t* remainingOut,
                                   int32_t* status) {
  return HAL_DMA_ERROR;
}

// Sampling Code
uint64_t HAL_GetDMASampleTime(const HAL_DMASample* dmaSample, int32_t* status) {
  return 0;
}

int32_t HAL_GetDMASampleEncoderRaw(const HAL_DMASample* dmaSample,
                                   HAL_EncoderHandle encoderHandle,
                                   int32_t* status) {
  return 0;
}

int32_t HAL_GetDMASampleCounter(const HAL_DMASample* dmaSample,
                                HAL_CounterHandle counterHandle,
                                int32_t* status) {
  return 0;
}

int32_t HAL_GetDMASampleEncoderPeriodRaw(const HAL_DMASample* dmaSample,
                                         HAL_EncoderHandle encoderHandle,
                                         int32_t* status) {
  return 0;
}

int32_t HAL_GetDMASampleCounterPeriod(const HAL_DMASample* dmaSample,
                                      HAL_CounterHandle counterHandle,
                                      int32_t* status) {
  return 0;
}
HAL_Bool HAL_GetDMASampleDigitalSource(const HAL_DMASample* dmaSample,
                                       HAL_Handle dSourceHandle,
                                       int32_t* status) {
  return 0;
}
int32_t HAL_GetDMASampleAnalogInputRaw(const HAL_DMASample* dmaSample,
                                       HAL_AnalogInputHandle aInHandle,
                                       int32_t* status) {
  return 0;
}

int32_t HAL_GetDMASampleAveragedAnalogInputRaw(const HAL_DMASample* dmaSample,
                                               HAL_AnalogInputHandle aInHandle,
                                               int32_t* status) {
  return 0;
}

void HAL_GetDMASampleAnalogAccumulator(const HAL_DMASample* dmaSample,
                                       HAL_AnalogInputHandle aInHandle,
                                       int64_t* count, int64_t* value,
                                       int32_t* status) {}

int32_t HAL_GetDMASampleDutyCycleOutputRaw(const HAL_DMASample* dmaSample,
                                           HAL_DutyCycleHandle dutyCycleHandle,
                                           int32_t* status) {
  return 0;
}
}  // extern "C"
