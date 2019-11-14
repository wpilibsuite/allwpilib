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

namespace {
struct AddressableLED {
  std::unique_ptr<tLED> led;
};
}

static LimitedHandleResource<HAL_AddressableLEDHandle, AddressableLED, kNumAddressableLEDs,
                      HAL_HandleEnum::AddressableLED>* addressableLEDHandles;

namespace hal {
namespace init {
void InitializeAddressableLED() {
  static LimitedHandleResource<HAL_AddressableLEDHandle, AddressableLED, kNumAddressableLEDs,
                               HAL_HandleEnum::AddressableLED>
      alH;
  addressableLEDHandles = &alH;
}
}  // namespace init
}  // namespace hal

extern "C" {

HAL_AddressableLEDHandle HAL_InitializeAddressableLED(HAL_DigitalHandle outputPort, int32_t* status) {
  auto digitalPort = hal::digitalChannelHandles->Get(outputPort, hal::HAL_HandleEnum::DIO);

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

  led->led->writeOutputSelect(remapDigitalChannelToBitfieldChannel(digitalPort->channel),status);

  led->led->writeStringLength(42, status);

  std::cout << "OutputIndex:" << led->led->readPixelOutputIndex(status) << std::endl;
  std::cout << "WriteIndex:" << led->led->readPixelWriteIndex(status) << std::endl;



  if (*status != 0) {
    addressableLEDHandles->Free(handle);
    return HAL_kInvalidHandle;
  }

  return handle;
}

void HAL_FreeAddressableLED(HAL_AddressableLEDHandle handle) {
  addressableLEDHandles->Free(handle);
}

void HAL_SetAddressableLEDTiming(HAL_AddressableLEDHandle handle, int32_t highTime0, int32_t lowTime0, int32_t highTime1, int32_t lowTime1, int32_t reset, int32_t* status) {
  auto led = addressableLEDHandles->Get(handle);
  if (!led) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  std::cout << "Low0: " << (int)led->led->readLowBitTickTiming(0, status) << std::endl;
  std::cout << "Low1: " << (int)led->led->readLowBitTickTiming(1, status) << std::endl;
  std::cout << "High0: " << (int)led->led->readHighBitTickTiming(0, status) << std::endl;
  std::cout << "High1: " << (int)led->led->readHighBitTickTiming(1, status) << std::endl;

  led->led->writeHighBitTickTiming(0, highTime0 / kSystemClockTicksPerMicrosecond, status);
  led->led->writeHighBitTickTiming(1, highTime1 / kSystemClockTicksPerMicrosecond, status);
  led->led->writeLowBitTickTiming(0, lowTime0 / kSystemClockTicksPerMicrosecond, status);
  led->led->writeLowBitTickTiming(1, lowTime1 / kSystemClockTicksPerMicrosecond, status);
  std::cout << "SyncTiming: " << led->led->readSyncTiming(status) << std::endl;
  //led->led->writeSyncTiming(reset / kSystemClockTicksPerMicrosecond, status);

    std::cout << "OutputIndex:" << led->led->readPixelOutputIndex(status) << std::endl;
  std::cout << "WriteIndex:" << led->led->readPixelWriteIndex(status) << std::endl;

  led->led->strobeLoad(status);
  std::cout << "Strobe Load: "  << *status << std::endl;

  led->led->strobeAbort(status);
  std::cout << "Strobe Abort: " << *status << std::endl;

  led->led->strobeResetLoad(status);
  std::cout << "Strobe Reset Load: " << *status << std::endl;

  std::cout << "SyncTiming: " << led->led->readSyncTiming(status) << std::endl;

  std::this_thread::sleep_for(std::chrono::milliseconds(1));

        std::cout << "OutputIndex:" << led->led->readPixelOutputIndex(status) << std::endl;
  std::cout << "WriteIndex:" << led->led->readPixelWriteIndex(status) << std::endl;

  //     led->led->strobeAbort(status);
  // std::cout << "Strobe Abort: " << *status << std::endl;

         led->led->strobeStart(status);
   std::cout << "Strobe Start: " << *status << std::endl;


         led->led->strobeStart(status);
   std::cout << "Strobe Start: " << *status << std::endl;


         led->led->strobeStart(status);
   std::cout << "Strobe Start: " << *status << std::endl;


         led->led->strobeStart(status);
   std::cout << "Strobe Start: " << *status << std::endl;


    std::cout << "OutputIndex:" << led->led->readPixelOutputIndex(status) << std::endl;
  std::cout << "WriteIndex:" << led->led->readPixelWriteIndex(status) << std::endl;

  std::this_thread::sleep_for(std::chrono::milliseconds(1));

      std::cout << "OutputIndex:" << led->led->readPixelOutputIndex(status) << std::endl;
  std::cout << "WriteIndex:" << led->led->readPixelWriteIndex(status) << std::endl;
}



}
