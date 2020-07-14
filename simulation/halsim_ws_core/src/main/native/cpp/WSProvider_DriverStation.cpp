/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <algorithm>

#include "WSProvider_DriverStation.h"

#include <hal/DriverStation.h>
#include <hal/Ports.h>
#include <hal/simulation/DriverStationData.h>

namespace wpilibws {

void HALSimWSProviderDriverStation::Initialize(WSRegisterFunc webRegisterFunc) {
  // TODO: if this isn't called, our callbacks get erased. Fixed in 2021
  HAL_InitializeDriverStation();

  CreateSingleProvider<HALSimWSProviderDriverStation>(
      "DriverStation", HALSIM_RegisterDriverStationAllCallbacks,
      webRegisterFunc);
}

wpi::json HALSimWSProviderDriverStation::OnSimValueChanged(const char* cbName) {
  wpi::json joysticks;
  for (int i = 0; i < HAL_kMaxJoysticks; i++) {
    HAL_JoystickAxes axes;
    HAL_GetJoystickAxes(i, &axes);
    std::vector<float> axes_v(axes.axes, axes.axes + axes.count);

    HAL_JoystickButtons buttons;
    HAL_GetJoystickButtons(i, &buttons);
    std::vector<bool> buttons_v((size_t)buttons.count);
    for (uint8_t i = 0; i < buttons.count; i++) {
      buttons_v[i] = (bool)(buttons.buttons & 1 << (i - 1));
    }

    HAL_JoystickPOVs povs;
    HAL_GetJoystickPOVs(i, &povs);
    std::vector<int16_t> povs_v(povs.povs, povs.povs + povs.count);

    joysticks[std::to_string(i)] = {
        {">buttons", buttons_v},
        {">povs", povs_v},
        {">axes", axes_v},
    };
  }

  std::string station;
  switch (HALSIM_GetDriverStationAllianceStationId()) {
    case HAL_AllianceStationID_kRed1:
      station = "red1";
      break;
    case HAL_AllianceStationID_kBlue1:
      station = "blue1";
      break;
    case HAL_AllianceStationID_kRed2:
      station = "red2";
      break;
    case HAL_AllianceStationID_kBlue2:
      station = "blue2";
      break;
    case HAL_AllianceStationID_kRed3:
      station = "red3";
      break;
    case HAL_AllianceStationID_kBlue3:
      station = "blue3";
      break;
  }

  return {
      {">enabled", (bool)HALSIM_GetDriverStationEnabled()},
      {">autonomous", (bool)HALSIM_GetDriverStationAutonomous()},
      {">test", (bool)HALSIM_GetDriverStationTest()},
      {">estop", (bool)HALSIM_GetDriverStationEStop()},
      {">fms", (bool)HALSIM_GetDriverStationFmsAttached()},
      {">ds", (bool)HALSIM_GetDriverStationDsAttached()},
      {"<match_time", HALSIM_GetDriverStationMatchTime()},
      {">station", station},
      {"joysticks", joysticks},
  };
}

void HALSimWSProviderDriverStation::OnNetValueChanged(const wpi::json& json) {
  wpi::json::const_iterator it;
  if ((it = json.find(">enabled")) != json.end()) {
    HALSIM_SetDriverStationEnabled(it.value());
  }
  if ((it = json.find(">autonomous")) != json.end()) {
    HALSIM_SetDriverStationAutonomous(it.value());
  }
  if ((it = json.find(">test")) != json.end()) {
    HALSIM_SetDriverStationTest(it.value());
  }
  if ((it = json.find(">estop")) != json.end()) {
    HALSIM_SetDriverStationEStop(it.value());
  }
  if ((it = json.find(">fms")) != json.end()) {
    HALSIM_SetDriverStationFmsAttached(it.value());
  }
  if ((it = json.find(">ds")) != json.end()) {
    HALSIM_SetDriverStationDsAttached(it.value());
  }

  if ((it = json.find(">station")) != json.end()) {
    std::string station = it.value();
    if (station == "red1") {
      HALSIM_SetDriverStationAllianceStationId(HAL_AllianceStationID_kRed1);
    } else if (station == "red2") {
      HALSIM_SetDriverStationAllianceStationId(HAL_AllianceStationID_kRed2);
    } else if (station == "red3") {
      HALSIM_SetDriverStationAllianceStationId(HAL_AllianceStationID_kRed3);
    } else if (station == "blue1") {
      HALSIM_SetDriverStationAllianceStationId(HAL_AllianceStationID_kBlue1);
    } else if (station == "blue2") {
      HALSIM_SetDriverStationAllianceStationId(HAL_AllianceStationID_kBlue2);
    } else if (station == "blue3") {
      HALSIM_SetDriverStationAllianceStationId(HAL_AllianceStationID_kBlue3);
    }
  }

  if ((it = json.find("joysticks")) != json.end()) {
    auto joysticks = it.value();
    for (auto jit = joysticks.cbegin(); jit != joysticks.cend(); ++jit) {
      auto stick = jit.value();
      auto num = std::stoi(jit.key());

      if ((it = stick.find(">axes")) != stick.end()) {
        HAL_JoystickAxes axes{};
        axes.count = std::min(it.value().size(),
                              (wpi::json::size_type)HAL_kMaxJoystickAxes);
        for (int i = 0; i < axes.count; ++i) {
          axes.axes[i] = it.value()[i];
        }
        HALSIM_SetJoystickAxes(num, &axes);
      }

      if ((it = stick.find(">buttons")) != stick.end()) {
        HAL_JoystickButtons buttons{};
        buttons.count = std::min(it.value().size(), (wpi::json::size_type)32);
        for (int i = 0; i < buttons.count; ++i) {
          if (it.value()[i]) {
            buttons.buttons |= 1 << (i - 1);
          }
        }
        HALSIM_SetJoystickButtons(num, &buttons);
      }

      if ((it = stick.find(">povs")) != stick.end()) {
        HAL_JoystickPOVs povs{};
        povs.count = std::min(it.value().size(),
                              (wpi::json::size_type)HAL_kMaxJoystickPOVs);
        for (int i = 0; i < povs.count; ++i) {
          povs.povs[i] = it.value()[i];
        }
        HALSIM_SetJoystickPOVs(num, &povs);
      }
    }
  }

  HALSIM_NotifyDriverStationNewData();
}

}
