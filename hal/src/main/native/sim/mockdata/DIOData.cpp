/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "../PortsInternal.h"
#include "DIODataInternal.h"

using namespace hal;

namespace hal {
namespace init {
void InitializeDIOData() {
  static DIOData sdd[kNumDigitalChannels];
  ::hal::SimDIOData = sdd;
}
}  // namespace init
}  // namespace hal

DIOData* hal::SimDIOData;
void DIOData::ResetData() {
  initialized.Reset(false);
  simDevice = 0;
  value.Reset(true);
  pulseLength.Reset(0.0);
  isInput.Reset(true);
  filterIndex.Reset(-1);
  displayName.Reset();
}

extern "C" {
void HALSIM_ResetDIOData(int32_t index) { SimDIOData[index].ResetData(); }

HAL_SimDeviceHandle HALSIM_GetDIOSimDevice(int32_t index) {
  return SimDIOData[index].simDevice;
}

#define DEFINE_CAPI(TYPE, CAPINAME, LOWERNAME)                          \
  HAL_SIMDATAVALUE_DEFINE_CAPI(TYPE, HALSIM, DIO##CAPINAME, SimDIOData, \
                               LOWERNAME)

const char* HALSIM_GetDIODisplayName(int32_t index) {
  return SimDIOData[index].displayName.Get();
}
void HALSIM_SetDIODisplayName(int32_t index, const char* displayName) {
  SimDIOData[index].displayName.Set(displayName);
}

DEFINE_CAPI(HAL_Bool, Initialized, initialized)
DEFINE_CAPI(HAL_Bool, Value, value)
DEFINE_CAPI(double, PulseLength, pulseLength)
DEFINE_CAPI(HAL_Bool, IsInput, isInput)
DEFINE_CAPI(int32_t, FilterIndex, filterIndex)

#define REGISTER(NAME) \
  SimDIOData[index].NAME.RegisterCallback(callback, param, initialNotify)

void HALSIM_RegisterDIOAllCallbacks(int32_t index, HAL_NotifyCallback callback,
                                    void* param, HAL_Bool initialNotify) {
  REGISTER(initialized);
  REGISTER(value);
  REGISTER(pulseLength);
  REGISTER(isInput);
  REGISTER(filterIndex);
}
}  // extern "C"
