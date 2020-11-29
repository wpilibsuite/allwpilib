/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2020 FIRST. All Rights Reserved.                        */
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
  digitalChannelB = 0;
  initialized.Reset(false);
  simDevice = 0;
  count.Reset(0);
  period.Reset(std::numeric_limits<double>::max());
  reset.Reset(false);
  maxPeriod.Reset(0);
  direction.Reset(false);
  reverseDirection.Reset(false);
  samplesToAverage.Reset(0);
  distancePerPulse.Reset(1);
  displayName.Reset();
}

extern "C" {
int32_t HALSIM_FindEncoderForChannel(int32_t channel) {
  for (int i = 0; i < kNumEncoders; ++i) {
    if (!SimEncoderData[i].initialized) continue;
    if (SimEncoderData[i].digitalChannelA == channel ||
        SimEncoderData[i].digitalChannelB == channel)
      return i;
  }
  return -1;
}

const char* HALSIM_GetEncoderDisplayName(int32_t index) {
  return SimEncoderData[index].displayName.Get();
}
void HALSIM_SetEncoderDisplayName(int32_t index, const char* displayName) {
  SimEncoderData[index].displayName.Set(displayName);
}

void HALSIM_ResetEncoderData(int32_t index) {
  SimEncoderData[index].ResetData();
}

int32_t HALSIM_GetEncoderDigitalChannelA(int32_t index) {
  return SimEncoderData[index].digitalChannelA;
}

int32_t HALSIM_GetEncoderDigitalChannelB(int32_t index) {
  return SimEncoderData[index].digitalChannelB;
}

HAL_SimDeviceHandle HALSIM_GetEncoderSimDevice(int32_t index) {
  return SimEncoderData[index].simDevice;
}

#define DEFINE_CAPI(TYPE, CAPINAME, LOWERNAME)                  \
  HAL_SIMDATAVALUE_DEFINE_CAPI(TYPE, HALSIM, Encoder##CAPINAME, \
                               SimEncoderData, LOWERNAME)

DEFINE_CAPI(HAL_Bool, Initialized, initialized)
DEFINE_CAPI(double, Period, period)
DEFINE_CAPI(HAL_Bool, Reset, reset)
DEFINE_CAPI(double, MaxPeriod, maxPeriod)
DEFINE_CAPI(HAL_Bool, Direction, direction)
DEFINE_CAPI(HAL_Bool, ReverseDirection, reverseDirection)
DEFINE_CAPI(int32_t, SamplesToAverage, samplesToAverage)
DEFINE_CAPI(double, DistancePerPulse, distancePerPulse)

int32_t HALSIM_RegisterEncoderCountCallback(int32_t index,
                                            HAL_NotifyCallback callback,
                                            void* param,
                                            HAL_Bool initialNotify) {
  return SimEncoderData[index].count.RegisterCallback(callback, param,
                                                      initialNotify);
}

void HALSIM_CancelEncoderCountCallback(int32_t index, int32_t uid) {
  SimEncoderData[index].count.CancelCallback(uid);
}

int32_t HALSIM_GetEncoderCount(int32_t index) {
  return SimEncoderData[index].count;
}

void HALSIM_SetEncoderCount(int32_t index, int32_t count) {
  SimEncoderData[index].count = count;
  SimEncoderData[index].reset = false;
}

void HALSIM_SetEncoderDistance(int32_t index, double distance) {
  auto& simData = SimEncoderData[index];
  simData.count = distance / simData.distancePerPulse;
  simData.reset = false;
}

double HALSIM_GetEncoderDistance(int32_t index) {
  auto& simData = SimEncoderData[index];
  return simData.count * simData.distancePerPulse;
}

void HALSIM_SetEncoderRate(int32_t index, double rate) {
  auto& simData = SimEncoderData[index];
  if (rate == 0) {
    simData.period = std::numeric_limits<double>::infinity();
    return;
  }

  simData.period = simData.distancePerPulse / rate;
}

double HALSIM_GetEncoderRate(int32_t index) {
  auto& simData = SimEncoderData[index];

  return simData.distancePerPulse / simData.period;
}

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
