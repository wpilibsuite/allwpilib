// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/hal/simulation/DriverStationData.h"

#include <cstring>

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "wpi/hal/DriverStation.h"
#include "wpi/util/StringExtras.hpp"

namespace wpi::hal {

TEST_CASE("DriverStationTest Joystick", "[hal][mockdata]") {
  HAL_JoystickAxes axes;
  HAL_JoystickPOVs povs;
  HAL_JoystickButtons buttons;

  // Check default values before anything has been set
  for (int joystickNum = 0; joystickNum < 6; ++joystickNum) {
    HAL_GetJoystickAxes(joystickNum, &axes);
    HAL_GetJoystickPOVs(joystickNum, &povs);
    HAL_GetJoystickButtons(joystickNum, &buttons);

    CHECK(0 == axes.available);
    for (int i = 0; i < HAL_MAX_JOYSTICK_AXES; ++i) {
      CHECK(0 == axes.axes[i]);
    }

    CHECK(0 == povs.available);
    for (int i = 0; i < HAL_MAX_JOYSTICK_POVS; ++i) {
      CHECK(0 == povs.povs[i]);
    }

    CHECK(0llu == buttons.available);
    CHECK(0llu == buttons.buttons);
  }

  HAL_JoystickAxes set_axes;
  std::memset(&set_axes, 0, sizeof(HAL_JoystickAxes));
  HAL_JoystickPOVs set_povs;
  std::memset(&set_povs, 0, sizeof(HAL_JoystickPOVs));
  HAL_JoystickButtons set_buttons;
  std::memset(&set_buttons, 0, sizeof(HAL_JoystickButtons));

  // Set values
  int joystickUnderTest = 4;
  set_axes.available = 0x1F;
  for (int i = 0; i < 5; ++i) {
    set_axes.axes[i] = i * 0.125;
  }

  set_povs.available = 0x7;
  set_povs.povs[0] = HAL_JOYSTICK_POV_UP;
  set_povs.povs[1] = HAL_JOYSTICK_POV_RIGHT;
  set_povs.povs[2] = HAL_JOYSTICK_POV_DOWN;

  set_buttons.available = 0xFF;
  set_buttons.buttons = 0xDEADBEEF;

  HALSIM_SetJoystickAxes(joystickUnderTest, &set_axes);
  HALSIM_SetJoystickPOVs(joystickUnderTest, &set_povs);
  HALSIM_SetJoystickButtons(joystickUnderTest, &set_buttons);

  HALSIM_NotifyDriverStationNewData();
  HAL_RefreshDSData();

  // Check the set values
  HAL_GetJoystickAxes(joystickUnderTest, &axes);
  HAL_GetJoystickPOVs(joystickUnderTest, &povs);
  HAL_GetJoystickButtons(joystickUnderTest, &buttons);

  CHECK(0x1F == axes.available);
  CHECK(axes.axes[0] == Catch::Approx(0.000).margin(0.000001));
  CHECK(axes.axes[1] == Catch::Approx(0.125).margin(0.000001));
  CHECK(axes.axes[2] == Catch::Approx(0.250).margin(0.000001));
  CHECK(axes.axes[3] == Catch::Approx(0.375).margin(0.000001));
  CHECK(axes.axes[4] == Catch::Approx(0.500).margin(0.000001));
  CHECK(axes.axes[5] == Catch::Approx(0).margin(
                            0.000001));  // Should not have been set, still 0
  CHECK(axes.axes[6] == Catch::Approx(0).margin(
                            0.000001));  // Should not have been set, still 0

  CHECK(0x7 == povs.available);
  CHECK(HAL_JOYSTICK_POV_UP == povs.povs[0]);
  CHECK(HAL_JOYSTICK_POV_RIGHT == povs.povs[1]);
  CHECK(HAL_JOYSTICK_POV_DOWN == povs.povs[2]);
  CHECK(0 == povs.povs[3]);  // Should not have been set, still 0
  CHECK(0 == povs.povs[4]);  // Should not have been set, still 0
  CHECK(0 == povs.povs[5]);  // Should not have been set, still 0
  CHECK(0 == povs.povs[6]);  // Should not have been set, still 0

  CHECK(0xFFllu == buttons.available);
  CHECK(0xDEADBEEFllu == buttons.buttons);

  // Reset
  HALSIM_ResetDriverStationData();
  HALSIM_NotifyDriverStationNewData();
  HAL_RefreshDSData();

  for (int joystickNum = 0; joystickNum < 6; ++joystickNum) {
    HAL_GetJoystickAxes(joystickNum, &axes);
    HAL_GetJoystickPOVs(joystickNum, &povs);
    HAL_GetJoystickButtons(joystickNum, &buttons);

    CHECK(0 == axes.available);
    for (int i = 0; i < HAL_MAX_JOYSTICK_AXES; ++i) {
      CHECK(0 == axes.axes[i]);
    }

    CHECK(0 == povs.available);
    for (int i = 0; i < HAL_MAX_JOYSTICK_POVS; ++i) {
      CHECK(0 == povs.povs[i]);
    }

    CHECK(0llu == buttons.available);
    CHECK(0llu == buttons.buttons);
  }
}

TEST_CASE("DriverStationTest EventInfo", "[hal][mockdata]") {
  constexpr std::string_view eventName = "UnitTest";
  HAL_MatchInfo info;
  wpi::util::format_to_n_c_str(info.eventName, sizeof(info.eventName),
                               eventName);
  info.matchNumber = 5;
  info.matchType = HAL_MatchType::HAL_MATCH_TYPE_QUALIFICATION;
  info.replayNumber = 42;
  HALSIM_SetMatchInfo(&info);

  HAL_RefreshDSData();

  HAL_MatchInfo dataBack;
  HAL_GetMatchInfo(&dataBack);

  CHECK(eventName == dataBack.eventName);
  CHECK(5 == dataBack.matchNumber);
  CHECK(HAL_MatchType::HAL_MATCH_TYPE_QUALIFICATION == dataBack.matchType);
  CHECK(42 == dataBack.replayNumber);
}

}  // namespace wpi::hal
