// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "hal/AnalogGyro.h"

#include <string>

#include "HALInitializer.h"
#include "HALInternal.h"
#include "PortsInternal.h"
#include "hal/AnalogAccumulator.h"
#include "hal/Errors.h"
#include "hal/handles/IndexedHandleResource.h"
#include "mockdata/AnalogGyroDataInternal.h"

namespace {
struct AnalogGyro {
  HAL_AnalogInputHandle handle;
  uint8_t index;
  std::string previousAllocation;
};
}  // namespace

using namespace hal;

static IndexedHandleResource<HAL_GyroHandle, AnalogGyro, kNumAccumulators,
                             HAL_HandleEnum::AnalogGyro>* analogGyroHandles;

namespace hal::init {
void InitializeAnalogGyro() {
  static IndexedHandleResource<HAL_GyroHandle, AnalogGyro, kNumAccumulators,
                               HAL_HandleEnum::AnalogGyro>
      agH;
  analogGyroHandles = &agH;
}
}  // namespace hal::init

extern "C" {
HAL_GyroHandle HAL_InitializeAnalogGyro(HAL_AnalogInputHandle analogHandle,
                                        const char* allocationLocation,
                                        int32_t* status) {
  hal::init::CheckInit();
  // Handle will be type checked by HAL_IsAccumulatorChannel
  int16_t channel = getHandleIndex(analogHandle);
  if (!HAL_IsAccumulatorChannel(analogHandle, status)) {
    if (*status == 0) {
      *status = HAL_INVALID_ACCUMULATOR_CHANNEL;
      hal::SetLastErrorIndexOutOfRange(status, "Invalid Index for Analog Gyro",
                                       0, kNumAccumulators, channel);
    }
    return HAL_kInvalidHandle;
  }

  HAL_GyroHandle handle;
  auto gyro = analogGyroHandles->Allocate(channel, &handle, status);

  if (*status != 0) {
    if (gyro) {
      hal::SetLastErrorPreviouslyAllocated(status, "Analog Gyro", channel,
                                           gyro->previousAllocation);
    } else {
      hal::SetLastErrorIndexOutOfRange(status, "Invalid Index for Analog Gyro",
                                       0, kNumAccumulators, channel);
    }
    return HAL_kInvalidHandle;  // failed to allocate. Pass error back.
  }

  gyro->handle = analogHandle;
  gyro->index = channel;

  SimAnalogGyroData[channel].initialized = true;

  gyro->previousAllocation = allocationLocation ? allocationLocation : "";

  return handle;
}

void HAL_SetupAnalogGyro(HAL_GyroHandle handle, int32_t* status) {
  // No op
}

void HAL_FreeAnalogGyro(HAL_GyroHandle handle) {
  auto gyro = analogGyroHandles->Get(handle);
  analogGyroHandles->Free(handle);
  if (gyro == nullptr) {
    return;
  }
  SimAnalogGyroData[gyro->index].initialized = false;
}

void HAL_SetAnalogGyroParameters(HAL_GyroHandle handle,
                                 double voltsPerDegreePerSecond, double offset,
                                 int32_t center, int32_t* status) {
  // No op
}

void HAL_SetAnalogGyroVoltsPerDegreePerSecond(HAL_GyroHandle handle,
                                              double voltsPerDegreePerSecond,
                                              int32_t* status) {
  // No op
}

void HAL_ResetAnalogGyro(HAL_GyroHandle handle, int32_t* status) {
  auto gyro = analogGyroHandles->Get(handle);
  if (gyro == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  SimAnalogGyroData[gyro->index].angle = 0.0;
}

void HAL_CalibrateAnalogGyro(HAL_GyroHandle handle, int32_t* status) {
  // Just do a reset
  HAL_ResetAnalogGyro(handle, status);
}

void HAL_SetAnalogGyroDeadband(HAL_GyroHandle handle, double volts,
                               int32_t* status) {
  // No op
}

double HAL_GetAnalogGyroAngle(HAL_GyroHandle handle, int32_t* status) {
  auto gyro = analogGyroHandles->Get(handle);
  if (gyro == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }

  return SimAnalogGyroData[gyro->index].angle;
}

double HAL_GetAnalogGyroRate(HAL_GyroHandle handle, int32_t* status) {
  auto gyro = analogGyroHandles->Get(handle);
  if (gyro == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }

  return SimAnalogGyroData[gyro->index].rate;
}

double HAL_GetAnalogGyroOffset(HAL_GyroHandle handle, int32_t* status) {
  return 0.0;
}

int32_t HAL_GetAnalogGyroCenter(HAL_GyroHandle handle, int32_t* status) {
  return 0;
}
}  // extern "C"
