// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "WSProvider_RoboRIO.h"

#include <hal/Ports.h>
#include <hal/simulation/RoboRioData.h>

#define REGISTER(halsim, jsonid, ctype, haltype)                          \
  HALSIM_RegisterRoboRio##halsim##Callback(                               \
      [](const char* name, void* param, const struct HAL_Value* value) {  \
        static_cast<HALSimWSProviderRoboRIO*>(param)->ProcessHalCallback( \
            {{jsonid, static_cast<ctype>(value->data.v_##haltype)}});     \
      },                                                                  \
      this, true)

namespace wpilibws {

void HALSimWSProviderRoboRIO::Initialize(WSRegisterFunc webRegisterFunc) {
  CreateSingleProvider<HALSimWSProviderRoboRIO>("RoboRIO", webRegisterFunc);
}

HALSimWSProviderRoboRIO::~HALSimWSProviderRoboRIO() {
  DoCancelCallbacks();
}

void HALSimWSProviderRoboRIO::RegisterCallbacks() {
  m_fpgaCbKey = REGISTER(FPGAButton, ">fpga_button", bool, boolean);
  m_vinVoltageCbKey = REGISTER(VInVoltage, ">vin_voltage", double, double);
  m_vinCurrentCbKey = REGISTER(VInCurrent, ">vin_current", double, double);

  m_6vVoltageCbKey = REGISTER(UserVoltage6V, ">6v_voltage", double, double);
  m_6vCurrentCbKey = REGISTER(UserCurrent6V, ">6v_current", double, double);
  m_6vActiveCbKey = REGISTER(UserActive6V, ">6v_active", bool, boolean);
  m_6vFaultsCbKey = REGISTER(UserFaults6V, ">6v_faults", int32_t, int);

  m_5vVoltageCbKey = REGISTER(UserVoltage5V, ">5v_voltage", double, double);
  m_5vCurrentCbKey = REGISTER(UserCurrent5V, ">5v_current", double, double);
  m_5vActiveCbKey = REGISTER(UserActive5V, ">5v_active", bool, boolean);
  m_5vFaultsCbKey = REGISTER(UserFaults5V, ">5v_faults", int32_t, int);

  m_3v3VoltageCbKey = REGISTER(UserVoltage3V3, ">3v3_voltage", double, double);
  m_3v3CurrentCbKey = REGISTER(UserCurrent3V3, ">3v3_current", double, double);
  m_3v3ActiveCbKey = REGISTER(UserActive3V3, ">3v3_active", bool, boolean);
  m_3v3FaultsCbKey = REGISTER(UserFaults3V3, ">3v3_faults", int32_t, int);
}

void HALSimWSProviderRoboRIO::CancelCallbacks() {
  DoCancelCallbacks();
}

void HALSimWSProviderRoboRIO::DoCancelCallbacks() {
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

  m_fpgaCbKey = 0;
  m_vinVoltageCbKey = 0;
  m_vinCurrentCbKey = 0;
  m_6vActiveCbKey = 0;
  m_6vCurrentCbKey = 0;
  m_6vFaultsCbKey = 0;
  m_6vVoltageCbKey = 0;
  m_5vActiveCbKey = 0;
  m_5vCurrentCbKey = 0;
  m_5vFaultsCbKey = 0;
  m_5vVoltageCbKey = 0;
  m_3v3ActiveCbKey = 0;
  m_3v3CurrentCbKey = 0;
  m_3v3FaultsCbKey = 0;
  m_3v3VoltageCbKey = 0;
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
