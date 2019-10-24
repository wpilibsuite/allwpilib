#include "hal/DutyCycle.h"

#include <memory>

#include "hal/handles/HandlesInternal.h"
#include "hal/handles/LimitedHandleResource.h"
#include "hal/Errors.h"
#include "hal/ChipObject.h"
#include "PortsInternal.h"
#include "HALInitializer.h"
#include "DigitalInternal.h"

using namespace hal;

namespace {
struct DutyCycle {
  std::unique_ptr<tDutyCycle> dutyCycle;
};
}  // namespace

static LimitedHandleResource<HAL_DutyCycleHandle, DutyCycle, kNumDutyCycles,
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
HAL_DutyCycleHandle HAL_InitializeDutyCycle(HAL_Handle digitalSourceHandle,
                                            HAL_AnalogTriggerType triggerType,
                                            int32_t* status) {
  hal::init::CheckInit();

  bool routingAnalogTrigger = false;
  uint8_t routingChannel = 0;
  uint8_t routingModule = 0;
  bool success =
      remapDigitalSource(digitalSourceHandle, triggerType, routingChannel,
                         routingModule, routingAnalogTrigger);

  if (!success) {
    *status = HAL_HANDLE_ERROR;
    return HAL_kInvalidHandle;
  }

  HAL_DutyCycleHandle handle = dutyCycleHandles->Allocate();
  if (handle == HAL_kInvalidHandle) {
    *status = NO_AVAILABLE_RESOURCES;
    return HAL_kInvalidHandle;
  }

  auto dutyCycle = dutyCycleHandles->Get(handle);
  uint32_t index = static_cast<uint32_t>(getHandleIndex(handle));
  dutyCycle->dutyCycle.reset(tDutyCycle::create(index, status));

  dutyCycle->dutyCycle->writeSource_AnalogTrigger(routingAnalogTrigger, status);
  dutyCycle->dutyCycle->writeSource_Channel(routingChannel, status);
  dutyCycle->dutyCycle->writeSource_Module(routingModule, status);

  return handle;
}
void HAL_FreeDutyCycle(HAL_DutyCycleHandle dutyCycleHandle) {
  // Just free it, the unique ptr will take care of everything else
  dutyCycleHandles->Free(dutyCycleHandle);
}

int32_t HAL_ReadDutyCycleFrequency(HAL_DutyCycleHandle dutyCycleHandle,
                                   int32_t* status) {
  auto dutyCycle = dutyCycleHandles->Get(dutyCycleHandle);
  if (!dutyCycle) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }

  return dutyCycle->dutyCycle->readFrequency(status);
}

int64_t HAL_ReadDutyCycleOutputRaw(HAL_DutyCycleHandle dutyCycleHandle,
                                   int32_t* status) {
  auto dutyCycle = dutyCycleHandles->Get(dutyCycleHandle);
  if (!dutyCycle) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }

  return dutyCycle->dutyCycle->readOutput(status);
}

double HAL_ReadDutyCycleOutputPercentage(HAL_DutyCycleHandle dutyCycleHandle,
                                   int32_t* status) {
  return ((double)HAL_ReadDutyCycleOutputRaw(dutyCycleHandle, status) / (4e7 - 1));
}
}