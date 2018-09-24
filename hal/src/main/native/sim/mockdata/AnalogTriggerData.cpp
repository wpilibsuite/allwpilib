/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "../PortsInternal.h"
#include "AnalogTriggerDataInternal.h"

using namespace hal;

namespace hal {
namespace init {
void InitializeAnalogTriggerData() {
  static AnalogTriggerData satd[kNumAnalogTriggers];
  ::hal::SimAnalogTriggerData = satd;
}
}  // namespace init
}  // namespace hal

AnalogTriggerData* hal::SimAnalogTriggerData;
void AnalogTriggerData::ResetData() {
  initialized.Reset(0);
  triggerLowerBound.Reset(0);
  triggerUpperBound.Reset(0);
  triggerMode.Reset(static_cast<HALSIM_AnalogTriggerMode>(0));
}

extern "C" {
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
