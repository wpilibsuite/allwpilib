/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "hal/AnalogGyro.h"

#include <thread>

#include <wpi/raw_ostream.h>

#include "AnalogInternal.h"
#include "HALInitializer.h"
#include "hal/AnalogAccumulator.h"
#include "hal/AnalogInput.h"
#include "hal/handles/IndexedHandleResource.h"

namespace {

struct AnalogGyro {
  HAL_AnalogInputHandle handle;
  double voltsPerDegreePerSecond;
  double offset;
  int32_t center;
};

}  // namespace

static constexpr uint32_t kOversampleBits = 10;
static constexpr uint32_t kAverageBits = 0;
static constexpr double kSamplesPerSecond = 50.0;
static constexpr double kCalibrationSampleTime = 5.0;
static constexpr double kDefaultVoltsPerDegreePerSecond = 0.007;

using namespace hal;

static IndexedHandleResource<HAL_GyroHandle, AnalogGyro, kNumAccumulators,
                             HAL_HandleEnum::AnalogGyro>* analogGyroHandles;

namespace hal {
namespace init {
void InitializeAnalogGyro() {
  static IndexedHandleResource<HAL_GyroHandle, AnalogGyro, kNumAccumulators,
                               HAL_HandleEnum::AnalogGyro>
      agHandles;
  analogGyroHandles = &agHandles;
}
}  // namespace init
}  // namespace hal

static void Wait(double seconds) {
  if (seconds < 0.0) return;
  std::this_thread::sleep_for(std::chrono::duration<double>(seconds));
}

extern "C" {

HAL_GyroHandle HAL_InitializeAnalogGyro(HAL_AnalogInputHandle analogHandle,
                                        int32_t* status) {
  hal::init::CheckInit();
  if (!HAL_IsAccumulatorChannel(analogHandle, status)) {
    if (*status == 0) {
      *status = HAL_INVALID_ACCUMULATOR_CHANNEL;
    }
    return HAL_kInvalidHandle;
  }

  // handle known to be correct, so no need to type check
  int16_t channel = getHandleIndex(analogHandle);

  auto handle = analogGyroHandles->Allocate(channel, status);

  if (*status != 0)
    return HAL_kInvalidHandle;  // failed to allocate. Pass error back.

  // Initialize port structure
  auto gyro = analogGyroHandles->Get(handle);
  if (gyro == nullptr) {  // would only error on thread issue
    *status = HAL_HANDLE_ERROR;
    return HAL_kInvalidHandle;
  }

  gyro->handle = analogHandle;
  gyro->voltsPerDegreePerSecond = 0;
  gyro->offset = 0;
  gyro->center = 0;

  return handle;
}

void HAL_SetupAnalogGyro(HAL_GyroHandle handle, int32_t* status) {
  auto gyro = analogGyroHandles->Get(handle);
  if (gyro == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  gyro->voltsPerDegreePerSecond = kDefaultVoltsPerDegreePerSecond;

  HAL_SetAnalogAverageBits(gyro->handle, kAverageBits, status);
  if (*status != 0) return;
  HAL_SetAnalogOversampleBits(gyro->handle, kOversampleBits, status);
  if (*status != 0) return;
  double sampleRate =
      kSamplesPerSecond * (1 << (kAverageBits + kOversampleBits));
  HAL_SetAnalogSampleRate(sampleRate, status);
  if (*status != 0) return;
  Wait(0.1);

  HAL_SetAnalogGyroDeadband(handle, 0.0, status);
  if (*status != 0) return;
}

void HAL_FreeAnalogGyro(HAL_GyroHandle handle) {
  analogGyroHandles->Free(handle);
}

void HAL_SetAnalogGyroParameters(HAL_GyroHandle handle,
                                 double voltsPerDegreePerSecond, double offset,
                                 int32_t center, int32_t* status) {
  auto gyro = analogGyroHandles->Get(handle);
  if (gyro == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  gyro->voltsPerDegreePerSecond = voltsPerDegreePerSecond;
  gyro->offset = offset;
  gyro->center = center;
  HAL_SetAccumulatorCenter(gyro->handle, center, status);
}

void HAL_SetAnalogGyroVoltsPerDegreePerSecond(HAL_GyroHandle handle,
                                              double voltsPerDegreePerSecond,
                                              int32_t* status) {
  auto gyro = analogGyroHandles->Get(handle);
  if (gyro == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  gyro->voltsPerDegreePerSecond = voltsPerDegreePerSecond;
}

void HAL_ResetAnalogGyro(HAL_GyroHandle handle, int32_t* status) {
  auto gyro = analogGyroHandles->Get(handle);
  if (gyro == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }
  HAL_ResetAccumulator(gyro->handle, status);
  if (*status != 0) return;

  const double sampleTime = 1.0 / HAL_GetAnalogSampleRate(status);
  const double overSamples =
      1 << HAL_GetAnalogOversampleBits(gyro->handle, status);
  const double averageSamples =
      1 << HAL_GetAnalogAverageBits(gyro->handle, status);
  if (*status != 0) return;
  Wait(sampleTime * overSamples * averageSamples);
}

void HAL_CalibrateAnalogGyro(HAL_GyroHandle handle, int32_t* status) {
  auto gyro = analogGyroHandles->Get(handle);
  if (gyro == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  HAL_InitAccumulator(gyro->handle, status);
  if (*status != 0) return;
  wpi::outs() << "Calibrating analog gyro for " << kCalibrationSampleTime
              << " seconds." << '\n';
  Wait(kCalibrationSampleTime);

  int64_t value;
  int64_t count;
  HAL_GetAccumulatorOutput(gyro->handle, &value, &count, status);
  if (*status != 0) return;

  gyro->center = static_cast<int32_t>(
      static_cast<double>(value) / static_cast<double>(count) + .5);

  gyro->offset = static_cast<double>(value) / static_cast<double>(count) -
                 static_cast<double>(gyro->center);
  HAL_SetAccumulatorCenter(gyro->handle, gyro->center, status);
  if (*status != 0) return;
  HAL_ResetAnalogGyro(handle, status);
}

void HAL_SetAnalogGyroDeadband(HAL_GyroHandle handle, double volts,
                               int32_t* status) {
  auto gyro = analogGyroHandles->Get(handle);
  if (gyro == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }
  int32_t deadband = static_cast<int32_t>(
      volts * 1e9 / HAL_GetAnalogLSBWeight(gyro->handle, status) *
      (1 << HAL_GetAnalogOversampleBits(gyro->handle, status)));
  if (*status != 0) return;
  HAL_SetAccumulatorDeadband(gyro->handle, deadband, status);
}

double HAL_GetAnalogGyroAngle(HAL_GyroHandle handle, int32_t* status) {
  auto gyro = analogGyroHandles->Get(handle);
  if (gyro == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }
  int64_t rawValue = 0;
  int64_t count = 0;
  HAL_GetAccumulatorOutput(gyro->handle, &rawValue, &count, status);

  int64_t value = rawValue - static_cast<int64_t>(static_cast<double>(count) *
                                                  gyro->offset);

  double scaledValue =
      value * 1e-9 *
      static_cast<double>(HAL_GetAnalogLSBWeight(gyro->handle, status)) *
      static_cast<double>(1 << HAL_GetAnalogAverageBits(gyro->handle, status)) /
      (HAL_GetAnalogSampleRate(status) * gyro->voltsPerDegreePerSecond);

  return scaledValue;
}

double HAL_GetAnalogGyroRate(HAL_GyroHandle handle, int32_t* status) {
  auto gyro = analogGyroHandles->Get(handle);
  if (gyro == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }

  return (HAL_GetAnalogAverageValue(gyro->handle, status) -
          (static_cast<double>(gyro->center) + gyro->offset)) *
         1e-9 * HAL_GetAnalogLSBWeight(gyro->handle, status) /
         ((1 << HAL_GetAnalogOversampleBits(gyro->handle, status)) *
          gyro->voltsPerDegreePerSecond);
}

double HAL_GetAnalogGyroOffset(HAL_GyroHandle handle, int32_t* status) {
  auto gyro = analogGyroHandles->Get(handle);
  if (gyro == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }
  return gyro->offset;
}

int32_t HAL_GetAnalogGyroCenter(HAL_GyroHandle handle, int32_t* status) {
  auto gyro = analogGyroHandles->Get(handle);
  if (gyro == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }
  return gyro->center;
}

}  // extern "C"
