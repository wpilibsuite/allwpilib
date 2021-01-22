// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "WSProvider_DIO.h"

#include <hal/Ports.h>
#include <hal/simulation/DIOData.h>

#define REGISTER(halsim, jsonid, ctype, haltype)                         \
  HALSIM_RegisterDIO##halsim##Callback(                                  \
      m_channel,                                                         \
      [](const char* name, void* param, const struct HAL_Value* value) { \
        static_cast<HALSimWSProviderDIO*>(param)->ProcessHalCallback(    \
            {{jsonid, static_cast<ctype>(value->data.v_##haltype)}});    \
      },                                                                 \
      this, true)

namespace wpilibws {

void HALSimWSProviderDIO::Initialize(WSRegisterFunc webRegisterFunc) {
  CreateProviders<HALSimWSProviderDIO>("DIO", HAL_GetNumDigitalChannels(),
                                       webRegisterFunc);
}

HALSimWSProviderDIO::~HALSimWSProviderDIO() {
  DoCancelCallbacks();
}

void HALSimWSProviderDIO::RegisterCallbacks() {
  m_initCbKey = REGISTER(Initialized, "<init", bool, boolean);
  m_valueCbKey = REGISTER(Value, "<>value", bool, boolean);
  m_pulseLengthCbKey = REGISTER(PulseLength, "<pulse_length", double, double);
  m_inputCbKey = REGISTER(IsInput, "<input", bool, boolean);
}

void HALSimWSProviderDIO::CancelCallbacks() {
  DoCancelCallbacks();
}

void HALSimWSProviderDIO::DoCancelCallbacks() {
  HALSIM_CancelDIOInitializedCallback(m_channel, m_initCbKey);
  HALSIM_CancelDIOValueCallback(m_channel, m_valueCbKey);
  HALSIM_CancelDIOPulseLengthCallback(m_channel, m_pulseLengthCbKey);
  HALSIM_CancelDIOIsInputCallback(m_channel, m_inputCbKey);

  m_initCbKey = 0;
  m_valueCbKey = 0;
  m_pulseLengthCbKey = 0;
  m_inputCbKey = 0;
}

void HALSimWSProviderDIO::OnNetValueChanged(const wpi::json& json) {
  wpi::json::const_iterator it;
  if ((it = json.find("<>value")) != json.end()) {
    HALSIM_SetDIOValue(m_channel, static_cast<bool>(it.value()));
  }
}

}  // namespace wpilibws
