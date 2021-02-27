// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "../PortsInternal.h"
#include "AnalogTriggerDataInternal.h"

using namespace hal;

namespace hal::init {
void InitializeAnalogTriggerData() {
  static AnalogTriggerData satd[kNumAnalogTriggers];
  ::hal::SimAnalogTriggerData = satd;
}
}  // namespace hal::init

AnalogTriggerData* hal::SimAnalogTriggerData;
void AnalogTriggerData::ResetData() {
  initialized.Reset(0);
  triggerLowerBound.Reset(0);
  triggerUpperBound.Reset(0);
  triggerMode.Reset(static_cast<HALSIM_AnalogTriggerMode>(0));
}

extern "C" {

int32_t HALSIM_FindAnalogTriggerForChannel(int32_t channel) {
  for (int i = 0; i < kNumAnalogTriggers; ++i) {
    if (SimAnalogTriggerData[i].initialized &&
        SimAnalogTriggerData[i].inputPort == channel) {
      return i;
    }
  }
  return -1;
}

void HALSIM_ResetAnalogTriggerData(int32_t index) {
  SimAnalogTriggerData[index].ResetData();
}

#define DEFINE_CAPI(TYPE, CAPINAME, LOWERNAME)                        \
  HAL_SIMDATAVALUE_DEFINE_CAPI(TYPE, HALSIM, AnalogTrigger##CAPINAME, \
                               SimAnalogTriggerData, LOWERNAME)

DEFINE_CAPI(HAL_Bool, Initialized, initialized)
DEFINE_CAPI(double, TriggerLowerBound, triggerLowerBound)
DEFINE_CAPI(double, TriggerUpperBound, triggerUpperBound)
DEFINE_CAPI(HALSIM_AnalogTriggerMode, TriggerMode, triggerMode)

#define REGISTER(NAME)                                               \
  SimAnalogTriggerData[index].NAME.RegisterCallback(callback, param, \
                                                    initialNotify)

void HALSIM_RegisterAnalogTriggerAllCallbacks(int32_t index,
                                              HAL_NotifyCallback callback,
                                              void* param,
                                              HAL_Bool initialNotify) {
  REGISTER(initialized);
  REGISTER(triggerLowerBound);
  REGISTER(triggerUpperBound);
  REGISTER(triggerMode);
}
}  // extern "C"
