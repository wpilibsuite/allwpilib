// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "hal/AddressableLED.h"

#include <cstring>
#include <memory>

#include <fmt/format.h>

#include "AddressableLEDSimd.h"
#include "ConstantsInternal.h"
#include "DigitalInternal.h"
#include "FPGACalls.h"
#include "HALInitializer.h"
#include "HALInternal.h"
#include "PortsInternal.h"
#include "hal/AddressableLEDTypes.h"
#include "hal/ChipObject.h"
#include "hal/handles/HandlesInternal.h"
#include "hal/handles/LimitedHandleResource.h"

using namespace hal;
using namespace hal::detail;

namespace {
struct AddressableLED {
  std::unique_ptr<tLED> led;
  void* ledBuffer;
  size_t ledBufferSize;
  int32_t stringLength = 1;
  HAL_AddressableLEDColorOrder colorOrder = HAL_ALED_GRB;
};
}  // namespace

static LimitedHandleResource<
    HAL_AddressableLEDHandle, AddressableLED, kNumAddressableLEDs,
    HAL_HandleEnum::AddressableLED>* addressableLEDHandles;

namespace hal::init {
void InitializeAddressableLED() {
  static LimitedHandleResource<HAL_AddressableLEDHandle, AddressableLED,
                               kNumAddressableLEDs,
                               HAL_HandleEnum::AddressableLED>
      alH;
  addressableLEDHandles = &alH;
}
}  // namespace hal::init

static constexpr const char* HmbName = "HMB_0_LED";

static void ConvertAndCopyLEDData(void* dst,
                                  const struct HAL_AddressableLEDData* src,
                                  int32_t len,
                                  HAL_AddressableLEDColorOrder order) {
  switch (order) {
    case HAL_ALED_GRB:
      std::memcpy(dst, src, len * sizeof(HAL_AddressableLEDData));
      break;
    case HAL_ALED_RGB:
      ConvertPixels<HAL_ALED_RGB>(reinterpret_cast<const uint8_t*>(src),
                                  reinterpret_cast<uint8_t*>(dst), len);
      break;
    case HAL_ALED_RBG:
      ConvertPixels<HAL_ALED_RBG>(reinterpret_cast<const uint8_t*>(src),
                                  reinterpret_cast<uint8_t*>(dst), len);
      break;
    case HAL_ALED_BGR:
      ConvertPixels<HAL_ALED_BGR>(reinterpret_cast<const uint8_t*>(src),
                                  reinterpret_cast<uint8_t*>(dst), len);
      break;
    case HAL_ALED_BRG:
      ConvertPixels<HAL_ALED_BRG>(reinterpret_cast<const uint8_t*>(src),
                                  reinterpret_cast<uint8_t*>(dst), len);
      break;
    case HAL_ALED_GBR:
      ConvertPixels<HAL_ALED_GBR>(reinterpret_cast<const uint8_t*>(src),
                                  reinterpret_cast<uint8_t*>(dst), len);
      break;
  }
}

extern "C" {

HAL_AddressableLEDHandle HAL_InitializeAddressableLED(
    HAL_DigitalHandle outputPort, int32_t* status) {
  hal::init::CheckInit();

  auto digitalPort =
      hal::digitalChannelHandles->Get(outputPort, hal::HAL_HandleEnum::PWM);

  if (!digitalPort) {
    // If DIO was passed, channel error, else generic error
    if (getHandleType(outputPort) == hal::HAL_HandleEnum::DIO) {
      *status = HAL_LED_CHANNEL_ERROR;
    } else {
      *status = HAL_HANDLE_ERROR;
    }
    return HAL_kInvalidHandle;
  }

  if (digitalPort->channel >= kNumPWMHeaders) {
    *status = HAL_LED_CHANNEL_ERROR;
    return HAL_kInvalidHandle;
  }

  auto handle = addressableLEDHandles->Allocate();

  if (handle == HAL_kInvalidHandle) {
    *status = NO_AVAILABLE_RESOURCES;
    return HAL_kInvalidHandle;
  }

  auto led = addressableLEDHandles->Get(handle);

  if (!led) {
    *status = HAL_HANDLE_ERROR;
    return HAL_kInvalidHandle;
  }

  led->led.reset(tLED::create(status));

  if (*status != 0) {
    addressableLEDHandles->Free(handle);
    return HAL_kInvalidHandle;
  }

  led->led->writeOutputSelect(digitalPort->channel, status);

  if (*status != 0) {
    addressableLEDHandles->Free(handle);
    return HAL_kInvalidHandle;
  }

  led->ledBuffer = nullptr;
  led->ledBufferSize = 0;

  uint32_t session = led->led->getSystemInterface()->getHandle();

  *status = hal::HAL_NiFpga_OpenHmb(session, HmbName, &led->ledBufferSize,
                                    &led->ledBuffer);

  if (*status != 0) {
    addressableLEDHandles->Free(handle);
    return HAL_kInvalidHandle;
  }

  return handle;
}

void HAL_FreeAddressableLED(HAL_AddressableLEDHandle handle) {
  auto led = addressableLEDHandles->Get(handle);
  if (!led) {
    return;
  }
  uint32_t session = led->led->getSystemInterface()->getHandle();
  hal::HAL_NiFpga_CloseHmb(session, HmbName);
  addressableLEDHandles->Free(handle);
}

void HAL_SetAddressableLEDColorOrder(HAL_AddressableLEDHandle handle,
                                     HAL_AddressableLEDColorOrder colorOrder,
                                     int32_t* status) {
  auto led = addressableLEDHandles->Get(handle);

  if (!led) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  led->colorOrder = colorOrder;
}

void HAL_SetAddressableLEDOutputPort(HAL_AddressableLEDHandle handle,
                                     HAL_DigitalHandle outputPort,
                                     int32_t* status) {
  auto digitalPort =
      hal::digitalChannelHandles->Get(outputPort, hal::HAL_HandleEnum::PWM);

  if (!digitalPort) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  auto led = addressableLEDHandles->Get(handle);
  if (!led) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  led->led->writeOutputSelect(digitalPort->channel, status);
}

void HAL_SetAddressableLEDLength(HAL_AddressableLEDHandle handle,
                                 int32_t length, int32_t* status) {
  auto led = addressableLEDHandles->Get(handle);
  if (!led) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  if (length > HAL_kAddressableLEDMaxLength || length < 0) {
    *status = PARAMETER_OUT_OF_RANGE;
    hal::SetLastError(
        status,
        fmt::format(
            "LED length must be less than or equal to {}. {} was requested",
            HAL_kAddressableLEDMaxLength, length));
    return;
  }

  led->led->strobeReset(status);

  while (led->led->readPixelWriteIndex(status) != 0) {
  }

  if (*status != 0) {
    return;
  }

  led->led->writeStringLength(length, status);

  led->stringLength = length;
}

static_assert(sizeof(HAL_AddressableLEDData) == sizeof(uint32_t),
              "LED Data must be 32 bit");

void HAL_WriteAddressableLEDData(HAL_AddressableLEDHandle handle,
                                 const struct HAL_AddressableLEDData* data,
                                 int32_t length, int32_t* status) {
  auto led = addressableLEDHandles->Get(handle);
  if (!led) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  if (length > led->stringLength || length < 0) {
    *status = PARAMETER_OUT_OF_RANGE;
    hal::SetLastError(
        status,
        fmt::format(
            "Data length must be less than or equal to {}. {} was requested",
            led->stringLength, length));
    return;
  }

  if (length == 0) {
    return;
  }

  ConvertAndCopyLEDData(led->ledBuffer, data, length, led->colorOrder);

  asm("dmb");

  led->led->strobeLoad(status);
}

void HAL_SetAddressableLEDBitTiming(HAL_AddressableLEDHandle handle,
                                    int32_t highTime0NanoSeconds,
                                    int32_t lowTime0NanoSeconds,
                                    int32_t highTime1NanoSeconds,
                                    int32_t lowTime1NanoSeconds,
                                    int32_t* status) {
  auto led = addressableLEDHandles->Get(handle);
  if (!led) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  led->led->writeLowBitTickTiming(0, highTime0NanoSeconds / 25, status);
  led->led->writeLowBitTickTiming(1, lowTime0NanoSeconds / 25, status);
  led->led->writeHighBitTickTiming(0, highTime1NanoSeconds / 25, status);
  led->led->writeHighBitTickTiming(1, lowTime1NanoSeconds / 25, status);
}

void HAL_SetAddressableLEDSyncTime(HAL_AddressableLEDHandle handle,
                                   int32_t syncTimeMicroSeconds,
                                   int32_t* status) {
  auto led = addressableLEDHandles->Get(handle);
  if (!led) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  led->led->writeSyncTiming(syncTimeMicroSeconds, status);
}

void HAL_StartAddressableLEDOutput(HAL_AddressableLEDHandle handle,
                                   int32_t* status) {
  auto led = addressableLEDHandles->Get(handle);
  if (!led) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  led->led->strobeStart(status);
}

void HAL_StopAddressableLEDOutput(HAL_AddressableLEDHandle handle,
                                  int32_t* status) {
  auto led = addressableLEDHandles->Get(handle);
  if (!led) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  led->led->strobeAbort(status);
}
}  // extern "C"
