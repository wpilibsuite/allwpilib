// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "../PortsInternal.h"
#include "PDPDataInternal.h"

using namespace hal;

namespace hal::init {
void InitializePDPData() {
  static PDPData spd[kNumPDPModules];
  ::hal::SimPDPData = spd;
}
}  // namespace hal::init

PDPData* hal::SimPDPData;
void PDPData::ResetData() {
  initialized.Reset(false);
  temperature.Reset(0.0);
  voltage.Reset(12.0);
  for (int i = 0; i < kNumPDPChannels; i++) {
    current[i].Reset(0.0);
  }
}

extern "C" {
void HALSIM_ResetPDPData(int32_t index) {
  SimPDPData[index].ResetData();
}

#define DEFINE_CAPI(TYPE, CAPINAME, LOWERNAME)                          \
  HAL_SIMDATAVALUE_DEFINE_CAPI(TYPE, HALSIM, PDP##CAPINAME, SimPDPData, \
                               LOWERNAME)

DEFINE_CAPI(HAL_Bool, Initialized, initialized)
DEFINE_CAPI(double, Temperature, temperature)
DEFINE_CAPI(double, Voltage, voltage)
HAL_SIMDATAVALUE_DEFINE_CAPI_CHANNEL(double, HALSIM, PDPCurrent, SimPDPData,
                                     current)

void HALSIM_GetPDPAllCurrents(int32_t index, double* currents) {
  auto& data = SimPDPData[index].current;
  for (int i = 0; i < kNumPDPChannels; i++) {
    currents[i] = data[i];
  }
}

void HALSIM_SetPDPAllCurrents(int32_t index, const double* currents) {
  auto& data = SimPDPData[index].current;
  for (int i = 0; i < kNumPDPChannels; i++) {
    data[i] = currents[i];
  }
}

#define REGISTER(NAME) \
  SimPDPData[index].NAME.RegisterCallback(callback, param, initialNotify)

void HALSIM_RegisterPDPAllNonCurrentCallbacks(int32_t index, int32_t channel,
                                              HAL_NotifyCallback callback,
                                              void* param,
                                              HAL_Bool initialNotify) {
  REGISTER(initialized);
  REGISTER(temperature);
  REGISTER(voltage);
}
}  // extern "C"
