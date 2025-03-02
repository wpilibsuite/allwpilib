// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "WSProvider_PWM.h"

#include <hal/Ports.h>
#include <hal/simulation/PWMData.h>

#define REGISTER(halsim, jsonid, ctype, haltype)                         \
  HALSIM_RegisterPWM##halsim##Callback(                                  \
      m_channel,                                                         \
      [](const char* name, void* param, const struct HAL_Value* value) { \
        static_cast<HALSimWSProviderPWM*>(param)->ProcessHalCallback(    \
            {{jsonid, static_cast<ctype>(value->data.v_##haltype)}});    \
      },                                                                 \
      this, true)
namespace wpilibws {

void HALSimWSProviderPWM::Initialize(WSRegisterFunc webRegisterFunc) {
  CreateProviders<HALSimWSProviderPWM>("PWM", HAL_GetNumPWMChannels(),
                                       webRegisterFunc);
}

HALSimWSProviderPWM::~HALSimWSProviderPWM() {
  DoCancelCallbacks();
}

void HALSimWSProviderPWM::RegisterCallbacks() {
  m_initCbKey = REGISTER(Initialized, "<init", bool, boolean);
  m_rawCbKey = REGISTER(PulseMicrosecond, "<raw", int32_t, int);
  m_outputPeriodCbKey = REGISTER(OutputPeriod, "<output_period", int32_t, int);
}

void HALSimWSProviderPWM::CancelCallbacks() {
  DoCancelCallbacks();
}

void HALSimWSProviderPWM::DoCancelCallbacks() {
  HALSIM_CancelPWMInitializedCallback(m_channel, m_initCbKey);
  HALSIM_CancelPWMPulseMicrosecondCallback(m_channel, m_rawCbKey);
  HALSIM_CancelPWMOutputPeriodCallback(m_channel, m_outputPeriodCbKey);

  m_initCbKey = 0;
  m_rawCbKey = 0;
  m_outputPeriodCbKey = 0;
}

}  // namespace wpilibws
