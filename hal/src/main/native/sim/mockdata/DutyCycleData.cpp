/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "../PortsInternal.h"
#include "DutyCycleDataInternal.h"

using namespace hal;

namespace hal {
namespace init {
void InitializeDutyCycleData() {
  static DutyCycleData sed[kNumDutyCycles];
  ::hal::SimDutyCycleData = sed;
}
}  // namespace init
}  // namespace hal

DutyCycleData* hal::SimDutyCycleData;

void DutyCycleData::ResetData() {
  digitalChannel = 0;
  initialized.Reset(false);
  simDevice = 0;
  frequency.Reset(0);
  output.Reset(0);
}

extern "C" {
int32_t HALSIM_FindDutyCycleForChannel(int32_t channel) {
  for (int i = 0; i < kNumDutyCycles; ++i) {
    if (SimDutyCycleData[i].initialized &&
        SimDutyCycleData[i].digitalChannel == channel)
      return i;
  }
  return -1;
}

void HALSIM_ResetDutyCycleData(int32_t index) {
  SimDutyCycleData[index].ResetData();
}
int32_t HALSIM_GetDutyCycleDigitalChannel(int32_t index) {
  return SimDutyCycleData[index].digitalChannel;
}

HAL_SimDeviceHandle HALSIM_GetDutyCycleSimDevice(int32_t index) {
  return SimDutyCycleData[index].simDevice;
}

#define DEFINE_CAPI(TYPE, CAPINAME, LOWERNAME)                    \
  HAL_SIMDATAVALUE_DEFINE_CAPI(TYPE, HALSIM, DutyCycle##CAPINAME, \
                               SimDutyCycleData, LOWERNAME)

DEFINE_CAPI(HAL_Bool, Initialized, initialized)
DEFINE_CAPI(int32_t, Frequency, frequency)
DEFINE_CAPI(double, Output, output)

#define REGISTER(NAME) \
  SimDutyCycleData[index].NAME.RegisterCallback(callback, param, initialNotify)

void HALSIM_RegisterDutyCycleAllCallbacks(int32_t index,
                                          HAL_NotifyCallback callback,
                                          void* param, HAL_Bool initialNotify) {
  REGISTER(initialized);
  REGISTER(frequency);
  REGISTER(output);
}

}  // extern "C"
