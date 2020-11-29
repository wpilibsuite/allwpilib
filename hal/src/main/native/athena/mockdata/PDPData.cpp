/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "hal/simulation/PDPData.h"

#include "../PortsInternal.h"
#include "hal/simulation/SimDataValue.h"

extern "C" {
void HALSIM_ResetPDPData(int32_t index) {}

#define DEFINE_CAPI(TYPE, CAPINAME, RETURN) \
  HAL_SIMDATAVALUE_STUB_CAPI(TYPE, HALSIM, PDP##CAPINAME, RETURN)

const char* HALSIM_GePDPDisplayName(int32_t index) { return ""; }
void HALSIM_SetPDPDisplayName(int32_t index, const char* displayName) {}
DEFINE_CAPI(HAL_Bool, Initialized, false)
DEFINE_CAPI(double, Temperature, 0)
DEFINE_CAPI(double, Voltage, 0)
HAL_SIMDATAVALUE_STUB_CAPI_CHANNEL(double, HALSIM, PDPCurrent, 0)

void HALSIM_GetPDPAllCurrents(int32_t index, double* currents) {
  for (int i = 0; i < hal::kNumPDPChannels; i++) currents[i] = 0;
}

void HALSIM_SetPDPAllCurrents(int32_t index, const double* currents) {}

void HALSIM_RegisterPDPAllNonCurrentCallbacks(int32_t index, int32_t channel,
                                              HAL_NotifyCallback callback,
                                              void* param,
                                              HAL_Bool initialNotify) {}
}  // extern "C"
