// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/driverstation/DriverStation.hpp"

#include <string>
#include <tuple>

#include <gtest/gtest.h>

#include "wpi/driverstation/Joystick.hpp"
#include "wpi/simulation/DriverStationSim.hpp"
#include "wpi/simulation/SimHooks.hpp"

class IsJoystickConnectedParametersTest
    : public ::testing::TestWithParam<std::tuple<int, int, int, bool>> {};

TEST_P(IsJoystickConnectedParametersTest, IsJoystickConnected) {
  wpi::sim::DriverStationSim::SetJoystickAxesMaximumIndex(
      1, std::get<0>(GetParam()));
  wpi::sim::DriverStationSim::SetJoystickButtonsMaximumIndex(
      1, std::get<1>(GetParam()));
  wpi::sim::DriverStationSim::SetJoystickPOVsMaximumIndex(
      1, std::get<2>(GetParam()));
  wpi::sim::DriverStationSim::NotifyNewData();

  ASSERT_EQ(std::get<3>(GetParam()),
            wpi::DriverStation::IsJoystickConnected(1));
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
  wpi::sim::DriverStationSim::SetFmsAttached(std::get<0>(GetParam()));
  wpi::sim::DriverStationSim::NotifyNewData();
  wpi::DriverStation::SilenceJoystickConnectionWarning(std::get<1>(GetParam()));

  // Create joystick and attempt to retrieve button.
  wpi::Joystick joystick(0);
  joystick.GetRawButton(1);

  wpi::sim::StepTiming(1_s);
  EXPECT_EQ(wpi::DriverStation::IsJoystickConnectionWarningSilenced(),
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
            "Warning: Joystick Button 1 missing (available 0), check if all "
            "controllers are plugged in\n"),
        std::make_tuple(
            true, true, false,
            "Warning: Joystick Button 1 missing (available 0), check if all "
            "controllers are plugged in\n"),
        std::make_tuple(
            true, false, false,
            "Warning: Joystick Button 1 missing (available 0), check if all "
            "controllers are plugged in\n")));
