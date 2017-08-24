/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2017 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "HAL/AnalogGyro.h"

#include <chrono>
#include <thread>

#include "AnalogInternal.h"
#include "HAL/AnalogAccumulator.h"
#include "HAL/AnalogInput.h"
#include "HAL/handles/IndexedHandleResource.h"
#include "MockData/AnalogGyroDataInternal.h"

namespace {
struct AnalogGyro {
  HAL_AnalogInputHandle handle;
  uint8_t index;
};
}

using namespace hal;

static IndexedHandleResource<HAL_GyroHandle, AnalogGyro, kNumAccumulators,
                             HAL_HandleEnum::AnalogGyro>
    analogGyroHandles;

extern "C" {
HAL_GyroHandle HAL_InitializeAnalogGyro(HAL_AnalogInputHandle analogHandle,
                                        int32_t* status) {
  if (!HAL_IsAccumulatorChannel(analogHandle, status)) {
    if (*status == 0) {
      *status = HAL_INVALID_ACCUMULATOR_CHANNEL;
    }
    return HAL_kInvalidHandle;
  }

  // handle known to be correct, so no need to type check
  int16_t channel = getHandleIndex(analogHandle);

  auto handle = analogGyroHandles.Allocate(channel, status);

  if (*status != 0)
    return HAL_kInvalidHandle;  // failed to allocate. Pass error back.

  // Initialize port structure
  auto gyro = analogGyroHandles.Get(handle);
  if (gyro == nullptr) {  // would only error on thread issue
    *status = HAL_HANDLE_ERROR;
    return HAL_kInvalidHandle;
  }

  gyro->handle = analogHandle;
  gyro->index = channel;

  SimAnalogGyroData[channel].SetInitialized(true);

  return handle;
}

void HAL_SetupAnalogGyro(HAL_GyroHandle handle, int32_t* status) {
  // No op
}

void HAL_FreeAnalogGyro(HAL_GyroHandle handle) {
  auto gyro = analogGyroHandles.Get(handle);
  analogGyroHandles.Free(handle);
  if (gyro == nullptr) return;
  SimAnalogGyroData[gyro->index].SetInitialized(false);
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
  auto gyro = analogGyroHandles.Get(handle);
  if (gyro == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  SimAnalogGyroData[gyro->index].SetAngle(0.0);
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
  auto gyro = analogGyroHandles.Get(handle);
  if (gyro == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }

  return SimAnalogGyroData[gyro->index].GetAngle();
}

double HAL_GetAnalogGyroRate(HAL_GyroHandle handle, int32_t* status) {
  auto gyro = analogGyroHandles.Get(handle);
  if (gyro == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }

  return SimAnalogGyroData[gyro->index].GetRate();
}

double HAL_GetAnalogGyroOffset(HAL_GyroHandle handle, int32_t* status) {
  return 0.0;
}

int32_t HAL_GetAnalogGyroCenter(HAL_GyroHandle handle, int32_t* status) {
  return 0;
}
}
