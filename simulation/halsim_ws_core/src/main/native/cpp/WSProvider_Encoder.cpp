/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "WSProvider_Encoder.h"

#include <hal/Ports.h>
#include <hal/simulation/EncoderData.h>

namespace wpilibws {

void HALSimWSProviderEncoder::Initialize(WSRegisterFunc webRegisterFunc) {
  CreateProviders<HALSimWSProviderEncoder>("Encoder", HAL_GetNumEncoders(),
                                           webRegisterFunc);
}

void HALSimWSProviderEncoder::RegisterCallbacks() {
  m_initCbKey = HALSIM_RegisterEncoderInitializedCallback(
      m_channel,
      [](const char* name, void* param, const struct HAL_Value* value) {
        static_cast<HALSimWSProviderEncoder*>(param)->ProcessHalCallback(
            {{"<init", static_cast<bool>(value->data.v_boolean)}});
      },
      this, true);

  m_countCbKey = HALSIM_RegisterEncoderCountCallback(
      m_channel,
      [](const char* name, void* param, const struct HAL_Value* value) {
        static_cast<HALSimWSProviderEncoder*>(param)->ProcessHalCallback(
            {{">count", value->data.v_int}});
      },
      this, true);

  m_periodCbKey = HALSIM_RegisterEncoderPeriodCallback(
      m_channel,
      [](const char* name, void* param, const struct HAL_Value* value) {
        static_cast<HALSimWSProviderEncoder*>(param)->ProcessHalCallback(
            {{">period", value->data.v_double}});
      },
      this, true);

  m_resetCbKey = HALSIM_RegisterEncoderResetCallback(
      m_channel,
      [](const char* name, void* param, const struct HAL_Value* value) {
        static_cast<HALSimWSProviderEncoder*>(param)->ProcessHalCallback(
            {{"<reset", static_cast<bool>(value->data.v_boolean)}});
      },
      this, true);

  m_reverseDirectionCbKey = HALSIM_RegisterEncoderReverseDirectionCallback(
      m_channel,
      [](const char* name, void* param, const struct HAL_Value* value) {
        static_cast<HALSimWSProviderEncoder*>(param)->ProcessHalCallback(
            {{"<reverse_direction", static_cast<bool>(value->data.v_boolean)}});
      },
      this, true);

  m_samplesCbKey = HALSIM_RegisterEncoderSamplesToAverageCallback(
      m_channel,
      [](const char* name, void* param, const struct HAL_Value* value) {
        static_cast<HALSimWSProviderEncoder*>(param)->ProcessHalCallback(
            {{"<samples_to_avg", value->data.v_int}});
      },
      this, true);
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
