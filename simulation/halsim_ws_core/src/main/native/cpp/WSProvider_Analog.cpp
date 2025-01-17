// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "WSProvider_Analog.h"

#include <hal/Ports.h>
#include <hal/simulation/AnalogInData.h>

#define REGISTER_AIN(halsim, jsonid, ctype, haltype)                       \
  HALSIM_RegisterAnalogIn##halsim##Callback(                               \
      m_channel,                                                           \
      [](const char* name, void* param, const struct HAL_Value* value) {   \
        static_cast<HALSimWSProviderAnalogIn*>(param)->ProcessHalCallback( \
            {{jsonid, static_cast<ctype>(value->data.v_##haltype)}});      \
      },                                                                   \
      this, true)

namespace wpilibws {

void HALSimWSProviderAnalogIn::Initialize(WSRegisterFunc webRegisterFunc) {
  CreateProviders<HALSimWSProviderAnalogIn>("AI", HAL_GetNumAnalogInputs(),
                                            webRegisterFunc);
}

HALSimWSProviderAnalogIn::~HALSimWSProviderAnalogIn() {
  DoCancelCallbacks();
}

void HALSimWSProviderAnalogIn::RegisterCallbacks() {
  m_initCbKey = REGISTER_AIN(Initialized, "<init", bool, boolean);
  m_avgbitsCbKey = REGISTER_AIN(AverageBits, "<avg_bits", int32_t, int);
  m_oversampleCbKey =
      REGISTER_AIN(OversampleBits, "<oversample_bits", int32_t, int);
  m_voltageCbKey = REGISTER_AIN(Voltage, ">voltage", double, double);
}

void HALSimWSProviderAnalogIn::CancelCallbacks() {
  DoCancelCallbacks();
}

void HALSimWSProviderAnalogIn::DoCancelCallbacks() {
  // Cancel callbacks
  HALSIM_CancelAnalogInInitializedCallback(m_channel, m_initCbKey);
  HALSIM_CancelAnalogInAverageBitsCallback(m_channel, m_avgbitsCbKey);
  HALSIM_CancelAnalogInOversampleBitsCallback(m_channel, m_oversampleCbKey);
  HALSIM_CancelAnalogInVoltageCallback(m_channel, m_voltageCbKey);

  // Reset callback IDs
  m_initCbKey = 0;
  m_avgbitsCbKey = 0;
  m_oversampleCbKey = 0;
  m_voltageCbKey = 0;
}

void HALSimWSProviderAnalogIn::OnNetValueChanged(const wpi::json& json) {
  wpi::json::const_iterator it;
  if ((it = json.find(">voltage")) != json.end()) {
    HALSIM_SetAnalogInVoltage(m_channel, it.value());
  }
}

}  // namespace wpilibws
