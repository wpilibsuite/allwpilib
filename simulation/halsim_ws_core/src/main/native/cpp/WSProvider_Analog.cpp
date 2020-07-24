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
                                            HALSIM_RegisterAnalogInAllCallbacks,
                                            webRegisterFunc);
}

wpi::json HALSimWSProviderAnalogIn::OnSimValueChanged(const char* cbName) {
  std::string cbType(cbName);
  bool sendDiffOnly = (cbType != "");

  wpi::json result;

  if (cbType == "Initialized" || !sendDiffOnly) {
    result["<init"] =
        static_cast<bool>(HALSIM_GetAnalogInInitialized(m_channel));
    if (sendDiffOnly) return result;
  }

  if (cbType == "AverageBits" || !sendDiffOnly) {
    result["<avg_bit"] = HALSIM_GetAnalogInAverageBits(m_channel);
    if (sendDiffOnly) return result;
  }

  if (cbType == "OversampleBits" || !sendDiffOnly) {
    result["<oversample_bits"] = HALSIM_GetAnalogInOversampleBits(m_channel);
    if (sendDiffOnly) return result;
  }

  if (cbType == "Voltage" || !sendDiffOnly) {
    result[">voltage"] = HALSIM_GetAnalogInVoltage(m_channel);
    if (sendDiffOnly) return result;
  }

  if (cbType == "AccumulatorInitialized" || !sendDiffOnly) {
    if (result.find("accum") == result.end()) {
      result["accum"] = wpi::json({});
    }
    result["accum"]["<init"] =
        static_cast<bool>(HALSIM_GetAnalogInAccumulatorInitialized(m_channel));
    if (sendDiffOnly) return result;
  }

  if (cbType == "AccumulatorValue" || !sendDiffOnly) {
    if (result.find("accum") == result.end()) {
      result["accum"] = wpi::json({});
    }
    result["accum"][">value"] = HALSIM_GetAnalogInAccumulatorValue(m_channel);
    if (sendDiffOnly) return result;
  }

  if (cbType == "AccumulatorCount" || !sendDiffOnly) {
    if (result.find("accum") == result.end()) {
      result["accum"] = wpi::json({});
    }
    result["accum"][">count"] = HALSIM_GetAnalogInAccumulatorCount(m_channel);
    if (sendDiffOnly) return result;
  }

  if (cbType == "AccumulatorCenter" || !sendDiffOnly) {
    if (result.find("accum") == result.end()) {
      result["accum"] = wpi::json({});
    }
    result["accum"]["<center"] = HALSIM_GetAnalogInAccumulatorCenter(m_channel);
    if (sendDiffOnly) return result;
  }

  if (cbType == "AccumulatorDeadband" || !sendDiffOnly) {
    if (result.find("accum") == result.end()) {
      result["accum"] = wpi::json({});
    }
    result["accum"]["<deadband"] =
        HALSIM_GetAnalogInAccumulatorDeadband(m_channel);
    if (sendDiffOnly) return result;
  }

  return result;
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
  CreateProviders<HALSimWSProviderAnalogOut>(
      "AO", HAL_GetNumAnalogOutputs(), HALSIM_RegisterAnalogOutAllCallbacks,
      webRegisterFunc);
}

wpi::json HALSimWSProviderAnalogOut::OnSimValueChanged(const char* cbName) {
  std::string cbType(cbName);
  bool sendDiffOnly = (cbType != "");

  wpi::json result;

  if (cbType == "Initialized" || !sendDiffOnly) {
    result["<init"] =
        static_cast<bool>(HALSIM_GetAnalogOutInitialized(m_channel));
    if (sendDiffOnly) return result;
  }

  if (cbType == "Voltage" || !sendDiffOnly) {
    result["<voltage"] = HALSIM_GetAnalogOutVoltage(m_channel);
    if (sendDiffOnly) return result;
  }

  return result;
}

}  // namespace wpilibws
