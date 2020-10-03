/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "hal/AddressableLED.h"

#include <cstring>

#include <FRC_FPGA_ChipObject/fpgainterfacecapi/NiFpga_HMB.h>

#include "ConstantsInternal.h"
#include "DigitalInternal.h"
#include "HALInitializer.h"
#include "PortsInternal.h"
#include "hal/AddressableLEDTypes.h"
#include "hal/ChipObject.h"
#include "hal/handles/HandlesInternal.h"
#include "hal/handles/LimitedHandleResource.h"

using namespace hal;

namespace {
struct AddressableLED {
  std::unique_ptr<tLED> led;
  void* ledBuffer;
  size_t ledBufferSize;
  int32_t stringLength = 1;
};
}  // namespace

static LimitedHandleResource<
    HAL_AddressableLEDHandle, AddressableLED, kNumAddressableLEDs,
    HAL_HandleEnum::AddressableLED>* addressableLEDHandles;

namespace hal {
namespace init {
void InitializeAddressableLED() {
  static LimitedHandleResource<HAL_AddressableLEDHandle, AddressableLED,
                               kNumAddressableLEDs,
                               HAL_HandleEnum::AddressableLED>
      alH;
  addressableLEDHandles = &alH;
}
}  // namespace init
}  // namespace hal

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

  *status = NiFpga_OpenHostMemoryBuffer(session, "HMB_0_LED", &led->ledBuffer,
                                        &led->ledBufferSize);

  if (*status != 0) {
    addressableLEDHandles->Free(handle);
    return HAL_kInvalidHandle;
  }

  return handle;
}

void HAL_FreeAddressableLED(HAL_AddressableLEDHandle handle) {
  addressableLEDHandles->Free(handle);
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

  if (length > HAL_kAddressableLEDMaxLength) {
    *status = PARAMETER_OUT_OF_RANGE;
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

  if (length > led->stringLength) {
    *status = PARAMETER_OUT_OF_RANGE;
    return;
  }

  std::memcpy(led->ledBuffer, data, length * sizeof(HAL_AddressableLEDData));

  asm("dmb");

  led->led->strobeLoad(status);
}

void HAL_SetAddressableLEDBitTiming(HAL_AddressableLEDHandle handle,
                                    int32_t lowTime0NanoSeconds,
                                    int32_t highTime0NanoSeconds,
                                    int32_t lowTime1NanoSeconds,
                                    int32_t highTime1NanoSeconds,
                                    int32_t* status) {
  auto led = addressableLEDHandles->Get(handle);
  if (!led) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  led->led->writeLowBitTickTiming(1, highTime0NanoSeconds / 25, status);
  led->led->writeLowBitTickTiming(0, lowTime0NanoSeconds / 25, status);
  led->led->writeHighBitTickTiming(1, highTime1NanoSeconds / 25, status);
  led->led->writeHighBitTickTiming(0, lowTime1NanoSeconds / 25, status);
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
