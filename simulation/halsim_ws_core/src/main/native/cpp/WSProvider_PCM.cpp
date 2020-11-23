/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "WSProvider_PCM.h"

#include <hal/Ports.h>
#include <hal/simulation/PCMData.h>

#define REGISTER_COMPRESSOR(halsim, jsonid, ctype, haltype)                  \
  HALSIM_RegisterPCMCompressor##halsim##Callback(                            \
      m_channel,                                                             \
      [](const char* name, void* param, const struct HAL_Value* value) {     \
        static_cast<HALSimWSProviderCompressor*>(param)->ProcessHalCallback( \
            {{jsonid, static_cast<ctype>(value->data.v_##haltype)}});        \
      },                                                                     \
      this, true)

#define REGISTER_COMPRESSOR_NOPREFIX(halsim, jsonid, ctype, haltype)         \
  HALSIM_RegisterPCM##halsim##Callback(                                      \
      m_channel,                                                             \
      [](const char* name, void* param, const struct HAL_Value* value) {     \
        static_cast<HALSimWSProviderCompressor*>(param)->ProcessHalCallback( \
            {{jsonid, static_cast<ctype>(value->data.v_##haltype)}});        \
      },                                                                     \
      this, true)
#define REGISTER_SOLENOID(halsim, jsonid, ctype, haltype)                  \
  HALSIM_RegisterPCMSolenoid##halsim##Callback(                            \
      m_pcmIndex, m_channel,                                               \
      [](const char* name, void* param, const struct HAL_Value* value) {   \
        static_cast<HALSimWSProviderSolenoid*>(param)->ProcessHalCallback( \
            {{jsonid, static_cast<ctype>(value->data.v_##haltype)}});      \
      },                                                                   \
      this, true)
namespace wpilibws {
void HALSimWSProviderCompressor::Initialize(WSRegisterFunc webRegisterFunc) {
  CreateProviders<HALSimWSProviderCompressor>("Compressor", 1, webRegisterFunc);
}

HALSimWSProviderCompressor::~HALSimWSProviderCompressor() {
  DoCancelCallbacks();
}

void HALSimWSProviderCompressor::RegisterCallbacks() {
  m_initCbKey = REGISTER_COMPRESSOR(Initialized, "<initialized", bool, boolean);
  m_onCbKey = REGISTER_COMPRESSOR(On, "<>on", bool, boolean);
  m_closedLoopCbKey = REGISTER_COMPRESSOR_NOPREFIX(
      ClosedLoopEnabled, "<>closed_loop", bool, boolean);
  m_pressureSwitchCbKey = REGISTER_COMPRESSOR_NOPREFIX(
      PressureSwitch, "<pressure_switch", bool, boolean);
  m_currentCbKey = REGISTER_COMPRESSOR(Current, "<current", double, double);
}

void HALSimWSProviderCompressor::CancelCallbacks() { DoCancelCallbacks(); }

void HALSimWSProviderCompressor::DoCancelCallbacks() {
  HALSIM_CancelPCMCompressorInitializedCallback(m_channel, m_initCbKey);
  HALSIM_CancelPCMCompressorOnCallback(m_channel, m_onCbKey);
  HALSIM_CancelPCMClosedLoopEnabledCallback(m_channel, m_closedLoopCbKey);
  HALSIM_CancelPCMPressureSwitchCallback(m_channel, m_pressureSwitchCbKey);
  HALSIM_CancelPCMCompressorCurrentCallback(m_channel, m_currentCbKey);

  m_initCbKey = 0;
  m_onCbKey = 0;
  m_closedLoopCbKey = 0;
  m_pressureSwitchCbKey = 0;
  m_currentCbKey = 0;
}

void HALSimWSProviderSolenoid::Initialize(WSRegisterFunc webRegisterFunc) {
  CreateProviders<HALSimWSProviderSolenoid>(
      "Solenoid", HAL_GetNumSolenoidChannels(), webRegisterFunc);
}

HALSimWSProviderSolenoid::~HALSimWSProviderSolenoid() { DoCancelCallbacks(); }

void HALSimWSProviderSolenoid::RegisterCallbacks() {
  m_initCbKey = REGISTER_SOLENOID(Initialized, "<initialized", bool, boolean);
  m_outputCbKey = REGISTER_SOLENOID(Output, "<>output", bool, boolean);
}

void HALSimWSProviderSolenoid::CancelCallbacks() { DoCancelCallbacks(); }

void HALSimWSProviderSolenoid::DoCancelCallbacks() {
  HALSIM_CancelPCMSolenoidInitializedCallback(m_pcmIndex, m_channel,
                                              m_initCbKey);
  HALSIM_CancelPCMSolenoidOutputCallback(m_pcmIndex, m_channel, m_outputCbKey);

  m_initCbKey = 0;
  m_outputCbKey = 0;
}
}  // namespace wpilibws
