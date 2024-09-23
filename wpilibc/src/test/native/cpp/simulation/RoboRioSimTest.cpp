// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/simulation/RoboRioSim.h"  // NOLINT(build/include_order)

#include <string>

#include <gtest/gtest.h>
#include <hal/HAL.h>
#include <hal/HALBase.h>

#include "callback_helpers/TestCallbackHelpers.h"
#include "frc/RobotController.h"

namespace frc::sim {

TEST(RoboRioSimTest, FPGAButton) {
  RoboRioSim::ResetData();

  int dummyStatus = 0;

  BooleanCallback callback;
  auto cb =
      RoboRioSim::RegisterFPGAButtonCallback(callback.GetCallback(), false);
  RoboRioSim::SetFPGAButton(true);
  EXPECT_TRUE(RoboRioSim::GetFPGAButton());
  EXPECT_TRUE(HAL_GetFPGAButton(&dummyStatus));
  EXPECT_TRUE(callback.WasTriggered());
  EXPECT_TRUE(callback.GetLastValue());

  callback.Reset();
  RoboRioSim::SetFPGAButton(false);
  EXPECT_FALSE(RoboRioSim::GetFPGAButton());
  EXPECT_FALSE(HAL_GetFPGAButton(&dummyStatus));
  EXPECT_TRUE(callback.WasTriggered());
  EXPECT_FALSE(callback.GetLastValue());
}

TEST(RoboRioSimTest, SetVin) {
  RoboRioSim::ResetData();

  DoubleCallback voltageCallback;
  DoubleCallback currentCallback;
  auto voltageCb = RoboRioSim::RegisterVInVoltageCallback(
      voltageCallback.GetCallback(), false);
  auto currentCb = RoboRioSim::RegisterVInCurrentCallback(
      currentCallback.GetCallback(), false);
  constexpr double kTestVoltage = 1.91;
  constexpr double kTestCurrent = 35.04;

  RoboRioSim::SetVInVoltage(units::volt_t{kTestVoltage});
  EXPECT_TRUE(voltageCallback.WasTriggered());
  EXPECT_EQ(kTestVoltage, voltageCallback.GetLastValue());
  EXPECT_EQ(kTestVoltage, RoboRioSim::GetVInVoltage().value());
  EXPECT_EQ(kTestVoltage, RobotController::GetInputVoltage());

  RoboRioSim::SetVInCurrent(units::ampere_t{kTestCurrent});
  EXPECT_TRUE(currentCallback.WasTriggered());
  EXPECT_EQ(kTestCurrent, currentCallback.GetLastValue());
  EXPECT_EQ(kTestCurrent, RoboRioSim::GetVInCurrent().value());
  EXPECT_EQ(kTestCurrent, RobotController::GetInputCurrent());
}

TEST(RoboRioSimTest, SetBrownout) {
  RoboRioSim::ResetData();

  DoubleCallback voltageCallback;
  auto voltageCb = RoboRioSim::RegisterBrownoutVoltageCallback(
      voltageCallback.GetCallback(), false);
  constexpr double kTestVoltage = 1.91;

  RoboRioSim::SetBrownoutVoltage(units::volt_t{kTestVoltage});
  EXPECT_TRUE(voltageCallback.WasTriggered());
  EXPECT_EQ(kTestVoltage, voltageCallback.GetLastValue());
  EXPECT_EQ(kTestVoltage, RoboRioSim::GetBrownoutVoltage().value());
  EXPECT_EQ(kTestVoltage, RobotController::GetBrownoutVoltage().value());
}

TEST(RoboRioSimTest, Set6V) {
  RoboRioSim::ResetData();

  DoubleCallback voltageCallback;
  DoubleCallback currentCallback;
  BooleanCallback activeCallback;
  IntCallback faultCallback;
  auto voltageCb = RoboRioSim::RegisterUserVoltage6VCallback(
      voltageCallback.GetCallback(), false);
  auto currentCb = RoboRioSim::RegisterUserCurrent6VCallback(
      currentCallback.GetCallback(), false);
  auto activeCb = RoboRioSim::RegisterUserActive6VCallback(
      activeCallback.GetCallback(), false);
  auto faultsCb = RoboRioSim::RegisterUserFaults6VCallback(
      faultCallback.GetCallback(), false);
  constexpr double kTestVoltage = 22.9;
  constexpr double kTestCurrent = 174;
  constexpr int kTestFaults = 229;

  RoboRioSim::SetUserVoltage6V(units::volt_t{kTestVoltage});
  EXPECT_TRUE(voltageCallback.WasTriggered());
  EXPECT_EQ(kTestVoltage, voltageCallback.GetLastValue());
  EXPECT_EQ(kTestVoltage, RoboRioSim::GetUserVoltage6V().value());
  EXPECT_EQ(kTestVoltage, RobotController::GetVoltage6V());

  RoboRioSim::SetUserCurrent6V(units::ampere_t{kTestCurrent});
  EXPECT_TRUE(currentCallback.WasTriggered());
  EXPECT_EQ(kTestCurrent, currentCallback.GetLastValue());
  EXPECT_EQ(kTestCurrent, RoboRioSim::GetUserCurrent6V().value());
  EXPECT_EQ(kTestCurrent, RobotController::GetCurrent6V());

  RoboRioSim::SetUserActive6V(false);
  EXPECT_TRUE(activeCallback.WasTriggered());
  EXPECT_FALSE(activeCallback.GetLastValue());
  EXPECT_FALSE(RoboRioSim::GetUserActive6V());
  EXPECT_FALSE(RobotController::GetEnabled6V());

  RoboRioSim::SetUserFaults6V(kTestFaults);
  EXPECT_TRUE(faultCallback.WasTriggered());
  EXPECT_EQ(kTestFaults, faultCallback.GetLastValue());
  EXPECT_EQ(kTestFaults, RoboRioSim::GetUserFaults6V());
  EXPECT_EQ(kTestFaults, RobotController::GetFaultCount6V());
}

TEST(RoboRioSimTest, Set5V) {
  RoboRioSim::ResetData();

  DoubleCallback voltageCallback;
  DoubleCallback currentCallback;
  BooleanCallback activeCallback;
  IntCallback faultCallback;
  auto voltageCb = RoboRioSim::RegisterUserVoltage5VCallback(
      voltageCallback.GetCallback(), false);
  auto currentCb = RoboRioSim::RegisterUserCurrent5VCallback(
      currentCallback.GetCallback(), false);
  auto activeCb = RoboRioSim::RegisterUserActive5VCallback(
      activeCallback.GetCallback(), false);
  auto faultsCb = RoboRioSim::RegisterUserFaults5VCallback(
      faultCallback.GetCallback(), false);
  constexpr double kTestVoltage = 22.9;
  constexpr double kTestCurrent = 174;
  constexpr int kTestFaults = 229;

  RoboRioSim::SetUserVoltage5V(units::volt_t{kTestVoltage});
  EXPECT_TRUE(voltageCallback.WasTriggered());
  EXPECT_EQ(kTestVoltage, voltageCallback.GetLastValue());
  EXPECT_EQ(kTestVoltage, RoboRioSim::GetUserVoltage5V().value());
  EXPECT_EQ(kTestVoltage, RobotController::GetVoltage5V());

  RoboRioSim::SetUserCurrent5V(units::ampere_t{kTestCurrent});
  EXPECT_TRUE(currentCallback.WasTriggered());
  EXPECT_EQ(kTestCurrent, currentCallback.GetLastValue());
  EXPECT_EQ(kTestCurrent, RoboRioSim::GetUserCurrent5V().value());
  EXPECT_EQ(kTestCurrent, RobotController::GetCurrent5V());

  RoboRioSim::SetUserActive5V(false);
  EXPECT_TRUE(activeCallback.WasTriggered());
  EXPECT_FALSE(activeCallback.GetLastValue());
  EXPECT_FALSE(RoboRioSim::GetUserActive5V());
  EXPECT_FALSE(RobotController::GetEnabled5V());

  RoboRioSim::SetUserFaults5V(kTestFaults);
  EXPECT_TRUE(faultCallback.WasTriggered());
  EXPECT_EQ(kTestFaults, faultCallback.GetLastValue());
  EXPECT_EQ(kTestFaults, RoboRioSim::GetUserFaults5V());
  EXPECT_EQ(kTestFaults, RobotController::GetFaultCount5V());
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

  RoboRioSim::SetUserVoltage3V3(units::volt_t{kTestVoltage});
  EXPECT_TRUE(voltageCallback.WasTriggered());
  EXPECT_EQ(kTestVoltage, voltageCallback.GetLastValue());
  EXPECT_EQ(kTestVoltage, RoboRioSim::GetUserVoltage3V3().value());
  EXPECT_EQ(kTestVoltage, RobotController::GetVoltage3V3());

  RoboRioSim::SetUserCurrent3V3(units::ampere_t{kTestCurrent});
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

  RoboRioSim::SetCPUTemp(units::celsius_t{kCPUTemp});
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

TEST(RoboRioSimTest, SetRadioLEDState) {
  RoboRioSim::ResetData();

  EnumCallback callback;
  auto cbHandle =
      RoboRioSim::RegisterRadioLEDStateCallback(callback.GetCallback(), false);

  RobotController::SetRadioLEDState(RadioLEDState::kGreen);
  EXPECT_TRUE(callback.WasTriggered());
  EXPECT_EQ(RadioLEDState::kGreen, callback.GetLastValue());
  EXPECT_EQ(RadioLEDState::kGreen, RoboRioSim::GetRadioLEDState());
  EXPECT_EQ(RadioLEDState::kGreen, RobotController::GetRadioLEDState());

  callback.Reset();

  RoboRioSim::SetRadioLEDState(RadioLEDState::kOrange);
  EXPECT_TRUE(callback.WasTriggered());
  EXPECT_EQ(RadioLEDState::kOrange, callback.GetLastValue());
  EXPECT_EQ(RadioLEDState::kOrange, RoboRioSim::GetRadioLEDState());
  EXPECT_EQ(RadioLEDState::kOrange, RobotController::GetRadioLEDState());
}

}  // namespace frc::sim
