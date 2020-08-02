/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "WSProvider_RoboRIO.h"

#include <hal/Ports.h>
#include <hal/simulation/RoboRioData.h>

namespace wpilibws {

void HALSimWSProviderRoboRIO::Initialize(WSRegisterFunc webRegisterFunc) {
  CreateSingleProvider<HALSimWSProviderRoboRIO>("RoboRIO", webRegisterFunc);
}

void HALSimWSProviderRoboRIO::RegisterCallbacks() {
  m_fpgaCbKey = HALSIM_RegisterRoboRioFPGAButtonCallback(
      [](const char* name, void* param, const struct HAL_Value* value) {
        static_cast<HALSimWSProviderRoboRIO*>(param)->ProcessHalCallback(
            {{">fpga_button", static_cast<bool>(value->data.v_boolean)}});
      },
      this, true);

  m_vinVoltageCbKey = HALSIM_RegisterRoboRioVInVoltageCallback(
      [](const char* name, void* param, const struct HAL_Value* value) {
        static_cast<HALSimWSProviderRoboRIO*>(param)->ProcessHalCallback(
            {{">vin_voltage", value->data.v_double}});
      },
      this, true);

  m_vinCurrentCbKey = HALSIM_RegisterRoboRioVInCurrentCallback(
      [](const char* name, void* param, const struct HAL_Value* value) {
        static_cast<HALSimWSProviderRoboRIO*>(param)->ProcessHalCallback(
            {{">vin_current", value->data.v_double}});
      },
      this, true);

  m_6vVoltageCbKey = HALSIM_RegisterRoboRioUserVoltage6VCallback(
      [](const char* name, void* param, const struct HAL_Value* value) {
        static_cast<HALSimWSProviderRoboRIO*>(param)->ProcessHalCallback(
            {{">6v_voltage", value->data.v_double}});
      },
      this, true);

  m_6vCurrentCbKey = HALSIM_RegisterRoboRioUserCurrent6VCallback(
      [](const char* name, void* param, const struct HAL_Value* value) {
        static_cast<HALSimWSProviderRoboRIO*>(param)->ProcessHalCallback(
            {{">6v_current", value->data.v_double}});
      },
      this, true);

  m_6vActiveCbKey = HALSIM_RegisterRoboRioUserActive6VCallback(
      [](const char* name, void* param, const struct HAL_Value* value) {
        static_cast<HALSimWSProviderRoboRIO*>(param)->ProcessHalCallback(
            {{">6v_active", static_cast<bool>(value->data.v_boolean)}});
      },
      this, true);

  m_6vFaultsCbKey = HALSIM_RegisterRoboRioUserFaults6VCallback(
      [](const char* name, void* param, const struct HAL_Value* value) {
        static_cast<HALSimWSProviderRoboRIO*>(param)->ProcessHalCallback(
            {{">6v_faults", value->data.v_int}});
      },
      this, true);

  m_5vVoltageCbKey = HALSIM_RegisterRoboRioUserVoltage5VCallback(
      [](const char* name, void* param, const struct HAL_Value* value) {
        static_cast<HALSimWSProviderRoboRIO*>(param)->ProcessHalCallback(
            {{">5v_voltage", value->data.v_double}});
      },
      this, true);

  m_5vCurrentCbKey = HALSIM_RegisterRoboRioUserCurrent5VCallback(
      [](const char* name, void* param, const struct HAL_Value* value) {
        static_cast<HALSimWSProviderRoboRIO*>(param)->ProcessHalCallback(
            {{">5v_current", value->data.v_double}});
      },
      this, true);

  m_5vActiveCbKey = HALSIM_RegisterRoboRioUserActive5VCallback(
      [](const char* name, void* param, const struct HAL_Value* value) {
        static_cast<HALSimWSProviderRoboRIO*>(param)->ProcessHalCallback(
            {{">5v_active", static_cast<bool>(value->data.v_boolean)}});
      },
      this, true);

  m_5vFaultsCbKey = HALSIM_RegisterRoboRioUserFaults5VCallback(
      [](const char* name, void* param, const struct HAL_Value* value) {
        static_cast<HALSimWSProviderRoboRIO*>(param)->ProcessHalCallback(
            {{">5v_faults", value->data.v_int}});
      },
      this, true);

  m_3v3VoltageCbKey = HALSIM_RegisterRoboRioUserVoltage3V3Callback(
      [](const char* name, void* param, const struct HAL_Value* value) {
        static_cast<HALSimWSProviderRoboRIO*>(param)->ProcessHalCallback(
            {{">3v3_voltage", value->data.v_double}});
      },
      this, true);

  m_3v3CurrentCbKey = HALSIM_RegisterRoboRioUserCurrent3V3Callback(
      [](const char* name, void* param, const struct HAL_Value* value) {
        static_cast<HALSimWSProviderRoboRIO*>(param)->ProcessHalCallback(
            {{">3v3_current", value->data.v_double}});
      },
      this, true);

  m_3v3ActiveCbKey = HALSIM_RegisterRoboRioUserActive3V3Callback(
      [](const char* name, void* param, const struct HAL_Value* value) {
        static_cast<HALSimWSProviderRoboRIO*>(param)->ProcessHalCallback(
            {{">3v3_active", static_cast<bool>(value->data.v_boolean)}});
      },
      this, true);

  m_3v3FaultsCbKey = HALSIM_RegisterRoboRioUserFaults3V3Callback(
      [](const char* name, void* param, const struct HAL_Value* value) {
        static_cast<HALSimWSProviderRoboRIO*>(param)->ProcessHalCallback(
            {{">3v3_faults", value->data.v_int}});
      },
      this, true);
}

void HALSimWSProviderRoboRIO::CancelCallbacks() {
  HALSIM_CancelRoboRioFPGAButtonCallback(m_fpgaCbKey);
  HALSIM_CancelRoboRioVInVoltageCallback(m_vinVoltageCbKey);
  HALSIM_CancelRoboRioVInCurrentCallback(m_vinCurrentCbKey);

  HALSIM_CancelRoboRioUserVoltage6VCallback(m_6vVoltageCbKey);
  HALSIM_CancelRoboRioUserCurrent6VCallback(m_6vCurrentCbKey);
  HALSIM_CancelRoboRioUserActive6VCallback(m_6vActiveCbKey);
  HALSIM_CancelRoboRioUserFaults6VCallback(m_6vFaultsCbKey);

  HALSIM_CancelRoboRioUserVoltage5VCallback(m_5vVoltageCbKey);
  HALSIM_CancelRoboRioUserCurrent5VCallback(m_5vCurrentCbKey);
  HALSIM_CancelRoboRioUserActive5VCallback(m_5vActiveCbKey);
  HALSIM_CancelRoboRioUserFaults5VCallback(m_5vFaultsCbKey);

  HALSIM_CancelRoboRioUserVoltage3V3Callback(m_3v3VoltageCbKey);
  HALSIM_CancelRoboRioUserCurrent3V3Callback(m_3v3CurrentCbKey);
  HALSIM_CancelRoboRioUserActive3V3Callback(m_3v3ActiveCbKey);
  HALSIM_CancelRoboRioUserFaults3V3Callback(m_3v3FaultsCbKey);
}

void HALSimWSProviderRoboRIO::OnNetValueChanged(const wpi::json& json) {
  wpi::json::const_iterator it;
  if ((it = json.find(">fpga_button")) != json.end()) {
    HALSIM_SetRoboRioFPGAButton(static_cast<bool>(it.value()));
  }

  if ((it = json.find(">vin_voltage")) != json.end()) {
    HALSIM_SetRoboRioVInVoltage(it.value());
  }
  if ((it = json.find(">vin_current")) != json.end()) {
    HALSIM_SetRoboRioVInCurrent(it.value());
  }

  if ((it = json.find(">6v_voltage")) != json.end()) {
    HALSIM_SetRoboRioUserVoltage6V(it.value());
  }
  if ((it = json.find(">6v_current")) != json.end()) {
    HALSIM_SetRoboRioUserCurrent6V(it.value());
  }
  if ((it = json.find(">6v_active")) != json.end()) {
    HALSIM_SetRoboRioUserActive6V(static_cast<bool>(it.value()));
  }
  if ((it = json.find(">6v_faults")) != json.end()) {
    HALSIM_SetRoboRioUserFaults6V(it.value());
  }

  if ((it = json.find(">5v_voltage")) != json.end()) {
    HALSIM_SetRoboRioUserVoltage5V(it.value());
  }
  if ((it = json.find(">5v_current")) != json.end()) {
    HALSIM_SetRoboRioUserCurrent5V(it.value());
  }
  if ((it = json.find(">5v_active")) != json.end()) {
    HALSIM_SetRoboRioUserActive5V(static_cast<bool>(it.value()));
  }
  if ((it = json.find(">5v_faults")) != json.end()) {
    HALSIM_SetRoboRioUserFaults5V(it.value());
  }

  if ((it = json.find(">3v3_voltage")) != json.end()) {
    HALSIM_SetRoboRioUserVoltage3V3(it.value());
  }
  if ((it = json.find(">3v3_current")) != json.end()) {
    HALSIM_SetRoboRioUserCurrent3V3(it.value());
  }
  if ((it = json.find(">3v3_active")) != json.end()) {
    HALSIM_SetRoboRioUserActive3V3(static_cast<bool>(it.value()));
  }
  if ((it = json.find(">3v3_faults")) != json.end()) {
    HALSIM_SetRoboRioUserFaults3V3(it.value());
  }
}

}  // namespace wpilibws
