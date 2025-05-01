// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "hal/Counter.h"

#include "CounterInternal.h"
#include "HALInitializer.h"
#include "PortsInternal.h"
#include "hal/handles/HandlesInternal.h"
#include "hal/handles/LimitedHandleResource.h"

namespace hal {

LimitedHandleResource<HAL_CounterHandle, Counter, NUM_COUNTERS,
                      HAL_HandleEnum::Counter>* counterHandles;
}  // namespace hal

namespace hal::init {
void InitializeCounter() {
  static LimitedHandleResource<HAL_CounterHandle, Counter, NUM_COUNTERS,
                               HAL_HandleEnum::Counter>
      cH;
  counterHandles = &cH;
}
}  // namespace hal::init

extern "C" {
HAL_CounterHandle HAL_InitializeCounter(int channel, HAL_Bool risingEdge,
                                        const char* allocationLocation,
                                        int32_t* status) {
  hal::init::CheckInit();
  return 0;
}
void HAL_FreeCounter(HAL_CounterHandle counterHandle) {}
void HAL_SetCounterEdgeConfiguration(HAL_CounterHandle counterHandle,
                                     HAL_Bool risingEdge, int32_t* status) {}
void HAL_ResetCounter(HAL_CounterHandle counterHandle, int32_t* status) {}
int32_t HAL_GetCounter(HAL_CounterHandle counterHandle, int32_t* status) {
  return 0;
}
double HAL_GetCounterPeriod(HAL_CounterHandle counterHandle, int32_t* status) {
  return 0.0;
}
void HAL_SetCounterMaxPeriod(HAL_CounterHandle counterHandle, double maxPeriod,
                             int32_t* status) {}
HAL_Bool HAL_GetCounterStopped(HAL_CounterHandle counterHandle,
                               int32_t* status) {
  return false;
}
}  // extern "C"
