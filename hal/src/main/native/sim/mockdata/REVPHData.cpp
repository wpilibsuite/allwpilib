// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "../PortsInternal.h"
#include "REVPHDataInternal.h"

using namespace hal;

namespace hal::init {
void InitializeREVPHData() {
  static REVPHData spd[kNumREVPHModules];
  ::hal::SimREVPHData = spd;
}
}  // namespace hal::init

REVPHData* hal::SimREVPHData;
void REVPHData::ResetData() {
  for (int i = 0; i < kNumREVPHChannels; i++) {
    solenoidOutput[i].Reset(false);
  }
  initialized.Reset(false);
  compressorOn.Reset(false);
  compressorConfigType.Reset(HAL_REVPHCompressorConfigType_kDisabled);
  pressureSwitch.Reset(false);
  compressorCurrent.Reset(0.0);
}

extern "C" {
void HALSIM_ResetREVPHData(int32_t index) {
  SimREVPHData[index].ResetData();
}

#define DEFINE_CAPI(TYPE, CAPINAME, LOWERNAME)                              \
  HAL_SIMDATAVALUE_DEFINE_CAPI(TYPE, HALSIM, REVPH##CAPINAME, SimREVPHData, \
                               LOWERNAME)

HAL_SIMDATAVALUE_DEFINE_CAPI_CHANNEL(HAL_Bool, HALSIM, REVPHSolenoidOutput,
                                     SimREVPHData, solenoidOutput)
DEFINE_CAPI(HAL_Bool, Initialized, initialized)
DEFINE_CAPI(HAL_Bool, CompressorOn, compressorOn)
DEFINE_CAPI(HAL_REVPHCompressorConfigType, CompressorConfigType,
            compressorConfigType)
DEFINE_CAPI(HAL_Bool, PressureSwitch, pressureSwitch)
DEFINE_CAPI(double, CompressorCurrent, compressorCurrent)

void HALSIM_GetREVPHAllSolenoids(int32_t index, uint8_t* values) {
  auto& data = SimREVPHData[index].solenoidOutput;
  uint8_t ret = 0;
  for (int i = 0; i < kNumCTRESolenoidChannels; i++) {
    ret |= (data[i] << i);
  }
  *values = ret;
}

void HALSIM_SetREVPHAllSolenoids(int32_t index, uint8_t values) {
  auto& data = SimREVPHData[index].solenoidOutput;
  for (int i = 0; i < kNumCTRESolenoidChannels; i++) {
    data[i] = (values & 0x1) != 0;
    values >>= 1;
  }
}

#define REGISTER(NAME) \
  SimREVPHData[index].NAME.RegisterCallback(callback, param, initialNotify)

void HALSIM_RegisterREVPHAllNonSolenoidCallbacks(int32_t index,
                                                 HAL_NotifyCallback callback,
                                                 void* param,
                                                 HAL_Bool initialNotify) {
  REGISTER(initialized);
  REGISTER(compressorOn);
  REGISTER(compressorConfigType);
  REGISTER(pressureSwitch);
  REGISTER(compressorCurrent);
}

void HALSIM_RegisterREVPHAllSolenoidCallbacks(int32_t index, int32_t channel,
                                              HAL_NotifyCallback callback,
                                              void* param,
                                              HAL_Bool initialNotify) {
  REGISTER(solenoidOutput[channel]);
}
}  // extern "C"
