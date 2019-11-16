#include "hal/AddressableLED.h"

#include "hal/handles/LimitedHandleResource.h"
#include "hal/handles/HandlesInternal.h"
#include "PortsInternal.h"
#include "DigitalInternal.h"
#include "hal/ChipObject.h"
#include "FRC_FPGA_ChipObject/fpgainterfacecapi/NiFpga_HMB.h"
#include "ConstantsInternal.h"
#include "cstring"
#include <iostream>
#include <thread>

using namespace hal;

namespace {
struct AddressableLED {
  std::unique_ptr<tLED> led;
  void* ledBuffer;
  size_t ledBufferSize;
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
  auto digitalPort =
      hal::digitalChannelHandles->Get(outputPort, hal::HAL_HandleEnum::PWM);

  if (!digitalPort) {
    *status = HAL_HANDLE_ERROR;
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

  if (static_cast<uint32_t>(length) > led->ledBufferSize) {
    *status = PARAMETER_OUT_OF_RANGE;
    return;
  }

  led->led->strobeReset(status);

  while (led->led->readPixelWriteIndex(status) != 0)
    ;

  if (*status != 0) {
    return;
  }

  led->led->writeStringLength(length, status);

  if (*status != 0) {
    return;
  }

  std::memcpy(led->ledBuffer, data, length * sizeof(HAL_AddressableLEDData));

  asm("dmb");

  led->led->strobeLoad(status);

  while (led->led->readPixelWriteIndex(status) < length)
    ;
}

void HAL_SetAddressableLEDTiming(HAL_AddressableLEDHandle handle,
                                 int32_t highTime0, int32_t lowTime0,
                                 int32_t highTime1, int32_t lowTime1,
                                 int32_t resetTime, int32_t* status) {
  auto led = addressableLEDHandles->Get(handle);
  if (!led) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  led->led->writeLowBitTickTiming(1, lowTime0 / 25, status);
  led->led->writeLowBitTickTiming(0, highTime0 / 25, status);
  led->led->writeHighBitTickTiming(1, lowTime1 / 25, status);
  led->led->writeHighBitTickTiming(0, highTime1 / 25, status);

  led->led->writeSyncTiming(resetTime, status);
}

void HAL_StartAddressableLEDWrite(HAL_AddressableLEDHandle handle,
                                  int32_t* status) {
  auto led = addressableLEDHandles->Get(handle);
  if (!led) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  led->led->strobeStart(status);
}

void HAL_StopAddressableLEDWrite(HAL_AddressableLEDHandle handle,
                                 int32_t* status) {
  auto led = addressableLEDHandles->Get(handle);
  if (!led) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  led->led->strobeAbort(status);
}
}
