// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "WSProvider_PDP.h"

#include <hal/Ports.h>
#include <hal/simulation/PDPData.h>

#define REGISTER(halsim, jsonid, ctype, haltype)                         \
  HALSIM_RegisterPDP##halsim##Callback(                                  \
      m_channel,                                                         \
      [](const char* name, void* param, const struct HAL_Value* value) { \
        static_cast<HALSimWSProviderPDP*>(param)->ProcessHalCallback(    \
            {{jsonid, static_cast<ctype>(value->data.v_##haltype)}});    \
      },                                                                 \
      this, true)

#define REGISTER_CURRENT_CHANNEL(jsonid, ctype, haltype)                  \
  HALSIM_RegisterPDPCurrentCallback(                                      \
      m_pdpIndex, m_channelIndex,                                         \
      [](const char* name, void* param, const struct HAL_Value* value) {  \
        static_cast<HALSimWSProviderPDPChannelCurrent*>(param)            \
            ->ProcessHalCallback(                                         \
                {{jsonid, static_cast<ctype>(value->data.v_##haltype)}}); \
      },                                                                  \
      this, true)

namespace wpilibws {
void HALSimWSProviderPDP::Initialize(WSRegisterFunc webRegisterFunc) {
  CreateProviders<HALSimWSProviderPDP>("PDP", HAL_GetNumPDPModules(),
                                       webRegisterFunc);
}

HALSimWSProviderPDP::~HALSimWSProviderPDP() {
  DoCancelCallbacks();
}

void HALSimWSProviderPDP::RegisterCallbacks() {
  m_initCbKey = REGISTER(Initialized, "<init", bool, boolean);
  m_temperatureCbKey = REGISTER(Temperature, ">temperature", double, double);
  m_voltageCbKey = REGISTER(Voltage, ">voltage", double, double);
}

void HALSimWSProviderPDP::CancelCallbacks() {
  DoCancelCallbacks();
}

void HALSimWSProviderPDP::DoCancelCallbacks() {
  HALSIM_CancelPDPInitializedCallback(m_channel, m_initCbKey);
  HALSIM_CancelPDPTemperatureCallback(m_channel, m_temperatureCbKey);
  HALSIM_CancelPDPVoltageCallback(m_channel, m_voltageCbKey);

  m_initCbKey = 0;
  m_temperatureCbKey = 0;
  m_voltageCbKey = 0;
}

void HALSimWSProviderPDPChannelCurrent::Initialize(
    WSRegisterFunc webRegisterFunc) {
  std::string prefix = "PDP";

  for (int32_t pdpIndex = 0; pdpIndex < HAL_GetNumPDPModules(); ++pdpIndex) {
    for (int32_t currentIndex = 0; currentIndex < HAL_GetNumPDPChannels();
         ++currentIndex) {
      auto key =
          (prefix + "/" + wpi::Twine(pdpIndex) + "/" + wpi::Twine(currentIndex))
              .str();
      auto ptr = std::make_unique<HALSimWSProviderPDPChannelCurrent>(
          pdpIndex, currentIndex, key, prefix);
      webRegisterFunc(key, std::move(ptr));
    }
  }
}

HALSimWSProviderPDPChannelCurrent::HALSimWSProviderPDPChannelCurrent(
    int32_t pdpChannel, int32_t currentChannel, const std::string& key,
    const std::string& type)
    : HALSimWSHalProvider(key, type),
      m_pdpIndex(pdpChannel),
      m_channelIndex(currentChannel) {
  m_deviceId =
      std::to_string(m_pdpIndex) + "/current/" + std::to_string(m_channelIndex);
}

HALSimWSProviderPDPChannelCurrent::~HALSimWSProviderPDPChannelCurrent() {
  DoCancelCallbacks();
}

void HALSimWSProviderPDPChannelCurrent::RegisterCallbacks() {
  m_currentCbKey = REGISTER_CURRENT_CHANNEL("<current", double, double);
}

void HALSimWSProviderPDPChannelCurrent::CancelCallbacks() {
  DoCancelCallbacks();
}

void HALSimWSProviderPDPChannelCurrent::DoCancelCallbacks() {
  HALSIM_CancelPDPCurrentCallback(m_pdpIndex, m_channelIndex, m_currentCbKey);

  m_currentCbKey = 0;
}
}  // namespace wpilibws
