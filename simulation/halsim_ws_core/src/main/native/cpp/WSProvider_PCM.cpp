// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "WSProvider_PCM.h"

#include <hal/Ports.h>
#include <hal/simulation/CTREPCMData.h>

#define REGISTER_CTREPCM(halsim, jsonid, ctype, haltype)                 \
  HALSIM_RegisterCTREPCM##halsim##Callback(                              \
      m_channel,                                                         \
      [](const char* name, void* param, const struct HAL_Value* value) { \
        static_cast<HALSimWSProviderPCM*>(param)->ProcessHalCallback(    \
            {{jsonid, static_cast<ctype>(value->data.v_##haltype)}});    \
      },                                                                 \
      this, true)
namespace wpilibws {
void HALSimWSProviderPCM::Initialize(WSRegisterFunc webRegisterFunc) {
  CreateProviders<HALSimWSProviderPCM>("CTREPCM", HAL_GetNumCTREPCMModules(),
                                       webRegisterFunc);
}

HALSimWSProviderPCM::~HALSimWSProviderPCM() {
  DoCancelCallbacks();
}

void HALSimWSProviderPCM::RegisterCallbacks() {
  m_initCbKey = REGISTER_CTREPCM(Initialized, "<init", bool, boolean);
  m_onCbKey = REGISTER_CTREPCM(CompressorOn, ">on", bool, boolean);
  m_closedLoopCbKey =
      REGISTER_CTREPCM(ClosedLoopEnabled, "<closed_loop", bool, boolean);
  m_pressureSwitchCbKey =
      REGISTER_CTREPCM(PressureSwitch, ">pressure_switch", bool, boolean);
  m_currentCbKey =
      REGISTER_CTREPCM(CompressorCurrent, ">current", double, double);
}

void HALSimWSProviderPCM::CancelCallbacks() {
  DoCancelCallbacks();
}

void HALSimWSProviderPCM::DoCancelCallbacks() {
  HALSIM_CancelCTREPCMInitializedCallback(m_channel, m_initCbKey);
  HALSIM_CancelCTREPCMCompressorOnCallback(m_channel, m_onCbKey);
  HALSIM_CancelCTREPCMClosedLoopEnabledCallback(m_channel, m_closedLoopCbKey);
  HALSIM_CancelCTREPCMPressureSwitchCallback(m_channel, m_pressureSwitchCbKey);
  HALSIM_CancelCTREPCMCompressorCurrentCallback(m_channel, m_currentCbKey);

  m_initCbKey = 0;
  m_onCbKey = 0;
  m_closedLoopCbKey = 0;
  m_pressureSwitchCbKey = 0;
  m_currentCbKey = 0;
}

}  // namespace wpilibws
