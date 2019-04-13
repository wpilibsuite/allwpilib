/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "../PortsInternal.h"
#include "DigitalPWMDataInternal.h"

using namespace hal;

namespace hal {
namespace init {
void InitializeDigitalPWMData() {
  static DigitalPWMData sdpd[kNumDigitalPWMOutputs];
  ::hal::SimDigitalPWMData = sdpd;
}
}  // namespace init
}  // namespace hal

DigitalPWMData* hal::SimDigitalPWMData;
void DigitalPWMData::ResetData() {
  initialized.Reset(false);
  dutyCycle.Reset(0.0);
  pin.Reset(0);
}

extern "C" {
void HALSIM_ResetDigitalPWMData(int32_t index) {
  SimDigitalPWMData[index].ResetData();
}

#define DEFINE_CAPI(TYPE, CAPINAME, LOWERNAME)                     \
  HAL_SIMDATAVALUE_DEFINE_CAPI(TYPE, HALSIM, DigitalPWM##CAPINAME, \
                               SimDigitalPWMData, LOWERNAME)

DEFINE_CAPI(HAL_Bool, Initialized, initialized)
DEFINE_CAPI(double, DutyCycle, dutyCycle)
DEFINE_CAPI(int32_t, Pin, pin)

#define REGISTER(NAME) \
  SimDigitalPWMData[index].NAME.RegisterCallback(callback, param, initialNotify)

void HALSIM_RegisterDigitalPWMAllCallbacks(int32_t index,
                                           HAL_NotifyCallback callback,
                                           void* param,
                                           HAL_Bool initialNotify) {
  REGISTER(initialized);
  REGISTER(dutyCycle);
  REGISTER(pin);
}
}  // extern "C"
