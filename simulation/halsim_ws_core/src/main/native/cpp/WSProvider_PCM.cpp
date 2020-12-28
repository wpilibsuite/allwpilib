// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "WSProvider_PCM.h"

#include <hal/Ports.h>
#include <hal/simulation/PCMData.h>

int32_t GetPcmChannelsToPublish() {
  // TODO should we do all 64?
  return 2;
}

#define REGISTER_PCM(halsim, jsonid, ctype, haltype)                     \
  HALSIM_RegisterPCM##halsim##Callback(                                  \
      m_channel,                                                         \
      [](const char* name, void* param, const struct HAL_Value* value) { \
        static_cast<HALSimWSProviderPCM*>(param)->ProcessHalCallback(    \
            {{jsonid, static_cast<ctype>(value->data.v_##haltype)}});    \
      },                                                                 \
      this, true)

#define REGISTER_SOLENOID(halsim, jsonid, ctype, haltype)                     \
  HALSIM_RegisterPCMSolenoid##halsim##Callback(                               \
      m_pcmIndex, m_solenoidIndex,                                            \
      [](const char* name, void* param, const struct HAL_Value* value) {      \
        static_cast<HALSimWSProviderPCMSolenoid*>(param)->ProcessHalCallback( \
            {{jsonid, static_cast<ctype>(value->data.v_##haltype)}});         \
      },                                                                      \
      this, true)
namespace wpilibws {
void HALSimWSProviderPCM::Initialize(WSRegisterFunc webRegisterFunc) {
  CreateProviders<HALSimWSProviderPCM>("PCM", GetPcmChannelsToPublish(),
                                       webRegisterFunc);
}

HALSimWSProviderPCM::~HALSimWSProviderPCM() { DoCancelCallbacks(); }

void HALSimWSProviderPCM::RegisterCallbacks() {
  m_initCbKey = REGISTER_PCM(CompressorInitialized, "<init", bool, boolean);
  m_onCbKey = REGISTER_PCM(CompressorInitialized, ">on", bool, boolean);
  m_closedLoopCbKey =
      REGISTER_PCM(ClosedLoopEnabled, "<closed_loop", bool, boolean);
  m_pressureSwitchCbKey =
      REGISTER_PCM(PressureSwitch, ">pressure_switch", bool, boolean);
  m_currentCbKey = REGISTER_PCM(CompressorCurrent, ">current", double, double);
}

void HALSimWSProviderPCM::CancelCallbacks() { DoCancelCallbacks(); }

void HALSimWSProviderPCM::DoCancelCallbacks() {
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

void HALSimWSProviderPCMSolenoid::Initialize(WSRegisterFunc webRegisterFunc) {
  std::string prefix = "PCM";

  for (int32_t pcmIndex = 0; pcmIndex < GetPcmChannelsToPublish(); ++pcmIndex) {
    for (int32_t solenoidIndex = 0;
         solenoidIndex < HAL_GetNumSolenoidChannels(); ++solenoidIndex) {
      auto key = (prefix + "/" + wpi::Twine(pcmIndex) + "/" +
                  wpi::Twine(solenoidIndex))
                     .str();
      auto ptr = std::make_unique<HALSimWSProviderPCMSolenoid>(
          pcmIndex, solenoidIndex, key, prefix);
      webRegisterFunc(key, std::move(ptr));
    }
  }
}

HALSimWSProviderPCMSolenoid::HALSimWSProviderPCMSolenoid(
    int32_t pcmChannel, int32_t solenoidChannel, const std::string& key,
    const std::string& type)
    : HALSimWSHalProvider(key, type),
      m_pcmIndex(pcmChannel),
      m_solenoidIndex(solenoidChannel) {
  m_deviceId = std::to_string(m_pcmIndex) + "/solenoids/" +
               std::to_string(solenoidChannel);
}

HALSimWSProviderPCMSolenoid::~HALSimWSProviderPCMSolenoid() {
  DoCancelCallbacks();
}

void HALSimWSProviderPCMSolenoid::RegisterCallbacks() {
  m_initCbKey = REGISTER_SOLENOID(Initialized, "<init", bool, boolean);
  m_outputCbKey = REGISTER_SOLENOID(Output, "<>output", bool, boolean);
}

void HALSimWSProviderPCMSolenoid::CancelCallbacks() { DoCancelCallbacks(); }

void HALSimWSProviderPCMSolenoid::DoCancelCallbacks() {
  HALSIM_CancelPCMSolenoidInitializedCallback(m_pcmIndex, m_solenoidIndex,
                                              m_initCbKey);
  HALSIM_CancelPCMSolenoidOutputCallback(m_pcmIndex, m_solenoidIndex,
                                         m_outputCbKey);

  m_initCbKey = 0;
  m_outputCbKey = 0;
}
}  // namespace wpilibws
