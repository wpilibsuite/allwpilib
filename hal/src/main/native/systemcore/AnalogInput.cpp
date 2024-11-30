// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "hal/AnalogInput.h"

#include <wpi/mutex.h>

#include "HALInitializer.h"
#include "HALInternal.h"
#include "PortsInternal.h"
#include "hal/AnalogAccumulator.h"
#include "hal/Errors.h"
#include "hal/handles/HandlesInternal.h"

namespace hal::init {
void InitializeAnalogInput() {}
}  // namespace hal::init

using namespace hal;

extern "C" {

HAL_AnalogInputHandle HAL_InitializeAnalogInputPort(
    HAL_PortHandle portHandle, const char* allocationLocation,
    int32_t* status) {
  hal::init::CheckInit();
  *status = HAL_HANDLE_ERROR;
  return HAL_kInvalidHandle;
}

void HAL_FreeAnalogInputPort(HAL_AnalogInputHandle analogPortHandle) {}

HAL_Bool HAL_CheckAnalogModule(int32_t module) {
  return module == 1;
}

HAL_Bool HAL_CheckAnalogInputChannel(int32_t channel) {
  return channel < kNumAnalogInputs && channel >= 0;
}

void HAL_SetAnalogInputSimDevice(HAL_AnalogInputHandle handle,
                                 HAL_SimDeviceHandle device) {}

void HAL_SetAnalogSampleRate(double samplesPerSecond, int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return;
}

double HAL_GetAnalogSampleRate(int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return 0;
}

void HAL_SetAnalogAverageBits(HAL_AnalogInputHandle analogPortHandle,
                              int32_t bits, int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return;
}

int32_t HAL_GetAnalogAverageBits(HAL_AnalogInputHandle analogPortHandle,
                                 int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return 0;
}

void HAL_SetAnalogOversampleBits(HAL_AnalogInputHandle analogPortHandle,
                                 int32_t bits, int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return;
}

int32_t HAL_GetAnalogOversampleBits(HAL_AnalogInputHandle analogPortHandle,
                                    int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return 0;
}

int32_t HAL_GetAnalogValue(HAL_AnalogInputHandle analogPortHandle,
                           int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return 0;
}

int32_t HAL_GetAnalogAverageValue(HAL_AnalogInputHandle analogPortHandle,
                                  int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return 0;
}

int32_t HAL_GetAnalogVoltsToValue(HAL_AnalogInputHandle analogPortHandle,
                                  double voltage, int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return 0;
}

double HAL_GetAnalogVoltage(HAL_AnalogInputHandle analogPortHandle,
                            int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return 0;
}

double HAL_GetAnalogValueToVolts(HAL_AnalogInputHandle analogPortHandle,
                                 int32_t rawValue, int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return 0;
}

double HAL_GetAnalogAverageVoltage(HAL_AnalogInputHandle analogPortHandle,
                                   int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return 0;
}

int32_t HAL_GetAnalogLSBWeight(HAL_AnalogInputHandle analogPortHandle,
                               int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return 0;
}

int32_t HAL_GetAnalogOffset(HAL_AnalogInputHandle analogPortHandle,
                            int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return 0;
}

}  // extern "C"
