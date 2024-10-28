// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "WSProvider_AddressableLED.h"

#include <vector>

#include <hal/Ports.h>
#include <hal/simulation/AddressableLEDData.h>

#define REGISTER(halsim, jsonid, ctype, haltype)                          \
  HALSIM_RegisterAddressableLED##halsim##Callback(                        \
      m_channel,                                                          \
      [](const char* name, void* param, const struct HAL_Value* value) {  \
        static_cast<HALSimWSProviderAddressableLED*>(param)               \
            ->ProcessHalCallback(                                         \
                {{jsonid, static_cast<ctype>(value->data.v_##haltype)}}); \
      },                                                                  \
      this, true)
namespace wpilibws {
void HALSimWSProviderAddressableLED::Initialize(
    WSRegisterFunc webRegisterFunc) {
  CreateProviders<HALSimWSProviderAddressableLED>(
      "AddressableLED", HAL_GetNumAddressableLEDs(), webRegisterFunc);
}

HALSimWSProviderAddressableLED::~HALSimWSProviderAddressableLED() {
  DoCancelCallbacks();
}

void HALSimWSProviderAddressableLED::RegisterCallbacks() {
  m_initCbKey = REGISTER(Initialized, "<init", bool, boolean);
  m_outputPortCbKey = REGISTER(OutputPort, "<output_port", int32_t, int);
  m_lengthCbKey = REGISTER(Length, "<length", int32_t, int);
  m_runningCbKey = REGISTER(Running, "<running", bool, boolean);

  m_dataCbKey = HALSIM_RegisterAddressableLEDDataCallback(
      0,
      [](const char* name, void* param, const unsigned char* buffer,
         unsigned int count) {
        auto provider = static_cast<HALSimWSProviderAddressableLED*>(param);

        size_t numLeds = count / sizeof(HAL_AddressableLEDData);
        const HAL_AddressableLEDData* data =
            reinterpret_cast<const HAL_AddressableLEDData*>(buffer);

        std::vector<wpi::json> jsonData;

        for (size_t i = 0; i < numLeds; ++i) {
          jsonData.push_back(
              {{"r", data[i].r}, {"g", data[i].g}, {"b", data[i].b}});
        }

        wpi::json payload;
        payload["<data"] = jsonData;

        provider->ProcessHalCallback(payload);
      },
      this);
}

void HALSimWSProviderAddressableLED::CancelCallbacks() {
  DoCancelCallbacks();
}

void HALSimWSProviderAddressableLED::DoCancelCallbacks() {
  HALSIM_CancelAddressableLEDInitializedCallback(m_channel, m_initCbKey);
  HALSIM_CancelAddressableLEDOutputPortCallback(m_channel, m_outputPortCbKey);
  HALSIM_CancelAddressableLEDLengthCallback(m_channel, m_lengthCbKey);
  HALSIM_CancelAddressableLEDRunningCallback(m_channel, m_runningCbKey);
  HALSIM_CancelAddressableLEDDataCallback(m_channel, m_dataCbKey);

  m_initCbKey = 0;
  m_outputPortCbKey = 0;
  m_lengthCbKey = 0;
  m_runningCbKey = 0;
  m_dataCbKey = 0;
}
}  // namespace wpilibws
