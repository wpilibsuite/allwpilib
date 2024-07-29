// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "../PortsInternal.h"
#include "PowerDistributionDataInternal.h"

using namespace hal;

namespace hal::init {
void InitializePowerDistributionData() {
  static PowerDistributionData spd[kNumPDSimModules];
  ::hal::SimPowerDistributionData = spd;
}
}  // namespace hal::init

PowerDistributionData* hal::SimPowerDistributionData;
void PowerDistributionData::ResetData() {
  initialized.Reset(false);
  temperature.Reset(0.0);
  voltage.Reset(12.0);
  for (int i = 0; i < kNumPDSimChannels; i++) {
    current[i].Reset(0.0);
  }
}

extern "C" {
void HALSIM_ResetPowerDistributionData(int32_t index) {
  SimPowerDistributionData[index].ResetData();
}

#define DEFINE_CAPI(TYPE, CAPINAME, LOWERNAME)                            \
  HAL_SIMDATAVALUE_DEFINE_CAPI(TYPE, HALSIM, PowerDistribution##CAPINAME, \
                               SimPowerDistributionData, LOWERNAME)

DEFINE_CAPI(HAL_Bool, Initialized, initialized)
DEFINE_CAPI(double, Temperature, temperature)
DEFINE_CAPI(double, Voltage, voltage)
HAL_SIMDATAVALUE_DEFINE_CAPI_CHANNEL(double, HALSIM, PowerDistributionCurrent,
                                     SimPowerDistributionData, current)

void HALSIM_GetPowerDistributionAllCurrents(int32_t index, double* currents,
                                            int length) {
  auto& data = SimPowerDistributionData[index].current;
  int toCopy = (std::min)(length, kNumPDSimChannels);
  for (int i = 0; i < toCopy; i++) {
    currents[i] = data[i];
  }
}

void HALSIM_SetPowerDistributionAllCurrents(int32_t index,
                                            const double* currents,
                                            int length) {
  auto& data = SimPowerDistributionData[index].current;
  int toCopy = (std::min)(length, kNumPDSimChannels);
  for (int i = 0; i < toCopy; i++) {
    data[i] = currents[i];
  }
}

#define REGISTER(NAME)                                                   \
  SimPowerDistributionData[index].NAME.RegisterCallback(callback, param, \
                                                        initialNotify)

void HALSIM_RegisterPowerDistributionAllNonCurrentCallbacks(
    int32_t index, int32_t channel, HAL_NotifyCallback callback, void* param,
    HAL_Bool initialNotify) {
  REGISTER(initialized);
  REGISTER(temperature);
  REGISTER(voltage);
}
}  // extern "C"
