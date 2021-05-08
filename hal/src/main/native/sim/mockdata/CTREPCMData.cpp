// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "../PortsInternal.h"
#include "CTREPCMDataInternal.h"

using namespace hal;

namespace hal::init {
void InitializeCTREPCMData() {
  static CTREPCMData spd[kNumCTREPCMModules];
  ::hal::SimCTREPCMData = spd;
}
}  // namespace hal::init

CTREPCMData* hal::SimCTREPCMData;
void CTREPCMData::ResetData() {
  for (int i = 0; i < kNumSolenoidChannels; i++) {
    solenoidInitialized[i].Reset(false);
    solenoidOutput[i].Reset(false);
  }
  anySolenoidInitialized.Reset(false);
  compressorInitialized.Reset(false);
  compressorOn.Reset(false);
  closedLoopEnabled.Reset(true);
  pressureSwitch.Reset(false);
  compressorCurrent.Reset(0.0);
}

extern "C" {
void HALSIM_ResetCTREPCMData(int32_t index) {
  SimCTREPCMData[index].ResetData();
}

#define DEFINE_CAPI(TYPE, CAPINAME, LOWERNAME)                          \
  HAL_SIMDATAVALUE_DEFINE_CAPI(TYPE, HALSIM, CTREPCM##CAPINAME, SimCTREPCMData, \
                               LOWERNAME)

HAL_SIMDATAVALUE_DEFINE_CAPI_CHANNEL(HAL_Bool, HALSIM, CTREPCMSolenoidInitialized,
                                     SimCTREPCMData, solenoidInitialized)
HAL_SIMDATAVALUE_DEFINE_CAPI_CHANNEL(HAL_Bool, HALSIM, CTREPCMSolenoidOutput,
                                     SimCTREPCMData, solenoidOutput)
DEFINE_CAPI(HAL_Bool, AnySolenoidInitialized, anySolenoidInitialized)
DEFINE_CAPI(HAL_Bool, CompressorInitialized, compressorInitialized)
DEFINE_CAPI(HAL_Bool, CompressorOn, compressorOn)
DEFINE_CAPI(HAL_Bool, ClosedLoopEnabled, closedLoopEnabled)
DEFINE_CAPI(HAL_Bool, PressureSwitch, pressureSwitch)
DEFINE_CAPI(double, CompressorCurrent, compressorCurrent)

void HALSIM_GetCTREPCMAllSolenoids(int32_t index, uint8_t* values) {
  auto& data = SimCTREPCMData[index].solenoidOutput;
  uint8_t ret = 0;
  for (int i = 0; i < kNumSolenoidChannels; i++) {
    ret |= (data[i] << i);
  }
  *values = ret;
}

void HALSIM_SetCTREPCMAllSolenoids(int32_t index, uint8_t values) {
  auto& data = SimCTREPCMData[index].solenoidOutput;
  for (int i = 0; i < kNumSolenoidChannels; i++) {
    data[i] = (values & 0x1) != 0;
    values >>= 1;
  }
}

#define REGISTER(NAME) \
  SimCTREPCMData[index].NAME.RegisterCallback(callback, param, initialNotify)

void HALSIM_RegisterCTREPCMAllNonSolenoidCallbacks(int32_t index,
                                               HAL_NotifyCallback callback,
                                               void* param,
                                               HAL_Bool initialNotify) {
  REGISTER(compressorInitialized);
  REGISTER(compressorOn);
  REGISTER(closedLoopEnabled);
  REGISTER(pressureSwitch);
  REGISTER(compressorCurrent);
}

void HALSIM_RegisterCTREPCMAllSolenoidCallbacks(int32_t index, int32_t channel,
                                            HAL_NotifyCallback callback,
                                            void* param,
                                            HAL_Bool initialNotify) {
  REGISTER(solenoidInitialized[channel]);
  REGISTER(solenoidOutput[channel]);
}
}  // extern "C"
