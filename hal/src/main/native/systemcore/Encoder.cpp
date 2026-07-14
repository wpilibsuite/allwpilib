// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/hal/Encoder.h"

#include <chrono>
#include <cstdint>
#include <cstdio>
#include <memory>
#include <thread>

#include "HALInitializer.hpp"
#include "PortsInternal.hpp"
#include "SmartIo.hpp"
#include "wpi/hal/ErrorHandling.hpp"
#include "wpi/hal/Errors.h"
#include "wpi/hal/handles/DigitalHandleResource.hpp"
#include "wpi/hal/monotonic_clock.hpp"

using namespace wpi::hal;

namespace {
struct Encoder {
  HAL_EncoderHandle handle{HAL_INVALID_HANDLE};
  HAL_DigitalHandle aPortHandle{HAL_INVALID_HANDLE};
  HAL_DigitalHandle bPortHandle{HAL_INVALID_HANDLE};
  std::shared_ptr<SmartIo> aPort;
  std::shared_ptr<SmartIo> bPort;

  HAL_EncoderEncodingType encodingType{HAL_ENCODER_4X_ENCODING};
  bool reverseDirection{false};
  double distancePerPulse{1.0};

  int32_t resetCount{0};
  int32_t lastRawCount{0};
  bool hasLastCount{false};
  bool direction{false};
};

DigitalHandleResource<HAL_EncoderHandle, Encoder, kNumSmartIo>* encoderHandles;

int EncodingScaleFactor(const Encoder& encoder) {
  switch (encoder.encodingType) {
    case HAL_ENCODER_1X_ENCODING:
      return 4;
    case HAL_ENCODER_2X_ENCODING:
      return 2;
    case HAL_ENCODER_4X_ENCODING:
      return 1;
    default:
      return 0;
  }
}

double DecodingScaleFactor(const Encoder& encoder) {
  switch (encoder.encodingType) {
    case HAL_ENCODER_1X_ENCODING:
      return 1.0 / 4.0;
    case HAL_ENCODER_2X_ENCODING:
      return 1.0 / 2.0;
    case HAL_ENCODER_4X_ENCODING:
      return 1.0;
    default:
      return 0.0;
  }
}

bool IsValidEncodingType(HAL_EncoderEncodingType encodingType) {
  return encodingType == HAL_ENCODER_1X_ENCODING ||
         encodingType == HAL_ENCODER_2X_ENCODING ||
         encodingType == HAL_ENCODER_4X_ENCODING;
}

bool IsValidChannelPair(int32_t aChannel, int32_t bChannel) {
  return aChannel >= 0 && aChannel < kNumSmartIo && aChannel % 2 == 0 &&
         bChannel == aChannel + 1;
}

std::shared_ptr<Encoder> GetEncoder(HAL_EncoderHandle handle, int32_t* status) {
  auto encoder = encoderHandles->Get(handle, HAL_HandleEnum::ENCODER);
  if (encoder == nullptr) {
    *status = HAL_HANDLE_ERROR;
  }
  return encoder;
}

int32_t ReadRawCount(Encoder& encoder, int32_t* status) {
  int32_t count = 0;
  *status = encoder.aPort->GetQuadrature(&count);
  if (*status != 0) {
    return 0;
  }

  if (encoder.hasLastCount && count != encoder.lastRawCount) {
    int64_t delta = static_cast<int64_t>(count) - encoder.lastRawCount;
    encoder.direction = delta > 0;
    if (encoder.reverseDirection) {
      encoder.direction = !encoder.direction;
    }
  }
  encoder.lastRawCount = count;
  encoder.hasLastCount = true;

  int64_t rawCount = static_cast<int64_t>(count) - encoder.resetCount;
  if (encoder.reverseDirection) {
    rawCount = -rawCount;
  }
  return static_cast<int32_t>(rawCount);
}

void ReleaseEncoderPorts(const std::shared_ptr<Encoder>& encoder) {
  auto aPortHandle = encoder->aPortHandle;
  auto bPortHandle = encoder->bPortHandle;

  encoderHandles->Free(encoder->handle, HAL_HandleEnum::ENCODER);

  // Wait for no other object to hold this encoder handle. The Encoder object
  // retains the ports until the last shared reference is released, so getters
  // already in progress can finish safely.
  auto start = wpi::hal::monotonic_clock::now();
  bool timeoutReported = false;
  while (encoder.use_count() != 1) {
    auto current = wpi::hal::monotonic_clock::now();
    if (!timeoutReported && start + std::chrono::seconds(1) < current) {
      std::puts("Encoder handle free taking longer than expected");
      std::fflush(stdout);
      timeoutReported = true;
    }
    std::this_thread::yield();
  }

  // Release the SmartIO objects before making their handles available for
  // reallocation. This ensures their destructors cannot close a newly
  // allocated resource on the same channels.
  encoder->aPort.reset();
  encoder->bPort.reset();
  smartIoHandles->Free(aPortHandle, HAL_HandleEnum::ENCODER);
  smartIoHandles->Free(bPortHandle, HAL_HandleEnum::ENCODER);
}
}  // namespace

namespace wpi::hal::init {
void InitializeEncoder() {
  static DigitalHandleResource<HAL_EncoderHandle, Encoder, kNumSmartIo> eH;
  encoderHandles = &eH;
}
}  // namespace wpi::hal::init

extern "C" {
HAL_EncoderHandle HAL_InitializeEncoder(int32_t aChannel, int32_t bChannel,
                                        HAL_Bool reverseDirection,
                                        HAL_EncoderEncodingType encodingType,
                                        int32_t* status) {
  wpi::hal::init::CheckInit();

  if (!IsValidChannelPair(aChannel, bChannel)) {
    *status = MakeError(
        HAL_PARAMETER_OUT_OF_RANGE,
        "Encoder channels must be paired as 0/1, 2/3, or 4/5 with the lower "
        "channel as A");
    return HAL_INVALID_HANDLE;
  }
  if (!IsValidEncodingType(encodingType)) {
    *status =
        MakeError(HAL_PARAMETER_OUT_OF_RANGE, "Invalid encoder encoding type");
    return HAL_INVALID_HANDLE;
  }

  auto aResource =
      smartIoHandles->Allocate(aChannel, HAL_HandleEnum::ENCODER, "Encoder");
  if (!aResource) {
    *status = aResource.error();
    return HAL_INVALID_HANDLE;
  }
  auto [aPortHandle, aPort] = *aResource;
  aPort->channel = aChannel;
  aPort->closeOnDestroy = false;

  auto bResource =
      smartIoHandles->Allocate(bChannel, HAL_HandleEnum::ENCODER, "Encoder");
  if (!bResource) {
    *status = bResource.error();
    smartIoHandles->Free(aPortHandle, HAL_HandleEnum::ENCODER);
    return HAL_INVALID_HANDLE;
  }
  auto [bPortHandle, bPort] = *bResource;
  bPort->channel = bChannel;
  bPort->closeOnDestroy = false;

  auto encoderResource =
      encoderHandles->Allocate(aChannel, HAL_HandleEnum::ENCODER, "Encoder");
  if (!encoderResource) {
    *status = encoderResource.error();
    smartIoHandles->Free(bPortHandle, HAL_HandleEnum::ENCODER);
    smartIoHandles->Free(aPortHandle, HAL_HandleEnum::ENCODER);
    return HAL_INVALID_HANDLE;
  }

  auto [handle, encoder] = *encoderResource;
  encoder->handle = handle;
  encoder->aPortHandle = aPortHandle;
  encoder->bPortHandle = bPortHandle;
  encoder->aPort = aPort;
  encoder->bPort = bPort;
  encoder->encodingType = encodingType;
  encoder->reverseDirection = reverseDirection;

  *status = aPort->InitializeMode(MRC_SmartIOMode::MRC_SmartIOMode_Quadrature);
  if (*status != 0) {
    encoder->aPort.reset();
    encoder->bPort.reset();
    encoderHandles->Free(handle, HAL_HandleEnum::ENCODER);
    smartIoHandles->Free(bPortHandle, HAL_HandleEnum::ENCODER);
    smartIoHandles->Free(aPortHandle, HAL_HandleEnum::ENCODER);
    return HAL_INVALID_HANDLE;
  }
  aPort->closeOnDestroy = true;

  int32_t count = 0;
  *status = aPort->GetQuadrature(&count);
  if (*status != 0) {
    encoder->aPort.reset();
    encoder->bPort.reset();
    encoderHandles->Free(handle, HAL_HandleEnum::ENCODER);
    smartIoHandles->Free(bPortHandle, HAL_HandleEnum::ENCODER);
    smartIoHandles->Free(aPortHandle, HAL_HandleEnum::ENCODER);
    return HAL_INVALID_HANDLE;
  }
  encoder->lastRawCount = count;
  encoder->hasLastCount = true;

  return handle;
}

void HAL_FreeEncoder(HAL_EncoderHandle encoderHandle) {
  auto encoder = encoderHandles->Get(encoderHandle, HAL_HandleEnum::ENCODER);
  if (encoder == nullptr) {
    return;
  }
  ReleaseEncoderPorts(encoder);
}

void HAL_SetEncoderSimDevice(HAL_EncoderHandle handle,
                             HAL_SimDeviceHandle device) {}

int32_t HAL_GetEncoder(HAL_EncoderHandle encoderHandle, int32_t* status) {
  auto encoder = GetEncoder(encoderHandle, status);
  if (encoder == nullptr) {
    return 0;
  }

  int32_t rawCount = ReadRawCount(*encoder, status);
  return static_cast<int32_t>(rawCount * DecodingScaleFactor(*encoder));
}

int32_t HAL_GetEncoderRaw(HAL_EncoderHandle encoderHandle, int32_t* status) {
  auto encoder = GetEncoder(encoderHandle, status);
  if (encoder == nullptr) {
    return 0;
  }

  return ReadRawCount(*encoder, status);
}

int32_t HAL_GetEncoderEncodingScale(HAL_EncoderHandle encoderHandle,
                                    int32_t* status) {
  auto encoder = GetEncoder(encoderHandle, status);
  if (encoder == nullptr) {
    return 0;
  }

  *status = 0;
  return EncodingScaleFactor(*encoder);
}

void HAL_ResetEncoder(HAL_EncoderHandle encoderHandle, int32_t* status) {
  auto encoder = GetEncoder(encoderHandle, status);
  if (encoder == nullptr) {
    return;
  }

  int32_t count = 0;
  *status = encoder->aPort->GetQuadrature(&count);
  if (*status != 0) {
    return;
  }
  encoder->resetCount = count;
  encoder->lastRawCount = count;
  encoder->hasLastCount = true;
}

HAL_Bool HAL_GetEncoderStopped(HAL_EncoderHandle encoderHandle,
                               int32_t* status) {
  auto encoder = GetEncoder(encoderHandle, status);
  if (encoder == nullptr) {
    return false;
  }

  int32_t rate = 0;
  *status = encoder->aPort->GetQuadratureRate(&rate);
  if (*status != 0) {
    return false;
  }
  return rate == 0;
}

HAL_Bool HAL_GetEncoderDirection(HAL_EncoderHandle encoderHandle,
                                 int32_t* status) {
  auto encoder = GetEncoder(encoderHandle, status);
  if (encoder == nullptr) {
    return false;
  }

  int32_t unused = ReadRawCount(*encoder, status);
  (void)unused;
  if (*status != 0) {
    return false;
  }
  return encoder->direction;
}

double HAL_GetEncoderDistance(HAL_EncoderHandle encoderHandle,
                              int32_t* status) {
  auto encoder = GetEncoder(encoderHandle, status);
  if (encoder == nullptr) {
    return 0;
  }

  int32_t rawCount = ReadRawCount(*encoder, status);
  if (*status != 0) {
    return 0;
  }
  return static_cast<double>(rawCount) * DecodingScaleFactor(*encoder) *
         encoder->distancePerPulse;
}

double HAL_GetEncoderRate(HAL_EncoderHandle encoderHandle, int32_t* status) {
  auto encoder = GetEncoder(encoderHandle, status);
  if (encoder == nullptr) {
    return 0;
  }

  int32_t rate = 0;
  *status = encoder->aPort->GetQuadratureRate(&rate);
  if (*status != 0) {
    return 0;
  }

  double scaledRate = static_cast<double>(rate) *
                      DecodingScaleFactor(*encoder) * encoder->distancePerPulse;
  return encoder->reverseDirection ? -scaledRate : scaledRate;
}

void HAL_SetEncoderDistancePerPulse(HAL_EncoderHandle encoderHandle,
                                    double distancePerPulse, int32_t* status) {
  auto encoder = GetEncoder(encoderHandle, status);
  if (encoder == nullptr) {
    return;
  }
  if (distancePerPulse == 0.0) {
    *status =
        MakeError(HAL_PARAMETER_OUT_OF_RANGE, "distancePerPulse must not be 0");
    return;
  }

  encoder->distancePerPulse = distancePerPulse;
  *status = 0;
}

void HAL_SetEncoderReverseDirection(HAL_EncoderHandle encoderHandle,
                                    HAL_Bool reverseDirection,
                                    int32_t* status) {
  auto encoder = GetEncoder(encoderHandle, status);
  if (encoder == nullptr) {
    return;
  }

  encoder->reverseDirection = reverseDirection;
  *status = 0;
}

double HAL_GetEncoderDecodingScaleFactor(HAL_EncoderHandle encoderHandle,
                                         int32_t* status) {
  auto encoder = GetEncoder(encoderHandle, status);
  if (encoder == nullptr) {
    return 0;
  }

  *status = 0;
  return DecodingScaleFactor(*encoder);
}

double HAL_GetEncoderDistancePerPulse(HAL_EncoderHandle encoderHandle,
                                      int32_t* status) {
  auto encoder = GetEncoder(encoderHandle, status);
  if (encoder == nullptr) {
    return 0;
  }

  *status = 0;
  return encoder->distancePerPulse;
}

HAL_EncoderEncodingType HAL_GetEncoderEncodingType(
    HAL_EncoderHandle encoderHandle, int32_t* status) {
  auto encoder = GetEncoder(encoderHandle, status);
  if (encoder == nullptr) {
    return HAL_ENCODER_4X_ENCODING;
  }

  *status = 0;
  return encoder->encodingType;
}

int32_t HAL_GetEncoderFPGAIndex(HAL_EncoderHandle encoderHandle,
                                int32_t* status) {
  auto encoder = GetEncoder(encoderHandle, status);
  if (encoder == nullptr) {
    return 0;
  }

  *status = 0;
  return getHandleIndex(encoder->aPortHandle) / 2;
}
}  // extern "C"
