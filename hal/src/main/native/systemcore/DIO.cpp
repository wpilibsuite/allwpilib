// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "hal/DIO.h"

#include <cmath>
#include <cstdio>
#include <thread>

#include "HALInitializer.h"
#include "HALInternal.h"
#include "PortsInternal.h"
#include "hal/Errors.h"
#include "hal/cpp/fpga_clock.h"
#include "hal/handles/HandlesInternal.h"
#include "hal/handles/LimitedHandleResource.h"

using namespace hal;

namespace hal::init {
void InitializeDIO() {}
}  // namespace hal::init

extern "C" {

HAL_DigitalHandle HAL_InitializeDIOPort(HAL_PortHandle portHandle,
                                        HAL_Bool input,
                                        const char* allocationLocation,
                                        int32_t* status) {
  hal::init::CheckInit();
  *status = HAL_HANDLE_ERROR;
  return HAL_kInvalidHandle;
}

HAL_Bool HAL_CheckDIOChannel(int32_t channel) {
  return channel < kNumDigitalChannels && channel >= 0;
}

void HAL_FreeDIOPort(HAL_DigitalHandle dioPortHandle) {}

void HAL_SetDIOSimDevice(HAL_DigitalHandle handle, HAL_SimDeviceHandle device) {
}

HAL_DigitalPWMHandle HAL_AllocateDigitalPWM(int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return HAL_kInvalidHandle;
}

void HAL_FreeDigitalPWM(HAL_DigitalPWMHandle pwmGenerator) {}

void HAL_SetDigitalPWMRate(double rate, int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return;
}

void HAL_SetDigitalPWMDutyCycle(HAL_DigitalPWMHandle pwmGenerator,
                                double dutyCycle, int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return;
}

void HAL_SetDigitalPWMPPS(HAL_DigitalPWMHandle pwmGenerator, double dutyCycle,
                          int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return;
}

void HAL_SetDigitalPWMOutputChannel(HAL_DigitalPWMHandle pwmGenerator,
                                    int32_t channel, int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return;
}

void HAL_SetDIO(HAL_DigitalHandle dioPortHandle, HAL_Bool value,
                int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return;
}

void HAL_SetDIODirection(HAL_DigitalHandle dioPortHandle, HAL_Bool input,
                         int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return;
}

HAL_Bool HAL_GetDIO(HAL_DigitalHandle dioPortHandle, int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return false;
}

HAL_Bool HAL_GetDIODirection(HAL_DigitalHandle dioPortHandle, int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return false;
}

void HAL_Pulse(HAL_DigitalHandle dioPortHandle, double pulseLength,
               int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return;
}

void HAL_PulseMultiple(uint32_t channelMask, double pulseLength,
                       int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return;
}

HAL_Bool HAL_IsPulsing(HAL_DigitalHandle dioPortHandle, int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return false;
}

HAL_Bool HAL_IsAnyPulsing(int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return false;
}

void HAL_SetFilterSelect(HAL_DigitalHandle dioPortHandle, int32_t filterIndex,
                         int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return;
}

int32_t HAL_GetFilterSelect(HAL_DigitalHandle dioPortHandle, int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return 0;
}

void HAL_SetFilterPeriod(int32_t filterIndex, int64_t value, int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return;
}

int64_t HAL_GetFilterPeriod(int32_t filterIndex, int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return 0;
}

}  // extern "C"
