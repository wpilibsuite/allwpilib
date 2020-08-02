/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "WSProvider_Analog.h"

#include <hal/Ports.h>
#include <hal/simulation/AnalogInData.h>
#include <hal/simulation/AnalogOutData.h>

namespace wpilibws {

void HALSimWSProviderAnalogIn::Initialize(WSRegisterFunc webRegisterFunc) {
  CreateProviders<HALSimWSProviderAnalogIn>("AI", HAL_GetNumAnalogInputs(),
                                            webRegisterFunc);
}

void HALSimWSProviderAnalogIn::RegisterCallbacks() {
  m_initCbKey = HALSIM_RegisterAnalogInInitializedCallback(
      m_channel,
      [](const char* name, void* param, const struct HAL_Value* value) {
        static_cast<HALSimWSProviderAnalogIn*>(param)->ProcessHalCallback(
            {{"<init", static_cast<bool>(value->data.v_boolean)}});
      },
      this, true);

  m_avgbitsCbKey = HALSIM_RegisterAnalogInAverageBitsCallback(
      m_channel,
      [](const char* name, void* param, const struct HAL_Value* value) {
        static_cast<HALSimWSProviderAnalogIn*>(param)->ProcessHalCallback(
            {{"<avg_bits", value->data.v_int}});
      },
      this, true);

  m_oversampleCbKey = HALSIM_RegisterAnalogInOversampleBitsCallback(
      m_channel,
      [](const char* name, void* param, const struct HAL_Value* value) {
        static_cast<HALSimWSProviderAnalogIn*>(param)->ProcessHalCallback(
            {{"<oversample_bits", value->data.v_int}});
      },
      this, true);

  m_voltageCbKey = HALSIM_RegisterAnalogInVoltageCallback(
      m_channel,
      [](const char* name, void* param, const struct HAL_Value* value) {
        static_cast<HALSimWSProviderAnalogIn*>(param)->ProcessHalCallback(
            {{">voltage", value->data.v_double}});
      },
      this, true);

  m_accumInitCbKey = HALSIM_RegisterAnalogInAccumulatorInitializedCallback(
      m_channel,
      [](const char* name, void* param, const struct HAL_Value* value) {
        static_cast<HALSimWSProviderAnalogIn*>(param)->ProcessHalCallback(
            {{"accum", {{"<init", static_cast<bool>(value->data.v_boolean)}}}});
      },
      this, true);

  m_accumValueCbKey = HALSIM_RegisterAnalogInAccumulatorValueCallback(
      m_channel,
      [](const char* name, void* param, const struct HAL_Value* value) {
        static_cast<HALSimWSProviderAnalogIn*>(param)->ProcessHalCallback(
            {{"accum", {{">value", value->data.v_long}}}});
      },
      this, true);

  m_accumCountCbKey = HALSIM_RegisterAnalogInAccumulatorCountCallback(
      m_channel,
      [](const char* name, void* param, const struct HAL_Value* value) {
        static_cast<HALSimWSProviderAnalogIn*>(param)->ProcessHalCallback(
            {{"accum", {{">count", value->data.v_long}}}});
      },
      this, true);

  m_accumCenterCbKey = HALSIM_RegisterAnalogInAccumulatorCenterCallback(
      m_channel,
      [](const char* name, void* param, const struct HAL_Value* value) {
        static_cast<HALSimWSProviderAnalogIn*>(param)->ProcessHalCallback(
            {{"accum", {{"<center", value->data.v_int}}}});
      },
      this, true);

  m_accumDeadbandCbKey = HALSIM_RegisterAnalogInAccumulatorDeadbandCallback(
      m_channel,
      [](const char* name, void* param, const struct HAL_Value* value) {
        static_cast<HALSimWSProviderAnalogIn*>(param)->ProcessHalCallback(
            {{"accum", {{"<deadband", value->data.v_int}}}});
      },
      this, true);
}

void HALSimWSProviderAnalogIn::CancelCallbacks() {
  // Cancel callbacks
  HALSIM_CancelAnalogInInitializedCallback(m_channel, m_initCbKey);
  HALSIM_CancelAnalogInAverageBitsCallback(m_channel, m_avgbitsCbKey);
  HALSIM_CancelAnalogInOversampleBitsCallback(m_channel, m_oversampleCbKey);
  HALSIM_CancelAnalogInVoltageCallback(m_channel, m_voltageCbKey);
  HALSIM_CancelAnalogInAccumulatorInitializedCallback(m_channel,
                                                      m_accumInitCbKey);
  HALSIM_CancelAnalogInAccumulatorValueCallback(m_channel, m_accumValueCbKey);
  HALSIM_CancelAnalogInAccumulatorCountCallback(m_channel, m_accumCountCbKey);
  HALSIM_CancelAnalogInAccumulatorCenterCallback(m_channel, m_accumCenterCbKey);
  HALSIM_CancelAnalogInAccumulatorDeadbandCallback(m_channel,
                                                   m_accumDeadbandCbKey);
}

void HALSimWSProviderAnalogIn::OnNetValueChanged(const wpi::json& json) {
  wpi::json::const_iterator it;
  if ((it = json.find(">voltage")) != json.end()) {
    HALSIM_SetAnalogInVoltage(m_channel, it.value());
  }
  if ((it = json.find("accum")) != json.end()) {
    auto accum = it.value();
    if ((it = accum.find(">value")) != accum.end()) {
      HALSIM_SetAnalogInAccumulatorValue(m_channel, it.value());
    }
    if ((it = json.find(">count")) != json.end()) {
      HALSIM_SetAnalogInAccumulatorCount(m_channel, it.value());
    }
  }
}

void HALSimWSProviderAnalogOut::Initialize(WSRegisterFunc webRegisterFunc) {
  CreateProviders<HALSimWSProviderAnalogOut>("AO", HAL_GetNumAnalogOutputs(),
                                             webRegisterFunc);
}

void HALSimWSProviderAnalogOut::RegisterCallbacks() {
  m_initCbKey = HALSIM_RegisterAnalogOutInitializedCallback(
      m_channel,
      [](const char* name, void* param, const struct HAL_Value* value) {
        static_cast<HALSimWSProviderAnalogOut*>(param)->ProcessHalCallback(
            {{"<init", static_cast<bool>(value->data.v_boolean)}});
      },
      this, true);

  m_voltageCbKey = HALSIM_RegisterAnalogOutVoltageCallback(
      m_channel,
      [](const char* name, void* param, const struct HAL_Value* value) {
        static_cast<HALSimWSProviderAnalogOut*>(param)->ProcessHalCallback(
            {{"<voltage", value->data.v_double}});
      },
      this, true);
}

void HALSimWSProviderAnalogOut::CancelCallbacks() {
  HALSIM_CancelAnalogOutInitializedCallback(m_channel, m_initCbKey);
  HALSIM_CancelAnalogOutVoltageCallback(m_channel, m_voltageCbKey);
}

}  // namespace wpilibws
