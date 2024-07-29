// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "../PortsInternal.h"
#include "RelayDataInternal.h"

using namespace hal;

namespace hal::init {
void InitializeRelayData() {
  static RelayData srd[kNumRelayHeaders];
  ::hal::SimRelayData = srd;
}
}  // namespace hal::init

RelayData* hal::SimRelayData;
void RelayData::ResetData() {
  initializedForward.Reset(false);
  initializedReverse.Reset(false);
  forward.Reset(false);
  reverse.Reset(false);
}

extern "C" {
void HALSIM_ResetRelayData(int32_t index) {
  SimRelayData[index].ResetData();
}

#define DEFINE_CAPI(TYPE, CAPINAME, LOWERNAME)                              \
  HAL_SIMDATAVALUE_DEFINE_CAPI(TYPE, HALSIM, Relay##CAPINAME, SimRelayData, \
                               LOWERNAME)

DEFINE_CAPI(HAL_Bool, InitializedForward, initializedForward)
DEFINE_CAPI(HAL_Bool, InitializedReverse, initializedReverse)
DEFINE_CAPI(HAL_Bool, Forward, forward)
DEFINE_CAPI(HAL_Bool, Reverse, reverse)

#define REGISTER(NAME) \
  SimRelayData[index].NAME.RegisterCallback(callback, param, initialNotify)

void HALSIM_RegisterRelayAllCallbacks(int32_t index,
                                      HAL_NotifyCallback callback, void* param,
                                      HAL_Bool initialNotify) {
  REGISTER(initializedForward);
  REGISTER(initializedReverse);
  REGISTER(forward);
  REGISTER(reverse);
}
}  // extern "C"
