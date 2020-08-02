/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "WSProvider_PWM.h"

#include <hal/Ports.h>
#include <hal/simulation/PWMData.h>

namespace wpilibws {

void HALSimWSProviderPWM::Initialize(WSRegisterFunc webRegisterFunc) {
  CreateProviders<HALSimWSProviderPWM>("PWM", HAL_GetNumPWMChannels(),
                                       webRegisterFunc);
}

void HALSimWSProviderPWM::RegisterCallbacks() {
  m_initCbKey = HALSIM_RegisterPWMInitializedCallback(
      m_channel,
      [](const char* name, void* param, const struct HAL_Value* value) {
        static_cast<HALSimWSProviderPWM*>(param)->ProcessHalCallback(
            {{"<init", static_cast<bool>(value->data.v_boolean)}});
      },
      this, true);

  m_speedCbKey = HALSIM_RegisterPWMSpeedCallback(
      m_channel,
      [](const char* name, void* param, const struct HAL_Value* value) {
        static_cast<HALSimWSProviderPWM*>(param)->ProcessHalCallback(
            {{"<speed", value->data.v_double}});
      },
      this, true);

  m_positionCbKey = HALSIM_RegisterPWMPositionCallback(
      m_channel,
      [](const char* name, void* param, const struct HAL_Value* value) {
        static_cast<HALSimWSProviderPWM*>(param)->ProcessHalCallback(
            {{"<position", value->data.v_double}});
      },
      this, true);

  m_rawCbKey = HALSIM_RegisterPWMRawValueCallback(
      m_channel,
      [](const char* name, void* param, const struct HAL_Value* value) {
        static_cast<HALSimWSProviderPWM*>(param)->ProcessHalCallback(
            {{"<raw", value->data.v_int}});
      },
      this, true);

  m_periodScaleCbKey = HALSIM_RegisterPWMPeriodScaleCallback(
      m_channel,
      [](const char* name, void* param, const struct HAL_Value* value) {
        static_cast<HALSimWSProviderPWM*>(param)->ProcessHalCallback(
            {{"<period_scale", value->data.v_int}});
      },
      this, true);

  m_zeroLatchCbKey = HALSIM_RegisterPWMZeroLatchCallback(
      m_channel,
      [](const char* name, void* param, const struct HAL_Value* value) {
        static_cast<HALSimWSProviderPWM*>(param)->ProcessHalCallback(
            {{"<zero_latch", static_cast<bool>(value->data.v_boolean)}});
      },
      this, true);
}

void HALSimWSProviderPWM::CancelCallbacks() {
  HALSIM_CancelPWMInitializedCallback(m_channel, m_initCbKey);
  HALSIM_CancelPWMSpeedCallback(m_channel, m_speedCbKey);
  HALSIM_CancelPWMPositionCallback(m_channel, m_positionCbKey);
  HALSIM_CancelPWMRawValueCallback(m_channel, m_rawCbKey);
  HALSIM_CancelPWMPeriodScaleCallback(m_channel, m_periodScaleCbKey);
  HALSIM_CancelPWMZeroLatchCallback(m_channel, m_zeroLatchCbKey);
}

}  // namespace wpilibws
