/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "../PortsInternal.h"
#include "EncoderDataInternal.h"

using namespace hal;

namespace hal {
namespace init {
void InitializeEncoderData() {
  static EncoderData sed[kNumEncoders];
  ::hal::SimEncoderData = sed;
}
}  // namespace init
}  // namespace hal

EncoderData* hal::SimEncoderData;
void EncoderData::ResetData() {
  digitalChannelA = 0;
  initialized.Reset(false);
  count.Reset(0);
  period.Reset(std::numeric_limits<double>::max());
  reset.Reset(false);
  maxPeriod.Reset(0);
  direction.Reset(false);
  reverseDirection.Reset(false);
  samplesToAverage.Reset(0);
  distancePerPulse.Reset(1);
}

extern "C" {
void HALSIM_ResetEncoderData(int32_t index) {
  SimEncoderData[index].ResetData();
}

int16_t HALSIM_GetDigitalChannelA(int32_t index) {
  return SimEncoderData[index].digitalChannelA;
}

#define DEFINE_CAPI(TYPE, CAPINAME, LOWERNAME)                  \
  HAL_SIMDATAVALUE_DEFINE_CAPI(TYPE, HALSIM, Encoder##CAPINAME, \
                               SimEncoderData, LOWERNAME)

DEFINE_CAPI(HAL_Bool, Initialized, initialized)
DEFINE_CAPI(int32_t, Count, count)
DEFINE_CAPI(double, Period, period)
DEFINE_CAPI(HAL_Bool, Reset, reset)
DEFINE_CAPI(double, MaxPeriod, maxPeriod)
DEFINE_CAPI(HAL_Bool, Direction, direction)
DEFINE_CAPI(HAL_Bool, ReverseDirection, reverseDirection)
DEFINE_CAPI(int32_t, SamplesToAverage, samplesToAverage)
DEFINE_CAPI(double, DistancePerPulse, distancePerPulse)

#define REGISTER(NAME) \
  SimEncoderData[index].NAME.RegisterCallback(callback, param, initialNotify)

void HALSIM_RegisterEncoderAllCallbacks(int32_t index,
                                        HAL_NotifyCallback callback,
                                        void* param, HAL_Bool initialNotify) {
  REGISTER(initialized);
  REGISTER(count);
  REGISTER(period);
  REGISTER(reset);
  REGISTER(maxPeriod);
  REGISTER(direction);
  REGISTER(reverseDirection);
  REGISTER(samplesToAverage);
  REGISTER(distancePerPulse);
}
}  // extern "C"
