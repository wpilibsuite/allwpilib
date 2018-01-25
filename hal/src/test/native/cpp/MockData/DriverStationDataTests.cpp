/*----------------------------------------------------------------------------*/
/* Copyright (c) 2015-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <cstring>

#include "HAL/HAL.h"
#include "HAL/Solenoid.h"
#include "MockData/DriverStationData.h"
#include "gtest/gtest.h"

namespace hal {

TEST(DriverStationTests, JoystickTests) {
  HAL_JoystickAxes axes;
  HAL_JoystickPOVs povs;
  HAL_JoystickButtons buttons;

  // Check default values before anything has been set
  for (int joystickNum = 0; joystickNum < 6; ++joystickNum) {
    HAL_GetJoystickAxes(joystickNum, &axes);
    HAL_GetJoystickPOVs(joystickNum, &povs);
    HAL_GetJoystickButtons(joystickNum, &buttons);

    EXPECT_EQ(0, axes.count);
    for (int i = 0; i < HAL_kMaxJoystickAxes; ++i) {
      EXPECT_EQ(0, axes.axes[i]);
    }

    EXPECT_EQ(0, povs.count);
    for (int i = 0; i < HAL_kMaxJoystickPOVs; ++i) {
      EXPECT_EQ(0, povs.povs[i]);
    }

    EXPECT_EQ(0, buttons.count);
    EXPECT_EQ(0u, buttons.buttons);
  }

  HAL_JoystickAxes set_axes;
  std::memset(&set_axes, 0, sizeof(HAL_JoystickAxes));
  HAL_JoystickPOVs set_povs;
  std::memset(&set_povs, 0, sizeof(HAL_JoystickPOVs));
  HAL_JoystickButtons set_buttons;
  std::memset(&set_buttons, 0, sizeof(HAL_JoystickButtons));

  // Set values
  int joystickUnderTest = 4;
  set_axes.count = 5;
  for (int i = 0; i < set_axes.count; ++i) {
    set_axes.axes[i] = i * .125;
  }

  set_povs.count = 3;
  for (int i = 0; i < set_povs.count; ++i) {
    set_povs.povs[i] = i * 15 + 12;
  }

  set_buttons.count = 8;
  set_buttons.buttons = 0xDEADBEEF;

  HALSIM_SetJoystickAxes(joystickUnderTest, &set_axes);
  HALSIM_SetJoystickPOVs(joystickUnderTest, &set_povs);
  HALSIM_SetJoystickButtons(joystickUnderTest, &set_buttons);

  // Check the set values
  HAL_GetJoystickAxes(joystickUnderTest, &axes);
  HAL_GetJoystickPOVs(joystickUnderTest, &povs);
  HAL_GetJoystickButtons(joystickUnderTest, &buttons);

  EXPECT_EQ(5, axes.count);
  EXPECT_NEAR(0.000, axes.axes[0], 0.000001);
  EXPECT_NEAR(0.125, axes.axes[1], 0.000001);
  EXPECT_NEAR(0.250, axes.axes[2], 0.000001);
  EXPECT_NEAR(0.375, axes.axes[3], 0.000001);
  EXPECT_NEAR(0.500, axes.axes[4], 0.000001);
  EXPECT_NEAR(0, axes.axes[5], 0.000001);  // Should not have been set, still 0
  EXPECT_NEAR(0, axes.axes[6], 0.000001);  // Should not have been set, still 0

  EXPECT_EQ(3, povs.count);
  EXPECT_EQ(12, povs.povs[0]);
  EXPECT_EQ(27, povs.povs[1]);
  EXPECT_EQ(42, povs.povs[2]);
  EXPECT_EQ(0, povs.povs[3]);  // Should not have been set, still 0
  EXPECT_EQ(0, povs.povs[4]);  // Should not have been set, still 0
  EXPECT_EQ(0, povs.povs[5]);  // Should not have been set, still 0
  EXPECT_EQ(0, povs.povs[6]);  // Should not have been set, still 0

  EXPECT_EQ(8, buttons.count);
  EXPECT_EQ(0xDEADBEEFu, buttons.buttons);

  // Reset
  HALSIM_ResetDriverStationData();
  for (int joystickNum = 0; joystickNum < 6; ++joystickNum) {
    HAL_GetJoystickAxes(joystickNum, &axes);
    HAL_GetJoystickPOVs(joystickNum, &povs);
    HAL_GetJoystickButtons(joystickNum, &buttons);

    EXPECT_EQ(0, axes.count);
    for (int i = 0; i < HAL_kMaxJoystickAxes; ++i) {
      EXPECT_EQ(0, axes.axes[i]);
    }

    EXPECT_EQ(0, povs.count);
    for (int i = 0; i < HAL_kMaxJoystickPOVs; ++i) {
      EXPECT_EQ(0, povs.povs[i]);
    }

    EXPECT_EQ(0, buttons.count);
    EXPECT_EQ(0u, buttons.buttons);
  }
}

TEST(DriverStationTests, EventInfoTest) {
  std::string eventName = "UnitTest";
  std::string gameData = "Insert game specific info here :D";
  HAL_MatchInfo info;
  info.eventName = const_cast<char*>(eventName.c_str());
  info.gameSpecificMessage = const_cast<char*>(gameData.c_str());
  info.matchNumber = 5;
  info.matchType = HAL_MatchType::HAL_kMatchType_qualification;
  info.replayNumber = 42;
  HALSIM_SetMatchInfo(&info);

  HAL_MatchInfo dataBack;
  HAL_GetMatchInfo(&dataBack);

  EXPECT_STREQ(eventName.c_str(), dataBack.eventName);
  EXPECT_STREQ(gameData.c_str(), dataBack.gameSpecificMessage);
  EXPECT_EQ(5, dataBack.matchNumber);
  EXPECT_EQ(HAL_MatchType::HAL_kMatchType_qualification, dataBack.matchType);
  EXPECT_EQ(42, dataBack.replayNumber);

  HAL_FreeMatchInfo(&dataBack);
}

}  // namespace hal
