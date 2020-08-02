/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "WSProvider_DIO.h"

#include <hal/Ports.h>
#include <hal/simulation/DIOData.h>

namespace wpilibws {

void HALSimWSProviderDIO::Initialize(WSRegisterFunc webRegisterFunc) {
  CreateProviders<HALSimWSProviderDIO>("DIO", HAL_GetNumDigitalChannels(),
                                       webRegisterFunc);
}

void HALSimWSProviderDIO::RegisterCallbacks() {
  m_initCbKey = HALSIM_RegisterDIOInitializedCallback(
      m_channel,
      [](const char* name, void* param, const struct HAL_Value* value) {
        static_cast<HALSimWSProviderDIO*>(param)->ProcessHalCallback(
            {{"<init", static_cast<bool>(value->data.v_boolean)}});
      },
      this, true);

  m_valueCbKey = HALSIM_RegisterDIOValueCallback(
      m_channel,
      [](const char* name, void* param, const struct HAL_Value* value) {
        static_cast<HALSimWSProviderDIO*>(param)->ProcessHalCallback(
            {{"<>value", static_cast<bool>(value->data.v_boolean)}});
      },
      this, true);

  m_pulseLengthCbKey = HALSIM_RegisterDIOPulseLengthCallback(
      m_channel,
      [](const char* name, void* param, const struct HAL_Value* value) {
        static_cast<HALSimWSProviderDIO*>(param)->ProcessHalCallback(
            {{"<pulse_length", value->data.v_double}});
      },
      this, true);

  m_inputCbKey = HALSIM_RegisterDIOIsInputCallback(
      m_channel,
      [](const char* name, void* param, const struct HAL_Value* value) {
        static_cast<HALSimWSProviderDIO*>(param)->ProcessHalCallback(
            {{"<input", static_cast<bool>(value->data.v_boolean)}});
      },
      this, true);
}

void HALSimWSProviderDIO::CancelCallbacks() {
  HALSIM_CancelDIOInitializedCallback(m_channel, m_initCbKey);
  HALSIM_CancelDIOValueCallback(m_channel, m_valueCbKey);
  HALSIM_CancelDIOPulseLengthCallback(m_channel, m_pulseLengthCbKey);
  HALSIM_CancelDIOIsInputCallback(m_channel, m_inputCbKey);
}

void HALSimWSProviderDIO::OnNetValueChanged(const wpi::json& json) {
  wpi::json::const_iterator it;
  if ((it = json.find("<>value")) != json.end()) {
    HALSIM_SetDIOValue(m_channel, static_cast<bool>(it.value()));
  }
}

}  // namespace wpilibws
