// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/halsim/ws_core/WSProvider_Analog.hpp"

#include "wpi/hal/Ports.h"
#include "wpi/hal/simulation/AnalogInData.h"

#define REGISTER_AIN(halsim, jsonid, ctype, haltype)                       \
  HALSIM_RegisterAnalogIn##halsim##Callback(                               \
      m_channel,                                                           \
      [](const char* name, void* param, const struct HAL_Value* value) {   \
        static_cast<HALSimWSProviderAnalogIn*>(param)->ProcessHalCallback( \
            wpi::util::json::object(                                       \
                jsonid, static_cast<ctype>(value->data.v_##haltype)));     \
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
  m_voltageCbKey = REGISTER_AIN(Voltage, ">voltage", double, double);
}

void HALSimWSProviderAnalogIn::CancelCallbacks() {
  DoCancelCallbacks();
}

void HALSimWSProviderAnalogIn::DoCancelCallbacks() {
  // Cancel callbacks
  HALSIM_CancelAnalogInInitializedCallback(m_channel, m_initCbKey);
  HALSIM_CancelAnalogInVoltageCallback(m_channel, m_voltageCbKey);

  // Reset callback IDs
  m_initCbKey = 0;
  m_voltageCbKey = 0;
}

void HALSimWSProviderAnalogIn::OnNetValueChanged(const wpi::util::json& json) {
  if (auto val = json.lookup(">voltage"); val && val->is_number()) {
    HALSIM_SetAnalogInVoltage(m_channel, val->get_number());
  }
}

}  // namespace wpilibws
