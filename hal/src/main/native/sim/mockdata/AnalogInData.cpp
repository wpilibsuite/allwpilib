/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "../PortsInternal.h"
#include "AnalogInDataInternal.h"

using namespace hal;

namespace hal {
namespace init {
void InitializeAnalogInData() {
  static AnalogInData sind[kNumAnalogInputs];
  ::hal::SimAnalogInData = sind;
}
}  // namespace init
}  // namespace hal

AnalogInData* hal::SimAnalogInData;
void AnalogInData::ResetData() {
  initialized.Reset(false);
  averageBits.Reset(7);
  oversampleBits.Reset(0);
  voltage.Reset(0.0);
  accumulatorInitialized.Reset(false);
  accumulatorValue.Reset(0);
  accumulatorCount.Reset(0);
  accumulatorCenter.Reset(0);
  accumulatorDeadband.Reset(0);
}

extern "C" {
void HALSIM_ResetAnalogInData(int32_t index) {
  SimAnalogInData[index].ResetData();
}

#define DEFINE_CAPI(TYPE, CAPINAME, LOWERNAME)                   \
  HAL_SIMDATAVALUE_DEFINE_CAPI(TYPE, HALSIM, AnalogIn##CAPINAME, \
                               SimAnalogInData, LOWERNAME)

DEFINE_CAPI(HAL_Bool, Initialized, initialized)
DEFINE_CAPI(int32_t, AverageBits, averageBits)
DEFINE_CAPI(int32_t, OversampleBits, oversampleBits)
DEFINE_CAPI(double, Voltage, voltage)
DEFINE_CAPI(HAL_Bool, AccumulatorInitialized, accumulatorInitialized)
DEFINE_CAPI(int64_t, AccumulatorValue, accumulatorValue)
DEFINE_CAPI(int64_t, AccumulatorCount, accumulatorCount)
DEFINE_CAPI(int32_t, AccumulatorCenter, accumulatorCenter)
DEFINE_CAPI(int32_t, AccumulatorDeadband, accumulatorDeadband)

#define REGISTER(NAME) \
  SimAnalogInData[index].NAME.RegisterCallback(callback, param, initialNotify)

void HALSIM_RegisterAnalogInAllCallbacks(int32_t index,
                                         HAL_NotifyCallback callback,
                                         void* param, HAL_Bool initialNotify) {
  REGISTER(initialized);
  REGISTER(averageBits);
  REGISTER(oversampleBits);
  REGISTER(voltage);
  REGISTER(accumulatorInitialized);
  REGISTER(accumulatorValue);
  REGISTER(accumulatorCount);
  REGISTER(accumulatorCenter);
  REGISTER(accumulatorDeadband);
}
}  // extern "C"
