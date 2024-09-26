// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "hal/Encoder.h"

#include <limits>

#include "CounterInternal.h"
#include "HALInitializer.h"
#include "HALInternal.h"
#include "PortsInternal.h"
#include "hal/Errors.h"
#include "hal/handles/HandlesInternal.h"
#include "hal/handles/LimitedHandleResource.h"
#include "mockdata/EncoderDataInternal.h"

using namespace hal;

namespace {
struct Encoder {
  HAL_Handle nativeHandle;
  HAL_FPGAEncoderHandle fpgaHandle;
  HAL_CounterHandle counterHandle;
  HAL_EncoderEncodingType encodingType;
  double distancePerPulse;
  uint8_t index;
};
struct Empty {};
}  // namespace

static LimitedHandleResource<HAL_EncoderHandle, Encoder,
                             kNumEncoders + kNumCounters,
                             HAL_HandleEnum::Encoder>* encoderHandles;

static LimitedHandleResource<HAL_FPGAEncoderHandle, Empty, kNumEncoders,
                             HAL_HandleEnum::FPGAEncoder>* fpgaEncoderHandles;

namespace hal::init {
void InitializeEncoder() {
  static LimitedHandleResource<HAL_FPGAEncoderHandle, Empty, kNumEncoders,
                               HAL_HandleEnum::FPGAEncoder>
      feH;
  fpgaEncoderHandles = &feH;
  static LimitedHandleResource<HAL_EncoderHandle, Encoder,
                               kNumEncoders + kNumCounters,
                               HAL_HandleEnum::Encoder>
      eH;
  encoderHandles = &eH;
}
}  // namespace hal::init

namespace hal {
bool GetEncoderBaseHandle(HAL_EncoderHandle handle,
                          HAL_FPGAEncoderHandle* fpgaHandle,
                          HAL_CounterHandle* counterHandle) {
  auto encoder = encoderHandles->Get(handle);
  if (!encoder) {
    return false;
  }

  *fpgaHandle = encoder->fpgaHandle;
  *counterHandle = encoder->counterHandle;
  return true;
}
}  // namespace hal

extern "C" {
HAL_EncoderHandle HAL_InitializeEncoder(
    HAL_Handle digitalSourceHandleA, HAL_AnalogTriggerType analogTriggerTypeA,
    HAL_Handle digitalSourceHandleB, HAL_AnalogTriggerType analogTriggerTypeB,
    HAL_Bool reverseDirection, HAL_EncoderEncodingType encodingType,
    int32_t* status) {
  hal::init::CheckInit();
  HAL_Handle nativeHandle = HAL_kInvalidHandle;
  if (encodingType == HAL_EncoderEncodingType::HAL_Encoder_k4X) {
    // k4x, allocate encoder
    nativeHandle = fpgaEncoderHandles->Allocate();
  } else {
    // k2x or k1x, allocate counter
    nativeHandle = counterHandles->Allocate();
  }
  if (nativeHandle == HAL_kInvalidHandle) {
    *status = NO_AVAILABLE_RESOURCES;
    return HAL_kInvalidHandle;
  }
  auto handle = encoderHandles->Allocate();
  if (handle == HAL_kInvalidHandle) {
    *status = NO_AVAILABLE_RESOURCES;
    return HAL_kInvalidHandle;
  }
  auto encoder = encoderHandles->Get(handle);
  if (encoder == nullptr) {  // would only occur on thread issue
    *status = HAL_HANDLE_ERROR;
    return HAL_kInvalidHandle;
  }
  int16_t index = getHandleIndex(handle);
  SimEncoderData[index].digitalChannelA = getHandleIndex(digitalSourceHandleA);
  SimEncoderData[index].digitalChannelB = getHandleIndex(digitalSourceHandleB);
  SimEncoderData[index].initialized = true;
  SimEncoderData[index].reverseDirection = reverseDirection;
  SimEncoderData[index].simDevice = 0;
  // TODO: Add encoding type to Sim data
  encoder->index = index;
  encoder->nativeHandle = nativeHandle;
  encoder->encodingType = encodingType;
  encoder->distancePerPulse = 1.0;
  if (encodingType == HAL_EncoderEncodingType::HAL_Encoder_k4X) {
    encoder->fpgaHandle = nativeHandle;
    encoder->counterHandle = HAL_kInvalidHandle;
  } else {
    encoder->fpgaHandle = HAL_kInvalidHandle;
    encoder->counterHandle = nativeHandle;
  }
  return handle;
}

void HAL_FreeEncoder(HAL_EncoderHandle encoderHandle) {
  auto encoder = encoderHandles->Get(encoderHandle);
  encoderHandles->Free(encoderHandle);
  if (encoder == nullptr) {
    return;
  }
  if (isHandleType(encoder->nativeHandle, HAL_HandleEnum::FPGAEncoder)) {
    fpgaEncoderHandles->Free(encoder->nativeHandle);
  } else if (isHandleType(encoder->nativeHandle, HAL_HandleEnum::Counter)) {
    counterHandles->Free(encoder->nativeHandle);
  }
  SimEncoderData[encoder->index].initialized = false;
}

void HAL_SetEncoderSimDevice(HAL_EncoderHandle handle,
                             HAL_SimDeviceHandle device) {
  auto encoder = encoderHandles->Get(handle);
  if (encoder == nullptr) {
    return;
  }
  SimEncoderData[encoder->index].simDevice = device;
}

static inline int EncodingScaleFactor(Encoder* encoder) {
  switch (encoder->encodingType) {
    case HAL_Encoder_k1X:
      return 1;
    case HAL_Encoder_k2X:
      return 2;
    case HAL_Encoder_k4X:
      return 4;
    default:
      return 0;
  }
}

static inline double DecodingScaleFactor(Encoder* encoder) {
  switch (encoder->encodingType) {
    case HAL_Encoder_k1X:
      return 1.0;
    case HAL_Encoder_k2X:
      return 0.5;
    case HAL_Encoder_k4X:
      return 0.25;
    default:
      return 0.0;
  }
}

int32_t HAL_GetEncoder(HAL_EncoderHandle encoderHandle, int32_t* status) {
  auto encoder = encoderHandles->Get(encoderHandle);
  if (encoder == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }

  return SimEncoderData[encoder->index].count;
}
int32_t HAL_GetEncoderRaw(HAL_EncoderHandle encoderHandle, int32_t* status) {
  auto encoder = encoderHandles->Get(encoderHandle);
  if (encoder == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }

  return SimEncoderData[encoder->index].count /
         DecodingScaleFactor(encoder.get());
}
int32_t HAL_GetEncoderEncodingScale(HAL_EncoderHandle encoderHandle,
                                    int32_t* status) {
  auto encoder = encoderHandles->Get(encoderHandle);
  if (encoder == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }

  return EncodingScaleFactor(encoder.get());
}
void HAL_ResetEncoder(HAL_EncoderHandle encoderHandle, int32_t* status) {
  auto encoder = encoderHandles->Get(encoderHandle);
  if (encoder == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  SimEncoderData[encoder->index].reset = true;
  SimEncoderData[encoder->index].count = 0;
  SimEncoderData[encoder->index].period = std::numeric_limits<double>::max();
}
double HAL_GetEncoderPeriod(HAL_EncoderHandle encoderHandle, int32_t* status) {
  auto encoder = encoderHandles->Get(encoderHandle);
  if (encoder == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }

  return SimEncoderData[encoder->index].period;
}
void HAL_SetEncoderMaxPeriod(HAL_EncoderHandle encoderHandle, double maxPeriod,
                             int32_t* status) {
  auto encoder = encoderHandles->Get(encoderHandle);
  if (encoder == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  SimEncoderData[encoder->index].maxPeriod = maxPeriod;
}
HAL_Bool HAL_GetEncoderStopped(HAL_EncoderHandle encoderHandle,
                               int32_t* status) {
  auto encoder = encoderHandles->Get(encoderHandle);
  if (encoder == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }

  return SimEncoderData[encoder->index].period >
         SimEncoderData[encoder->index].maxPeriod;
}
HAL_Bool HAL_GetEncoderDirection(HAL_EncoderHandle encoderHandle,
                                 int32_t* status) {
  auto encoder = encoderHandles->Get(encoderHandle);
  if (encoder == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }

  return SimEncoderData[encoder->index].direction;
}
double HAL_GetEncoderDistance(HAL_EncoderHandle encoderHandle,
                              int32_t* status) {
  auto encoder = encoderHandles->Get(encoderHandle);
  if (encoder == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }

  return SimEncoderData[encoder->index].count * encoder->distancePerPulse;
}
double HAL_GetEncoderRate(HAL_EncoderHandle encoderHandle, int32_t* status) {
  auto encoder = encoderHandles->Get(encoderHandle);
  if (encoder == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }

  return encoder->distancePerPulse / SimEncoderData[encoder->index].period;
}
void HAL_SetEncoderMinRate(HAL_EncoderHandle encoderHandle, double minRate,
                           int32_t* status) {
  auto encoder = encoderHandles->Get(encoderHandle);
  if (encoder == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  if (minRate == 0.0) {
    *status = PARAMETER_OUT_OF_RANGE;
    hal::SetLastError(status, "minRate must not be 0");
    return;
  }

  SimEncoderData[encoder->index].maxPeriod =
      encoder->distancePerPulse / minRate;
}
void HAL_SetEncoderDistancePerPulse(HAL_EncoderHandle encoderHandle,
                                    double distancePerPulse, int32_t* status) {
  auto encoder = encoderHandles->Get(encoderHandle);
  if (encoder == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  if (distancePerPulse == 0.0) {
    *status = PARAMETER_OUT_OF_RANGE;
    hal::SetLastError(status, "distancePerPulse must not be 0");
    return;
  }
  encoder->distancePerPulse = distancePerPulse;
  SimEncoderData[encoder->index].distancePerPulse = distancePerPulse;
}
void HAL_SetEncoderReverseDirection(HAL_EncoderHandle encoderHandle,
                                    HAL_Bool reverseDirection,
                                    int32_t* status) {
  auto encoder = encoderHandles->Get(encoderHandle);
  if (encoder == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  SimEncoderData[encoder->index].reverseDirection = reverseDirection;
}
void HAL_SetEncoderSamplesToAverage(HAL_EncoderHandle encoderHandle,
                                    int32_t samplesToAverage, int32_t* status) {
  auto encoder = encoderHandles->Get(encoderHandle);
  if (encoder == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  SimEncoderData[encoder->index].samplesToAverage = samplesToAverage;
}
int32_t HAL_GetEncoderSamplesToAverage(HAL_EncoderHandle encoderHandle,
                                       int32_t* status) {
  auto encoder = encoderHandles->Get(encoderHandle);
  if (encoder == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }

  return SimEncoderData[encoder->index].samplesToAverage;
}

void HAL_SetEncoderIndexSource(HAL_EncoderHandle encoderHandle,
                               HAL_Handle digitalSourceHandle,
                               HAL_AnalogTriggerType analogTriggerType,
                               HAL_EncoderIndexingType type, int32_t* status) {
  // Not implemented yet
}

int32_t HAL_GetEncoderFPGAIndex(HAL_EncoderHandle encoderHandle,
                                int32_t* status) {
  auto encoder = encoderHandles->Get(encoderHandle);
  if (encoder == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }

  return encoder->index;
}

double HAL_GetEncoderDecodingScaleFactor(HAL_EncoderHandle encoderHandle,
                                         int32_t* status) {
  auto encoder = encoderHandles->Get(encoderHandle);
  if (encoder == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return 0.0;
  }

  return DecodingScaleFactor(encoder.get());
}

double HAL_GetEncoderDistancePerPulse(HAL_EncoderHandle encoderHandle,
                                      int32_t* status) {
  auto encoder = encoderHandles->Get(encoderHandle);
  if (encoder == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return 0.0;
  }

  return SimEncoderData[encoder->index].distancePerPulse;
}

HAL_EncoderEncodingType HAL_GetEncoderEncodingType(
    HAL_EncoderHandle encoderHandle, int32_t* status) {
  auto encoder = encoderHandles->Get(encoderHandle);
  if (encoder == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return HAL_Encoder_k4X;  // default to k4x
  }

  return encoder->encodingType;
}
}  // extern "C"
