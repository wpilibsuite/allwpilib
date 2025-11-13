// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/simulation/RoboRioSim.hpp"

#include <string>

#include <gtest/gtest.h>

#include "callback_helpers/TestCallbackHelpers.hpp"
#include "wpi/hal/HAL.h"
#include "wpi/hal/HALBase.h"
#include "wpi/system/RobotController.hpp"

namespace wpi::sim {

TEST(RoboRioSimTest, SetVin) {
  RoboRioSim::ResetData();

  DoubleCallback voltageCallback;
  auto voltageCb = RoboRioSim::RegisterVInVoltageCallback(
      voltageCallback.GetCallback(), false);
  constexpr double kTestVoltage = 1.91;

  RoboRioSim::SetVInVoltage(wpi::units::volt_t{kTestVoltage});
  EXPECT_TRUE(voltageCallback.WasTriggered());
  EXPECT_EQ(kTestVoltage, voltageCallback.GetLastValue());
  EXPECT_EQ(kTestVoltage, RoboRioSim::GetVInVoltage().value());
  EXPECT_EQ(kTestVoltage, RobotController::GetInputVoltage());
}

TEST(RoboRioSimTest, SetBrownout) {
  RoboRioSim::ResetData();

  DoubleCallback voltageCallback;
  auto voltageCb = RoboRioSim::RegisterBrownoutVoltageCallback(
      voltageCallback.GetCallback(), false);
  constexpr double kTestVoltage = 1.91;

  RoboRioSim::SetBrownoutVoltage(wpi::units::volt_t{kTestVoltage});
  EXPECT_TRUE(voltageCallback.WasTriggered());
  EXPECT_EQ(kTestVoltage, voltageCallback.GetLastValue());
  EXPECT_EQ(kTestVoltage, RoboRioSim::GetBrownoutVoltage().value());
  EXPECT_EQ(kTestVoltage, RobotController::GetBrownoutVoltage().value());
}

TEST(RoboRioSimTest, Set3V3) {
  RoboRioSim::ResetData();

  DoubleCallback voltageCallback;
  DoubleCallback currentCallback;
  BooleanCallback activeCallback;
  IntCallback faultCallback;
  auto voltageCb = RoboRioSim::RegisterUserVoltage3V3Callback(
      voltageCallback.GetCallback(), false);
  auto currentCb = RoboRioSim::RegisterUserCurrent3V3Callback(
      currentCallback.GetCallback(), false);
  auto activeCb = RoboRioSim::RegisterUserActive3V3Callback(
      activeCallback.GetCallback(), false);
  auto faultsCb = RoboRioSim::RegisterUserFaults3V3Callback(
      faultCallback.GetCallback(), false);
  constexpr double kTestVoltage = 22.9;
  constexpr double kTestCurrent = 174;
  constexpr int kTestFaults = 229;

  RoboRioSim::SetUserVoltage3V3(wpi::units::volt_t{kTestVoltage});
  EXPECT_TRUE(voltageCallback.WasTriggered());
  EXPECT_EQ(kTestVoltage, voltageCallback.GetLastValue());
  EXPECT_EQ(kTestVoltage, RoboRioSim::GetUserVoltage3V3().value());
  EXPECT_EQ(kTestVoltage, RobotController::GetVoltage3V3());

  RoboRioSim::SetUserCurrent3V3(wpi::units::ampere_t{kTestCurrent});
  EXPECT_TRUE(currentCallback.WasTriggered());
  EXPECT_EQ(kTestCurrent, currentCallback.GetLastValue());
  EXPECT_EQ(kTestCurrent, RoboRioSim::GetUserCurrent3V3().value());
  EXPECT_EQ(kTestCurrent, RobotController::GetCurrent3V3());

  RoboRioSim::SetUserActive3V3(false);
  EXPECT_TRUE(activeCallback.WasTriggered());
  EXPECT_FALSE(activeCallback.GetLastValue());
  EXPECT_FALSE(RoboRioSim::GetUserActive3V3());
  EXPECT_FALSE(RobotController::GetEnabled3V3());

  RoboRioSim::SetUserFaults3V3(kTestFaults);
  EXPECT_TRUE(faultCallback.WasTriggered());
  EXPECT_EQ(kTestFaults, faultCallback.GetLastValue());
  EXPECT_EQ(kTestFaults, RoboRioSim::GetUserFaults3V3());
  EXPECT_EQ(kTestFaults, RobotController::GetFaultCount3V3());
}

TEST(RoboRioSimTest, SetCPUTemp) {
  RoboRioSim::ResetData();

  DoubleCallback callback;
  auto cbHandle =
      RoboRioSim::RegisterCPUTempCallback(callback.GetCallback(), false);
  constexpr double kCPUTemp = 100.0;

  RoboRioSim::SetCPUTemp(wpi::units::celsius_t{kCPUTemp});
  EXPECT_TRUE(callback.WasTriggered());
  EXPECT_EQ(kCPUTemp, callback.GetLastValue());
  EXPECT_EQ(kCPUTemp, RoboRioSim::GetCPUTemp().value());
  EXPECT_EQ(kCPUTemp, RobotController::GetCPUTemp().value());
}

TEST(RoboRioSimTest, SetTeamNumber) {
  RoboRioSim::ResetData();

  IntCallback callback;
  auto cbHandle =
      RoboRioSim::RegisterTeamNumberCallback(callback.GetCallback(), false);
  constexpr int kTeamNumber = 9999;

  RoboRioSim::SetTeamNumber(kTeamNumber);
  EXPECT_TRUE(callback.WasTriggered());
  EXPECT_EQ(kTeamNumber, callback.GetLastValue());
  EXPECT_EQ(kTeamNumber, RoboRioSim::GetTeamNumber());
  EXPECT_EQ(kTeamNumber, RobotController::GetTeamNumber());
}

TEST(RoboRioSimTest, SetSerialNumber) {
  const std::string kSerialNum = "Hello";

  RoboRioSim::ResetData();

  RoboRioSim::SetSerialNumber(kSerialNum);
  EXPECT_EQ(kSerialNum, RoboRioSim::GetSerialNumber());
  EXPECT_EQ(kSerialNum, RobotController::GetSerialNumber());

  const std::string kSerialNumberOverflow = "SerialNumber";
  const std::string kSerialNumberTruncated = kSerialNumberOverflow.substr(0, 8);

  RoboRioSim::SetSerialNumber(kSerialNumberOverflow);
  EXPECT_EQ(kSerialNumberTruncated, RoboRioSim::GetSerialNumber());
  EXPECT_EQ(kSerialNumberTruncated, RobotController::GetSerialNumber());
}

TEST(RoboRioSimTest, SetComments) {
  const std::string kComments =
      "Hello! These are comments in the roboRIO web interface!";

  RoboRioSim::ResetData();

  RoboRioSim::SetComments(kComments);
  EXPECT_EQ(kComments, RoboRioSim::GetComments());
  EXPECT_EQ(kComments, RobotController::GetComments());

  const std::string kCommentsOverflow =
      "Hello! These are comments in the roboRIO web interface! This comment "
      "exceeds 64 characters!";
  const std::string kCommentsTruncated = kCommentsOverflow.substr(0, 64);

  RoboRioSim::SetComments(kCommentsOverflow);
  EXPECT_EQ(kCommentsTruncated, RoboRioSim::GetComments());
  EXPECT_EQ(kCommentsTruncated, RobotController::GetComments());
}

}  // namespace wpi::sim
