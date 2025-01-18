// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "hal/AddressableLED.h"

#include <cstring>
#include <memory>

#include <fmt/format.h>

#include "HALInitializer.h"
#include "hal/Errors.h"

using namespace hal;

namespace hal::init {
void InitializeAddressableLED() {}
}  // namespace hal::init

extern "C" {

HAL_AddressableLEDHandle HAL_InitializeAddressableLED(
    HAL_DigitalHandle outputPort, int32_t* status) {
  hal::init::CheckInit();

  *status = HAL_HANDLE_ERROR;
  return HAL_kInvalidHandle;
}

void HAL_FreeAddressableLED(HAL_AddressableLEDHandle handle) {}

void HAL_SetAddressableLEDOutputPort(HAL_AddressableLEDHandle handle,
                                     HAL_DigitalHandle outputPort,
                                     int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return;
}

void HAL_SetAddressableLEDLength(HAL_AddressableLEDHandle handle,
                                 int32_t length, int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return;
}

static_assert(sizeof(HAL_AddressableLEDData) == sizeof(uint32_t),
              "LED Data must be 32 bit");

void HAL_WriteAddressableLEDData(HAL_AddressableLEDHandle handle,
                                 const struct HAL_AddressableLEDData* data,
                                 int32_t length, int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return;
}

void HAL_SetAddressableLEDBitTiming(HAL_AddressableLEDHandle handle,
                                    int32_t highTime0NanoSeconds,
                                    int32_t lowTime0NanoSeconds,
                                    int32_t highTime1NanoSeconds,
                                    int32_t lowTime1NanoSeconds,
                                    int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return;
}

void HAL_SetAddressableLEDSyncTime(HAL_AddressableLEDHandle handle,
                                   int32_t syncTimeMicroSeconds,
                                   int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return;
}

void HAL_StartAddressableLEDOutput(HAL_AddressableLEDHandle handle,
                                   int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return;
}

void HAL_StopAddressableLEDOutput(HAL_AddressableLEDHandle handle,
                                  int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return;
}
}  // extern "C"
