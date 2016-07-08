/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
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
#include "handles/IndexedHandleResource.h"

namespace {
struct AnalogGyro {
  HalAnalogInputHandle handle;
  float voltsPerDegreePerSecond;
  float offset;
  uint32_t center;
};
}

static constexpr uint32_t kOversampleBits = 10;
static constexpr uint32_t kAverageBits = 0;
static constexpr float kSamplesPerSecond = 50.0;
static constexpr float kCalibrationSampleTime = 5.0;
static constexpr float kDefaultVoltsPerDegreePerSecond = 0.007;

using namespace hal;

static IndexedHandleResource<HalGyroHandle, AnalogGyro, kNumAccumulators,
                             HalHandleEnum::AnalogGyro>
    analogGyroHandles;

static void Wait(double seconds) {
  if (seconds < 0.0) return;
  std::this_thread::sleep_for(std::chrono::duration<double>(seconds));
}

extern "C" {
HalGyroHandle initializeAnalogGyro(HalAnalogInputHandle analog_handle,
                                   int32_t* status) {
  if (!isAccumulatorChannel(analog_handle, status)) {
    if (*status == 0) {
      *status = PARAMETER_OUT_OF_RANGE;
    }
    return HAL_INVALID_HANDLE;
  }

  // handle known to be correct, so no need to type check
  int16_t channel = getHandleIndex(analog_handle);

  auto handle = analogGyroHandles.Allocate(channel, status);

  if (*status != 0)
    return HAL_INVALID_HANDLE;  // failed to allocate. Pass error back.

  // Initialize port structure
  auto gyro = analogGyroHandles.Get(handle);
  if (gyro == nullptr) {  // would only error on thread issue
    *status = HAL_HANDLE_ERROR;
    return HAL_INVALID_HANDLE;
  }

  gyro->handle = analog_handle;
  gyro->voltsPerDegreePerSecond = 0;
  gyro->offset = 0;
  gyro->center = 0;

  return handle;
}

void setupAnalogGyro(HalGyroHandle handle, int32_t* status) {
  auto gyro = analogGyroHandles.Get(handle);
  if (gyro == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  gyro->voltsPerDegreePerSecond = kDefaultVoltsPerDegreePerSecond;

  setAnalogAverageBits(gyro->handle, kAverageBits, status);
  if (*status != 0) return;
  setAnalogOversampleBits(gyro->handle, kOversampleBits, status);
  if (*status != 0) return;
  float sampleRate =
      kSamplesPerSecond * (1 << (kAverageBits + kOversampleBits));
  setAnalogSampleRate(sampleRate, status);
  if (*status != 0) return;
  Wait(0.1);

  setAnalogGyroDeadband(handle, 0.0f, status);
  if (*status != 0) return;
}

void freeAnalogGyro(HalGyroHandle handle) { analogGyroHandles.Free(handle); }

void setAnalogGyroParameters(HalGyroHandle handle,
                             float voltsPerDegreePerSecond, float offset,
                             uint32_t center, int32_t* status) {
  auto gyro = analogGyroHandles.Get(handle);
  if (gyro == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  gyro->voltsPerDegreePerSecond = voltsPerDegreePerSecond;
  gyro->offset = offset;
  gyro->center = center;
  setAccumulatorCenter(gyro->handle, center, status);
}

void setAnalogGyroVoltsPerDegreePerSecond(HalGyroHandle handle,
                                          float voltsPerDegreePerSecond,
                                          int32_t* status) {
  auto gyro = analogGyroHandles.Get(handle);
  if (gyro == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  gyro->voltsPerDegreePerSecond = voltsPerDegreePerSecond;
}

void resetAnalogGyro(HalGyroHandle handle, int32_t* status) {
  auto gyro = analogGyroHandles.Get(handle);
  if (gyro == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }
  resetAccumulator(gyro->handle, status);
  if (*status != 0) return;

  const float sampleTime = 1.0f / getAnalogSampleRate(status);
  const float overSamples = 1 << getAnalogOversampleBits(gyro->handle, status);
  const float averageSamples = 1 << getAnalogAverageBits(gyro->handle, status);
  if (*status != 0) return;
  Wait(sampleTime * overSamples * averageSamples);
}

void calibrateAnalogGyro(HalGyroHandle handle, int32_t* status) {
  auto gyro = analogGyroHandles.Get(handle);
  if (gyro == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  initAccumulator(gyro->handle, status);
  if (*status != 0) return;
  Wait(kCalibrationSampleTime);

  int64_t value;
  uint32_t count;
  getAccumulatorOutput(gyro->handle, &value, &count, status);
  if (*status != 0) return;

  gyro->center = (uint32_t)((float)value / (float)count + .5);

  gyro->offset = ((float)value / (float)count) - (float)gyro->center;
  setAccumulatorCenter(gyro->handle, gyro->center, status);
  if (*status != 0) return;
  resetAnalogGyro(handle, status);
}

void setAnalogGyroDeadband(HalGyroHandle handle, float volts, int32_t* status) {
  auto gyro = analogGyroHandles.Get(handle);
  if (gyro == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }
  int32_t deadband = volts * 1e9 / getAnalogLSBWeight(gyro->handle, status) *
                     (1 << getAnalogOversampleBits(gyro->handle, status));
  if (*status != 0) return;
  setAccumulatorDeadband(gyro->handle, deadband, status);
}

float getAnalogGyroAngle(HalGyroHandle handle, int32_t* status) {
  auto gyro = analogGyroHandles.Get(handle);
  if (gyro == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }
  int64_t rawValue = 0;
  uint32_t count = 0;
  getAccumulatorOutput(gyro->handle, &rawValue, &count, status);

  int64_t value = rawValue - (int64_t)((float)count * gyro->offset);

  double scaledValue =
      value * 1e-9 * (double)getAnalogLSBWeight(gyro->handle, status) *
      (double)(1 << getAnalogAverageBits(gyro->handle, status)) /
      (getAnalogSampleRate(status) * gyro->voltsPerDegreePerSecond);

  return (float)scaledValue;
}

double getAnalogGyroRate(HalGyroHandle handle, int32_t* status) {
  auto gyro = analogGyroHandles.Get(handle);
  if (gyro == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }

  return (getAnalogAverageValue(gyro->handle, status) -
          ((double)gyro->center + gyro->offset)) *
         1e-9 * getAnalogLSBWeight(gyro->handle, status) /
         ((1 << getAnalogOversampleBits(gyro->handle, status)) *
          gyro->voltsPerDegreePerSecond);
}

float getAnalogGyroOffset(HalGyroHandle handle, int32_t* status) {
  auto gyro = analogGyroHandles.Get(handle);
  if (gyro == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }
  return gyro->offset;
}

uint32_t getAnalogGyroCenter(HalGyroHandle handle, int32_t* status) {
  auto gyro = analogGyroHandles.Get(handle);
  if (gyro == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }
  return gyro->center;
}
}
