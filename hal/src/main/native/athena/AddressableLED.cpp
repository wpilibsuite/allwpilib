#include "hal/AddressableLED.h"

#include "hal/handles/LimitedHandleResource.h"
#include "hal/handles/HandlesInternal.h"
#include "PortsInternal.h"
#include "DigitalInternal.h"
#include "hal/ChipObject.h"
#include "ConstantsInternal.h"
#include "cstring"
#include <iostream>
#include <thread>

using namespace hal;

extern "C" {
/**
 * Open a host memory buffer.
 *
 * @param session [in] handle to a currently open session
 * @param memoryName [in] name of the HMB memory configured in the project
 * @param virtualAddress [out] virtual address that the host will use to access the memory
 * @param size [out] size in bytes of the memory area
 * @return result of the call
 */
NiFpga_Status NiFpga_OpenHostMemoryBuffer(NiFpga_Session session,
                                          const char* memoryName,
                                          void** virtualAddress,
                                          size_t* size);

}

namespace {
struct AddressableLED {
  std::unique_ptr<tLED> led;
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
      hal::digitalChannelHandles->Get(outputPort, hal::HAL_HandleEnum::DIO);

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

  led->led->writeStringLength(1, status);

  if (*status != 0) {
    addressableLEDHandles->Free(handle);
    return HAL_kInvalidHandle;
  }

  led->led->writeOutputSelect(
      remapDigitalChannelToBitfieldChannel(digitalPort->channel), status);

  if (*status != 0) {
    addressableLEDHandles->Free(handle);
    return HAL_kInvalidHandle;
  }

  uint32_t session = led->led->getSystemInterface()->getHandle();
  std::cout << "Session Handle: " << session << " lv status " << *status << std::endl;

  uint32_t* data = nullptr;
  size_t size = 0;
  *status = NiFpga_OpenHostMemoryBuffer(session, "HMB_0_LED", reinterpret_cast<void**>(&data), &size);

  std::cout << "HMB Open Error: "  << *status << std::endl;

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
      hal::digitalChannelHandles->Get(outputPort, hal::HAL_HandleEnum::DIO);

  if (!digitalPort) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  auto led = addressableLEDHandles->Get(handle);
  if (!led) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  led->led->writeOutputSelect(
      remapDigitalChannelToBitfieldChannel(digitalPort->channel), status);
}

void HAL_WriteAddressableLEDStringLength(HAL_AddressableLEDHandle handle,
                                         int32_t value, int32_t* status) {
  auto led = addressableLEDHandles->Get(handle);
  if (!led) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  led->led->writeStringLength(value, status);
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

void HAL_WriteAddressableLEDOnce(HAL_AddressableLEDHandle handle,
                                 int32_t* status) {
  auto led = addressableLEDHandles->Get(handle);
  if (!led) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  led->led->strobeStart(status);
  led->led->strobeAbort(status);
}

void HAL_WriteAddressableLEDContinuously(HAL_AddressableLEDHandle handle,
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
