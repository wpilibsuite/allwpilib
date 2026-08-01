// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <string>
#include <string_view>
#include <tuple>

#include <gtest/gtest.h>

#include "wpi/driverstation/Joystick.hpp"
#include "wpi/driverstation/internal/DriverStationBackend.hpp"
#include "wpi/hal/simulation/DriverStationData.h"
#include "wpi/simulation/AlertSim.hpp"
#include "wpi/simulation/DriverStationSim.hpp"
#include "wpi/simulation/SimHooks.hpp"
#include "wpi/util/Alert.hpp"

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
            wpi::internal::DriverStationBackend::IsJoystickConnected(1));
}

INSTANTIATE_TEST_SUITE_P(IsConnectedTests, IsJoystickConnectedParametersTest,
                         ::testing::Values(std::make_tuple(0, 0, 0, false),
                                           std::make_tuple(1, 0, 0, true),
                                           std::make_tuple(0, 1, 0, true),
                                           std::make_tuple(0, 0, 1, true),
                                           std::make_tuple(1, 1, 1, true),
                                           std::make_tuple(4, 10, 1, true)));
class JoystickConnectionAlertTest
    : public ::testing::TestWithParam<std::tuple<bool, bool, bool, bool>> {};

static void ResetJoystickAlerts() {
  HALSIM_SetJoystickButtonsAvailable(0, 0);
  HALSIM_SetJoystickAxesAvailable(0, 0);
  HALSIM_SetJoystickPOVsAvailable(0, 0);
  wpi::sim::DriverStationSim::SetFmsAttached(false);
  wpi::sim::DriverStationSim::NotifyNewData();
  wpi::internal::DriverStationBackend::SilenceJoystickConnectionAlert(true);
  wpi::sim::AlertSim::ResetData();
  wpi::internal::DriverStationBackend::SilenceJoystickConnectionAlert(false);
}

static bool IsDriverStationAlertActive(std::string_view id,
                                       std::string_view text,
                                       wpi::util::Alert::Level level) {
  for (const auto& alert : wpi::sim::AlertSim::GetActive()) {
    if (alert.group == "DriverStation" && alert.id == id &&
        alert.text == text && alert.level == level) {
      return true;
    }
  }

  return false;
}

static bool IsJoystickDisconnectedAlertActive(int stick) {
  std::string expectedText =
      "Joystick on port " + std::to_string(stick) +
      " not available, check if all controllers are plugged in";

  return IsDriverStationAlertActive(
      "joystick" + std::to_string(stick) + "Disconnected", expectedText,
      wpi::util::Alert::Level::HIGH);
}

TEST_P(JoystickConnectionAlertTest, JoystickConnectionAlerts) {
  ResetJoystickAlerts();

  ::testing::internal::CaptureStderr();

  // Set FMS and Silence settings
  wpi::sim::DriverStationSim::SetFmsAttached(std::get<0>(GetParam()));
  wpi::sim::DriverStationSim::NotifyNewData();
  wpi::internal::DriverStationBackend::SilenceJoystickConnectionAlert(
      std::get<1>(GetParam()));

  // Create joystick and attempt to retrieve button.
  wpi::Joystick joystick(0);
  joystick.GetHID().GetRawButton(1);

  wpi::sim::StepTiming(1_s);
  EXPECT_EQ(
      wpi::internal::DriverStationBackend::IsJoystickConnectionAlertSilenced(),
      std::get<2>(GetParam()));
  EXPECT_EQ(IsJoystickDisconnectedAlertActive(0), std::get<3>(GetParam()));
  EXPECT_EQ(::testing::internal::GetCapturedStderr(), "");

  ResetJoystickAlerts();
}

TEST(DriverStationTest, JoystickResourceAlerts) {
  ResetJoystickAlerts();

  HALSIM_SetJoystickButtonsAvailable(0, 1);
  HALSIM_SetJoystickAxesAvailable(0, 1);
  HALSIM_SetJoystickPOVsAvailable(0, 1);
  uint8_t fingerCounts[HAL_MAX_JOYSTICK_TOUCHPADS] = {1, 0};
  HALSIM_SetJoystickTouchpadCounts(0, 1, fingerCounts);
  wpi::sim::DriverStationSim::NotifyNewData();

  ::testing::internal::CaptureStderr();

  wpi::internal::DriverStationBackend::GetStickButton(0, 1);
  wpi::internal::DriverStationBackend::GetStickButton(0, 2);
  wpi::internal::DriverStationBackend::GetStickAxis(0, 1);
  wpi::internal::DriverStationBackend::GetStickPOV(0, 1);
  wpi::internal::DriverStationBackend::GetStickTouchpadFinger(0, 0, 1);

  EXPECT_TRUE(IsDriverStationAlertActive(
      "joystick0ButtonUnavailable", "Joystick Button 2 on port 0 not available",
      wpi::util::Alert::Level::MEDIUM));
  EXPECT_TRUE(IsDriverStationAlertActive(
      "joystick0AxisUnavailable", "Joystick axis 1 on port 0 not available",
      wpi::util::Alert::Level::MEDIUM));
  EXPECT_TRUE(IsDriverStationAlertActive("joystick0POVUnavailable",
                                         "Joystick POV 1 on port 0 not "
                                         "available",
                                         wpi::util::Alert::Level::MEDIUM));
  EXPECT_TRUE(IsDriverStationAlertActive(
      "joystick0TouchpadFingerUnavailable",
      "Joystick touchpad finger 1 on touchpad 0 on port 0 not available",
      wpi::util::Alert::Level::MEDIUM));
  EXPECT_FALSE(IsJoystickDisconnectedAlertActive(0));
  EXPECT_EQ(::testing::internal::GetCapturedStderr(), "");

  ResetJoystickAlerts();
}

TEST(DriverStationTest, JoystickAlertCollisionDoesNotCommitPartialAlerts) {
  ResetJoystickAlerts();

  HALSIM_SetJoystickButtonsAvailable(0, 1);
  HALSIM_SetJoystickAxesAvailable(0, 1);
  HALSIM_SetJoystickPOVsAvailable(0, 1);
  wpi::sim::DriverStationSim::NotifyNewData();

  {
    wpi::util::Alert collision{"DriverStation", "joystick0AxisUnavailable",
                               "collision", wpi::util::Alert::Level::MEDIUM};
    ASSERT_TRUE(collision);

    ::testing::internal::CaptureStderr();

    EXPECT_FALSE(wpi::internal::DriverStationBackend::GetStickButton(0, 2));
    EXPECT_EQ(::testing::internal::GetCapturedStderr(), "");

    auto alerts = wpi::sim::AlertSim::GetAll();
    ASSERT_EQ(alerts.size(), 1u);
    EXPECT_EQ(alerts[0].group, "DriverStation");
    EXPECT_EQ(alerts[0].id, "joystick0AxisUnavailable");
    EXPECT_EQ(alerts[0].text, "collision");
    EXPECT_EQ(alerts[0].level, wpi::util::Alert::Level::MEDIUM);
    EXPECT_FALSE(alerts[0].isActive());

    EXPECT_EQ(wpi::internal::DriverStationBackend::GetStickAxis(0, 1), 0.0);
    EXPECT_EQ(wpi::sim::AlertSim::GetAll().size(), 1u);
  }

  EXPECT_FALSE(wpi::internal::DriverStationBackend::GetStickButton(0, 2));
  EXPECT_TRUE(IsDriverStationAlertActive(
      "joystick0ButtonUnavailable", "Joystick Button 2 on port 0 not available",
      wpi::util::Alert::Level::MEDIUM));

  ResetJoystickAlerts();
}

INSTANTIATE_TEST_SUITE_P(
    DriverStationTests, JoystickConnectionAlertTest,
    ::testing::Values(std::make_tuple(false, true, true, false),
                      std::make_tuple(false, false, false, true),
                      std::make_tuple(true, true, false, true),
                      std::make_tuple(true, false, false, true)));
