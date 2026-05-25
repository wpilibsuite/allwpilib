// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/halsim/ws_core/WSProvider_AddressableLED.hpp"

#include <utility>
#include <vector>

#include "wpi/hal/Ports.h"
#include "wpi/hal/simulation/AddressableLEDData.h"

#define REGISTER(halsim, jsonid, ctype, haltype)                         \
  HALSIM_RegisterAddressableLED##halsim##Callback(                       \
      m_channel,                                                         \
      [](const char* name, void* param, const struct HAL_Value* value) { \
        static_cast<HALSimWSProviderAddressableLED*>(param)              \
            ->ProcessHalCallback(wpi::util::json::object(                \
                jsonid, static_cast<ctype>(value->data.v_##haltype)));   \
      },                                                                 \
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
  m_startCbKey = REGISTER(Start, "<start", int32_t, int);
  m_lengthCbKey = REGISTER(Length, "<length", int32_t, int);

  m_dataCbKey = HALSIM_RegisterAddressableLEDDataCallback(
      [](const char* name, void* param, const unsigned char* buffer,
         unsigned int count) {
        auto provider = static_cast<HALSimWSProviderAddressableLED*>(param);

        size_t numLeds = count / sizeof(HAL_AddressableLEDData);
        const HAL_AddressableLEDData* data =
            reinterpret_cast<const HAL_AddressableLEDData*>(buffer);

        auto jsonData = wpi::util::json::array();

        for (size_t i = 0; i < numLeds; ++i) {
          jsonData.emplace_back(
              wpi::util::json::object("r", static_cast<int64_t>(data[i].r), "g",
                                      static_cast<int64_t>(data[i].g), "b",
                                      static_cast<int64_t>(data[i].b)));
        }

        wpi::util::json payload;
        payload["<data"] = std::move(jsonData);

        provider->ProcessHalCallback(payload);
      },
      this);
}

void HALSimWSProviderAddressableLED::CancelCallbacks() {
  DoCancelCallbacks();
}

void HALSimWSProviderAddressableLED::DoCancelCallbacks() {
  HALSIM_CancelAddressableLEDInitializedCallback(m_channel, m_initCbKey);
  HALSIM_CancelAddressableLEDStartCallback(m_channel, m_startCbKey);
  HALSIM_CancelAddressableLEDLengthCallback(m_channel, m_lengthCbKey);
  HALSIM_CancelAddressableLEDDataCallback(m_dataCbKey);

  m_initCbKey = 0;
  m_startCbKey = 0;
  m_lengthCbKey = 0;
  m_dataCbKey = 0;
}
}  // namespace wpilibws
