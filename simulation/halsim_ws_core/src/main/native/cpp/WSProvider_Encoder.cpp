/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "WSProvider_Encoder.h"

#include <hal/Ports.h>
#include <hal/simulation/EncoderData.h>

#define REGISTER(halsim, jsonid, ctype, haltype)                          \
  HALSIM_RegisterEncoder##halsim##Callback(                               \
      m_channel,                                                          \
      [](const char* name, void* param, const struct HAL_Value* value) {  \
        static_cast<HALSimWSProviderEncoder*>(param)->ProcessHalCallback( \
            {{jsonid, static_cast<ctype>(value->data.v_##haltype)}});     \
      },                                                                  \
      this, true)

namespace wpilibws {

void HALSimWSProviderEncoder::Initialize(WSRegisterFunc webRegisterFunc) {
  CreateProviders<HALSimWSProviderEncoder>("Encoder", HAL_GetNumEncoders(),
                                           webRegisterFunc);
}

void HALSimWSProviderEncoder::RegisterCallbacks() {
  // Special case for initialization since we will need to send
  // the digital channels down the line as well
  m_initCbKey = HALSIM_RegisterEncoderInitializedCallback(
      m_channel,
      [](const char* name, void* param, const struct HAL_Value* value) {
        auto provider = static_cast<HALSimWSProviderEncoder*>(param);
        bool init = static_cast<bool>(value->data.v_boolean);

        wpi::json payload = {{"<init", init}};

        if (init) {
          payload["<channel_a"] =
              HALSIM_GetEncoderDigitalChannelA(provider->GetChannel());
          payload["<channel_b"] =
              HALSIM_GetEncoderDigitalChannelB(provider->GetChannel());
        }

        provider->ProcessHalCallback(payload);
      },
      this, true);
  m_countCbKey = REGISTER(Count, ">count", int32_t, int);
  m_periodCbKey = REGISTER(Period, ">period", double, double);
  m_resetCbKey = REGISTER(Reset, "<reset", bool, boolean);
  m_reverseDirectionCbKey =
      REGISTER(ReverseDirection, "<reverse_direction", bool, boolean);
  m_samplesCbKey = REGISTER(SamplesToAverage, "<samples_to_avg", int32_t, int);
}

void HALSimWSProviderEncoder::CancelCallbacks() {
  HALSIM_CancelEncoderInitializedCallback(m_channel, m_initCbKey);
  HALSIM_CancelEncoderCountCallback(m_channel, m_countCbKey);
  HALSIM_CancelEncoderPeriodCallback(m_channel, m_periodCbKey);
  HALSIM_CancelEncoderResetCallback(m_channel, m_resetCbKey);
  HALSIM_CancelEncoderReverseDirectionCallback(m_channel,
                                               m_reverseDirectionCbKey);
  HALSIM_CancelEncoderSamplesToAverageCallback(m_channel, m_samplesCbKey);
}

void HALSimWSProviderEncoder::OnNetValueChanged(const wpi::json& json) {
  wpi::json::const_iterator it;
  if ((it = json.find(">count")) != json.end()) {
    HALSIM_SetEncoderCount(m_channel, it.value());
  }
  if ((it = json.find(">period")) != json.end()) {
    HALSIM_SetEncoderPeriod(m_channel, it.value());
  }
}

}  // namespace wpilibws
