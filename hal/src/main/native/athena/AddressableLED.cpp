#include "hal/AddressableLED.h"

#include "hal/handles/LimitedHandleResource.h"
#include "hal/handles/HandlesInternal.h"
#include "PortsInternal.h"

using namespace hal;

namespace {
struct AddressableLED {

};
}

static LimitedHandleResource<HAL_DutyCycleHandle, AddressableLED, kNumDutyCycles,
                      HAL_HandleEnum::DutyCycle>* dutyCycleHandles;

namespace hal {
namespace init {
void InitializeDutyCycle() {
  static LimitedHandleResource<HAL_DutyCycleHandle, DutyCycle, kNumDutyCycles,
                               HAL_HandleEnum::DutyCycle>
      dcH;
  dutyCycleHandles = &dcH;
}
}  // namespace init
}  // namespace hal

extern "C" {

HAL_AddressableLEDHandle HAL_InitializeAddressableLED(HAL_DigitalHandle outputPort, int32_t* status) {

}
void HAL_FreeAddressableLED(HAL_AddressableLEDHandle handle) {

}


}
