// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <string>
#include <tuple>

#include "frc/DriverStation.h"
#include "frc/Joystick.h"
#include "frc/simulation/DriverStationSim.h"
#include "frc/simulation/SimHooks.h"
#include "gtest/gtest.h"

class IsJoystickConnectedParametersTest
    : public ::testing::TestWithParam<std::tuple<int, int, int, bool>> {};

TEST_P(IsJoystickConnectedParametersTest, IsJoystickConnected) {
  frc::sim::DriverStationSim::SetJoystickAxisCount(1, std::get<0>(GetParam()));
  frc::sim::DriverStationSim::SetJoystickButtonCount(1,
                                                     std::get<1>(GetParam()));
  frc::sim::DriverStationSim::SetJoystickPOVCount(1, std::get<2>(GetParam()));
  frc::sim::DriverStationSim::NotifyNewData();

  ASSERT_EQ(std::get<3>(GetParam()),
            frc::DriverStation::IsJoystickConnected(1));
}

INSTANTIATE_TEST_SUITE_P(IsConnectedTests, IsJoystickConnectedParametersTest,
                         ::testing::Values(std::make_tuple(0, 0, 0, false),
                                           std::make_tuple(1, 0, 0, true),
                                           std::make_tuple(0, 1, 0, true),
                                           std::make_tuple(0, 0, 1, true),
                                           std::make_tuple(1, 1, 1, true),
                                           std::make_tuple(4, 10, 1, true)));
class JoystickConnectionWarningTest
    : public ::testing::TestWithParam<
          std::tuple<bool, bool, bool, std::string>> {};

TEST_P(JoystickConnectionWarningTest, JoystickConnectionWarnings) {
  // Capture all output to stderr.
  ::testing::internal::CaptureStderr();

  // Set FMS and Silence settings
  frc::sim::DriverStationSim::SetFmsAttached(std::get<0>(GetParam()));
  frc::sim::DriverStationSim::NotifyNewData();
  frc::DriverStation::SilenceJoystickConnectionWarning(std::get<1>(GetParam()));

  // Create joystick and attempt to retrieve button.
  frc::Joystick joystick(0);
  joystick.GetRawButton(1);

  frc::sim::StepTiming(1_s);
  EXPECT_EQ(frc::DriverStation::IsJoystickConnectionWarningSilenced(),
            std::get<2>(GetParam()));
  EXPECT_EQ(::testing::internal::GetCapturedStderr().substr(
                0, std::get<3>(GetParam()).size()),
            std::get<3>(GetParam()));
}

INSTANTIATE_TEST_SUITE_P(
    DriverStationTests, JoystickConnectionWarningTest,
    ::testing::Values(
        std::make_tuple(false, true, true, ""),
        std::make_tuple(
            false, false, false,
            "Warning: Joystick Button 1 missing (max 0), check if all "
            "controllers are plugged in\n"),
        std::make_tuple(
            true, true, false,
            "Warning: Joystick Button 1 missing (max 0), check if all "
            "controllers are plugged in\n"),
        std::make_tuple(
            true, false, false,
            "Warning: Joystick Button 1 missing (max 0), check if all "
            "controllers are plugged in\n")));
