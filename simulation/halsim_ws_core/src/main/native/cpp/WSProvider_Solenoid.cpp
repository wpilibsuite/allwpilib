// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "WSProvider_Solenoid.h"

#include <memory>
#include <string>
#include <utility>

#include <fmt/format.h>
#include <hal/Ports.h>
#include <hal/simulation/CTREPCMData.h>

#define REGISTER_SOLENOID(halsim, jsonid, ctype, haltype)                  \
  HALSIM_RegisterCTREPCMSolenoid##halsim##Callback(                        \
      m_pcmIndex, m_solenoidIndex,                                         \
      [](const char* name, void* param, const struct HAL_Value* value) {   \
        static_cast<HALSimWSProviderSolenoid*>(param)->ProcessHalCallback( \
            {{jsonid, static_cast<ctype>(value->data.v_##haltype)}});      \
      },                                                                   \
      this, true)

namespace wpilibws {
void HALSimWSProviderSolenoid::Initialize(WSRegisterFunc webRegisterFunc) {
  for (int32_t CTREPCMIndex = 0; CTREPCMIndex < HAL_GetNumCTREPCMModules();
       ++CTREPCMIndex) {
    for (int32_t solenoidIndex = 0;
         solenoidIndex < HAL_GetNumCTRESolenoidChannels(); ++solenoidIndex) {
      auto key = fmt::format("Solenoid/{},{}", CTREPCMIndex, solenoidIndex);
      auto ptr = std::make_unique<HALSimWSProviderSolenoid>(
          CTREPCMIndex, solenoidIndex, key, "Solenoid");
      webRegisterFunc(key, std::move(ptr));
    }
  }
}

HALSimWSProviderSolenoid::HALSimWSProviderSolenoid(int32_t CTREPCMChannel,
                                                   int32_t solenoidChannel,
                                                   const std::string& key,
                                                   const std::string& type)
    : HALSimWSHalProvider(key, type),
      m_pcmIndex(CTREPCMChannel),
      m_solenoidIndex(solenoidChannel) {
  m_deviceId = fmt::format("{},{}", m_pcmIndex, solenoidChannel);
}

HALSimWSProviderSolenoid::~HALSimWSProviderSolenoid() {
  DoCancelCallbacks();
}

void HALSimWSProviderSolenoid::RegisterCallbacks() {
  // m_initCbKey = REGISTER_SOLENOID(Initialized, "<init", bool, boolean);
  m_outputCbKey = REGISTER_SOLENOID(Output, "<output", bool, boolean);
}

void HALSimWSProviderSolenoid::CancelCallbacks() {
  DoCancelCallbacks();
}

void HALSimWSProviderSolenoid::DoCancelCallbacks() {
  // HALSIM_CancelCTREPCMSolenoidInitializedCallback(m_pcmIndex,
  // m_solenoidIndex,
  //                                                 m_initCbKey);
  HALSIM_CancelCTREPCMSolenoidOutputCallback(m_pcmIndex, m_solenoidIndex,
                                             m_outputCbKey);

  m_initCbKey = 0;
  m_outputCbKey = 0;
}
}  // namespace wpilibws
