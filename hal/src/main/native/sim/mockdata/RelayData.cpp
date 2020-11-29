/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "../PortsInternal.h"
#include "RelayDataInternal.h"

using namespace hal;

namespace hal {
namespace init {
void InitializeRelayData() {
  static RelayData srd[kNumRelayHeaders];
  ::hal::SimRelayData = srd;
}
}  // namespace init
}  // namespace hal

RelayData* hal::SimRelayData;
void RelayData::ResetData() {
  initializedForward.Reset(false);
  initializedReverse.Reset(false);
  forward.Reset(false);
  reverse.Reset(false);
  displayName.Reset();
}

extern "C" {
void HALSIM_ResetRelayData(int32_t index) { SimRelayData[index].ResetData(); }

#define DEFINE_CAPI(TYPE, CAPINAME, LOWERNAME)                              \
  HAL_SIMDATAVALUE_DEFINE_CAPI(TYPE, HALSIM, Relay##CAPINAME, SimRelayData, \
                               LOWERNAME)

const char* HALSIM_GetRelayDisplayName(int32_t index) {
  return SimRelayData[index].displayName.Get();
}
void HALSIM_SetRelayDisplayName(int32_t index, const char* displayName) {
  SimRelayData[index].displayName.Set(displayName);
}

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
