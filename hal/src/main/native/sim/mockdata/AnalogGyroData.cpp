/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "../PortsInternal.h"
#include "AnalogGyroDataInternal.h"

using namespace hal;

namespace hal {
namespace init {
void InitializeAnalogGyroData() {
  static AnalogGyroData agd[kNumAccumulators];
  ::hal::SimAnalogGyroData = agd;
}
}  // namespace init
}  // namespace hal

AnalogGyroData* hal::SimAnalogGyroData;
void AnalogGyroData::ResetData() {
  angle.Reset(0.0);
  rate.Reset(0.0);
  initialized.Reset(false);
  displayName.Reset();
}

extern "C" {
void HALSIM_ResetAnalogGyroData(int32_t index) {
  SimAnalogGyroData[index].ResetData();
}

#define DEFINE_CAPI(TYPE, CAPINAME, LOWERNAME)                     \
  HAL_SIMDATAVALUE_DEFINE_CAPI(TYPE, HALSIM, AnalogGyro##CAPINAME, \
                               SimAnalogGyroData, LOWERNAME)

const char* HALSIM_GetAnalogGyroDisplayName(int32_t index) {
  return SimAnalogGyroData[index].displayName.Get();
}
void HALSIM_SetAnalogGyroDisplayName(int32_t index, const char* displayName) {
  SimAnalogGyroData[index].displayName.Set(displayName);
}

DEFINE_CAPI(double, Angle, angle)
DEFINE_CAPI(double, Rate, rate)
DEFINE_CAPI(HAL_Bool, Initialized, initialized)

#define REGISTER(NAME) \
  SimAnalogGyroData[index].NAME.RegisterCallback(callback, param, initialNotify)

void HALSIM_RegisterAnalogGyroAllCallbacks(int32_t index,
                                           HAL_NotifyCallback callback,
                                           void* param,
                                           HAL_Bool initialNotify) {
  REGISTER(angle);
  REGISTER(rate);
  REGISTER(initialized);
}
}  // extern "C"
