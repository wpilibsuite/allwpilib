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
  CreateSingleProvider<HALSimWSProviderRoboRIO>(
      "RoboRIO", HALSIM_RegisterRoboRioAllCallbacks, webRegisterFunc);
}

wpi::json HALSimWSProviderRoboRIO::OnSimValueChanged(const char* cbName) {
  std::string cbType(cbName);

  if (cbType == "FPGAButton") {
    return {
      {">fpga_button", static_cast<bool>(HALSIM_GetRoboRioFPGAButton())}
    };
  }
  else if (cbType == "VInVoltage") {
    return {
      {">vin_voltage", HALSIM_GetRoboRioVInVoltage()}
    };
  }
  else if (cbType == "VInCurrent") {
    return {
      {">vin_current", HALSIM_GetRoboRioVInCurrent()}
    };
  }
  else if (cbType == "UserVoltage6V") {
    return {
      {">6v_voltage", HALSIM_GetRoboRioUserVoltage6V()}
    };
  }
  else if (cbType == "UserCurrent6V") {
    return {
      {">6v_current", HALSIM_GetRoboRioUserCurrent6V()}
    };
  }
  else if (cbType == "UserActive6V") {
    return {
      {">6v_active", static_cast<bool>(HALSIM_GetRoboRioUserActive6V())}
    };
  }
  else if (cbType == "UserFaults6V") {
    return {
      {">6v_faults", HALSIM_GetRoboRioUserFaults6V()}
    };
  }
  else if (cbType == "UserVoltage5V") {
    return {
      {">5v_voltage", HALSIM_GetRoboRioUserVoltage5V()}
    };
  }
  else if (cbType == "UserCurrent5V") {
    return {
      {">5v_current", HALSIM_GetRoboRioUserCurrent5V()}
    };
  }
  else if (cbType == "UserActive5V") {
    return {
      {">5v_active", static_cast<bool>(HALSIM_GetRoboRioUserActive5V())}
    };
  }
  else if (cbType == "UserFaults5V") {
    return {
      {">5v_faults", HALSIM_GetRoboRioUserFaults5V()}
    };
  }
  else if (cbType == "UserVoltage3V3") {
    return {
      {">3v3_voltage", HALSIM_GetRoboRioUserVoltage3V3()}
    };
  }
  else if (cbType == "UserCurrent3V3") {
    return {
      {">3v3_current", HALSIM_GetRoboRioUserCurrent3V3()}
    };
  }
  else if (cbType == "UserActive3V3") {
    return {
      {">3v3_active", static_cast<bool>(HALSIM_GetRoboRioUserActive3V3())}
    };
  }
  else if (cbType == "UserFaults3V3") {
    return {
      {">3v3_faults", HALSIM_GetRoboRioUserFaults3V3()}
    };
  }

  return {};
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
