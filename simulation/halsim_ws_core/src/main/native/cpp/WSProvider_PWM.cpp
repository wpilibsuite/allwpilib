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
  m_speedCbKey = REGISTER(Speed, "<speed", double, double);
  m_positionCbKey = REGISTER(Position, "<position", double, double);
  m_rawCbKey = REGISTER(RawValue, "<raw", int32_t, int);
  m_periodScaleCbKey = REGISTER(PeriodScale, "<period_scale", int32_t, int);
  m_zeroLatchCbKey = REGISTER(ZeroLatch, "<zero_latch", bool, boolean);
}

void HALSimWSProviderPWM::CancelCallbacks() {
  DoCancelCallbacks();
}

void HALSimWSProviderPWM::DoCancelCallbacks() {
  HALSIM_CancelPWMInitializedCallback(m_channel, m_initCbKey);
  HALSIM_CancelPWMSpeedCallback(m_channel, m_speedCbKey);
  HALSIM_CancelPWMPositionCallback(m_channel, m_positionCbKey);
  HALSIM_CancelPWMRawValueCallback(m_channel, m_rawCbKey);
  HALSIM_CancelPWMPeriodScaleCallback(m_channel, m_periodScaleCbKey);
  HALSIM_CancelPWMZeroLatchCallback(m_channel, m_zeroLatchCbKey);

  m_initCbKey = 0;
  m_speedCbKey = 0;
  m_positionCbKey = 0;
  m_rawCbKey = 0;
  m_periodScaleCbKey = 0;
  m_zeroLatchCbKey = 0;
}

}  // namespace wpilibws
