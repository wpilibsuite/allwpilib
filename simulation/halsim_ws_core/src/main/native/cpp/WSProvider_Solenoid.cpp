// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "WSProvider_Solenoid.h"

#include <hal/Ports.h>
#include <hal/simulation/PCMData.h>

#define REGISTER_SOLENOID(halsim, jsonid, ctype, haltype)                  \
  HALSIM_RegisterPCMSolenoid##halsim##Callback(                            \
      m_pcmIndex, m_solenoidIndex,                                         \
      [](const char* name, void* param, const struct HAL_Value* value) {   \
        static_cast<HALSimWSProviderSolenoid*>(param)->ProcessHalCallback( \
            {{jsonid, static_cast<ctype>(value->data.v_##haltype)}});      \
      },                                                                   \
      this, true)

namespace wpilibws {
void HALSimWSProviderSolenoid::Initialize(WSRegisterFunc webRegisterFunc) {
  for (int32_t pcmIndex = 0; pcmIndex < HAL_GetNumPCMModules(); ++pcmIndex) {
    for (int32_t solenoidIndex = 0;
         solenoidIndex < HAL_GetNumSolenoidChannels(); ++solenoidIndex) {
      auto key =
          ("Solenoid/" + wpi::Twine(pcmIndex) + "," + wpi::Twine(solenoidIndex))
              .str();
      auto ptr = std::make_unique<HALSimWSProviderSolenoid>(
          pcmIndex, solenoidIndex, key, "Solenoid");
      webRegisterFunc(key, std::move(ptr));
    }
  }
}

HALSimWSProviderSolenoid::HALSimWSProviderSolenoid(int32_t pcmChannel,
                                                   int32_t solenoidChannel,
                                                   const std::string& key,
                                                   const std::string& type)
    : HALSimWSHalProvider(key, type),
      m_pcmIndex(pcmChannel),
      m_solenoidIndex(solenoidChannel) {
  m_deviceId =
      std::to_string(m_pcmIndex) + "," + std::to_string(solenoidChannel);
}

HALSimWSProviderSolenoid::~HALSimWSProviderSolenoid() {
  DoCancelCallbacks();
}

void HALSimWSProviderSolenoid::RegisterCallbacks() {
  m_initCbKey = REGISTER_SOLENOID(Initialized, "<init", bool, boolean);
  m_outputCbKey = REGISTER_SOLENOID(Output, "<output", bool, boolean);
}

void HALSimWSProviderSolenoid::CancelCallbacks() {
  DoCancelCallbacks();
}

void HALSimWSProviderSolenoid::DoCancelCallbacks() {
  HALSIM_CancelPCMSolenoidInitializedCallback(m_pcmIndex, m_solenoidIndex,
                                              m_initCbKey);
  HALSIM_CancelPCMSolenoidOutputCallback(m_pcmIndex, m_solenoidIndex,
                                         m_outputCbKey);

  m_initCbKey = 0;
  m_outputCbKey = 0;
}
}  // namespace wpilibws
