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

#define REGISTER_AIN(halsim, jsonid, ctype, haltype)                       \
  HALSIM_RegisterAnalogIn##halsim##Callback(                               \
      m_channel,                                                           \
      [](const char* name, void* param, const struct HAL_Value* value) {   \
        static_cast<HALSimWSProviderAnalogIn*>(param)->ProcessHalCallback( \
            {{jsonid, static_cast<ctype>(value->data.v_##haltype)}});      \
      },                                                                   \
      this, true)

#define REGISTER_AIN_ACCUM(halsim, jsonid, ctype, haltype)                 \
  HALSIM_RegisterAnalogInAccumulator##halsim##Callback(                    \
      m_channel,                                                           \
      [](const char* name, void* param, const struct HAL_Value* value) {   \
        static_cast<HALSimWSProviderAnalogIn*>(param)->ProcessHalCallback( \
            {{"accum",                                                     \
              {{jsonid, static_cast<ctype>(value->data.v_##haltype)}}}});  \
      },                                                                   \
      this, true)

#define REGISTER_AOUT(halsim, jsonid, ctype, haltype)                       \
  HALSIM_RegisterAnalogOut##halsim##Callback(                               \
      m_channel,                                                            \
      [](const char* name, void* param, const struct HAL_Value* value) {    \
        static_cast<HALSimWSProviderAnalogOut*>(param)->ProcessHalCallback( \
            {{jsonid, static_cast<ctype>(value->data.v_##haltype)}});       \
      },                                                                    \
      this, true)

namespace wpilibws {

void HALSimWSProviderAnalogIn::Initialize(WSRegisterFunc webRegisterFunc) {
  CreateProviders<HALSimWSProviderAnalogIn>("AI", HAL_GetNumAnalogInputs(),
                                            webRegisterFunc);
}

void HALSimWSProviderAnalogIn::RegisterCallbacks() {
  m_initCbKey = REGISTER_AIN(Initialized, "<init", bool, boolean);
  m_avgbitsCbKey = REGISTER_AIN(AverageBits, "<avg_bits", int32_t, int);
  m_oversampleCbKey =
      REGISTER_AIN(OversampleBits, "<oversample_bits", int32_t, int);
  m_voltageCbKey = REGISTER_AIN(Voltage, ">voltage", double, double);

  m_accumInitCbKey = REGISTER_AIN_ACCUM(Initialized, "<init", bool, boolean);
  m_accumValueCbKey = REGISTER_AIN_ACCUM(Value, ">value", int64_t,
                                         long);  // NOLINT(runtime/int)
  m_accumCountCbKey = REGISTER_AIN_ACCUM(Count, ">count", int64_t,
                                         long);  // NOLINT(runtime/int)
  m_accumCenterCbKey = REGISTER_AIN_ACCUM(Center, "<center", int32_t, int);
  m_accumDeadbandCbKey =
      REGISTER_AIN_ACCUM(Deadband, "<deadband", int32_t, int);
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
  m_initCbKey = REGISTER_AOUT(Initialized, "<init", bool, boolean);
  m_voltageCbKey = REGISTER_AOUT(Voltage, "<voltage", double, double);
}

void HALSimWSProviderAnalogOut::CancelCallbacks() {
  HALSIM_CancelAnalogOutInitializedCallback(m_channel, m_initCbKey);
  HALSIM_CancelAnalogOutVoltageCallback(m_channel, m_voltageCbKey);
}

}  // namespace wpilibws
