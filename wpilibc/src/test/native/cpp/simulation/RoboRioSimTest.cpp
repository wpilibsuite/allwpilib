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

TEST(RoboRioSimTest, SetVin) {
  RoboRioSim::ResetData();

  DoubleCallback voltageCallback;
  auto voltageCb = RoboRioSim::RegisterVInVoltageCallback(
      voltageCallback.GetCallback(), false);
  constexpr double TEST_VOLTAGE = 1.91;

  RoboRioSim::SetVInVoltage(units::volt_t{TEST_VOLTAGE});
  EXPECT_TRUE(voltageCallback.WasTriggered());
  EXPECT_EQ(TEST_VOLTAGE, voltageCallback.GetLastValue());
  EXPECT_EQ(TEST_VOLTAGE, RoboRioSim::GetVInVoltage().value());
  EXPECT_EQ(TEST_VOLTAGE, RobotController::GetInputVoltage());
}

TEST(RoboRioSimTest, SetBrownout) {
  RoboRioSim::ResetData();

  DoubleCallback voltageCallback;
  auto voltageCb = RoboRioSim::RegisterBrownoutVoltageCallback(
      voltageCallback.GetCallback(), false);
  constexpr double TEST_VOLTAGE = 1.91;

  RoboRioSim::SetBrownoutVoltage(units::volt_t{TEST_VOLTAGE});
  EXPECT_TRUE(voltageCallback.WasTriggered());
  EXPECT_EQ(TEST_VOLTAGE, voltageCallback.GetLastValue());
  EXPECT_EQ(TEST_VOLTAGE, RoboRioSim::GetBrownoutVoltage().value());
  EXPECT_EQ(TEST_VOLTAGE, RobotController::GetBrownoutVoltage().value());
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
  constexpr double TEST_VOLTAGE = 22.9;
  constexpr double TEST_CURRENT = 174;
  constexpr int TEST_FAULTS = 229;

  RoboRioSim::SetUserVoltage3V3(units::volt_t{TEST_VOLTAGE});
  EXPECT_TRUE(voltageCallback.WasTriggered());
  EXPECT_EQ(TEST_VOLTAGE, voltageCallback.GetLastValue());
  EXPECT_EQ(TEST_VOLTAGE, RoboRioSim::GetUserVoltage3V3().value());
  EXPECT_EQ(TEST_VOLTAGE, RobotController::GetVoltage3V3());

  RoboRioSim::SetUserCurrent3V3(units::ampere_t{TEST_CURRENT});
  EXPECT_TRUE(currentCallback.WasTriggered());
  EXPECT_EQ(TEST_CURRENT, currentCallback.GetLastValue());
  EXPECT_EQ(TEST_CURRENT, RoboRioSim::GetUserCurrent3V3().value());
  EXPECT_EQ(TEST_CURRENT, RobotController::GetCurrent3V3());

  RoboRioSim::SetUserActive3V3(false);
  EXPECT_TRUE(activeCallback.WasTriggered());
  EXPECT_FALSE(activeCallback.GetLastValue());
  EXPECT_FALSE(RoboRioSim::GetUserActive3V3());
  EXPECT_FALSE(RobotController::GetEnabled3V3());

  RoboRioSim::SetUserFaults3V3(TEST_FAULTS);
  EXPECT_TRUE(faultCallback.WasTriggered());
  EXPECT_EQ(TEST_FAULTS, faultCallback.GetLastValue());
  EXPECT_EQ(TEST_FAULTS, RoboRioSim::GetUserFaults3V3());
  EXPECT_EQ(TEST_FAULTS, RobotController::GetFaultCount3V3());
}

TEST(RoboRioSimTest, SetCPUTemp) {
  RoboRioSim::ResetData();

  DoubleCallback callback;
  auto cbHandle =
      RoboRioSim::RegisterCPUTempCallback(callback.GetCallback(), false);
  constexpr double CPU_TEMP = 100.0;

  RoboRioSim::SetCPUTemp(units::celsius_t{CPU_TEMP});
  EXPECT_TRUE(callback.WasTriggered());
  EXPECT_EQ(CPU_TEMP, callback.GetLastValue());
  EXPECT_EQ(CPU_TEMP, RoboRioSim::GetCPUTemp().value());
  EXPECT_EQ(CPU_TEMP, RobotController::GetCPUTemp().value());
}

TEST(RoboRioSimTest, SetTeamNumber) {
  RoboRioSim::ResetData();

  IntCallback callback;
  auto cbHandle =
      RoboRioSim::RegisterTeamNumberCallback(callback.GetCallback(), false);
  constexpr int TEAM_NUMBER = 9999;

  RoboRioSim::SetTeamNumber(TEAM_NUMBER);
  EXPECT_TRUE(callback.WasTriggered());
  EXPECT_EQ(TEAM_NUMBER, callback.GetLastValue());
  EXPECT_EQ(TEAM_NUMBER, RoboRioSim::GetTeamNumber());
  EXPECT_EQ(TEAM_NUMBER, RobotController::GetTeamNumber());
}

TEST(RoboRioSimTest, SetSerialNumber) {
  const std::string SERIAL_NUM = "Hello";

  RoboRioSim::ResetData();

  RoboRioSim::SetSerialNumber(SERIAL_NUM);
  EXPECT_EQ(SERIAL_NUM, RoboRioSim::GetSerialNumber());
  EXPECT_EQ(SERIAL_NUM, RobotController::GetSerialNumber());

  const std::string SERIAL_NUMBER_OVERFLOW = "SerialNumber";
  const std::string SERIAL_NUMBER_TRUNCATED =
      SERIAL_NUMBER_OVERFLOW.substr(0, 8);

  RoboRioSim::SetSerialNumber(SERIAL_NUMBER_OVERFLOW);
  EXPECT_EQ(SERIAL_NUMBER_TRUNCATED, RoboRioSim::GetSerialNumber());
  EXPECT_EQ(SERIAL_NUMBER_TRUNCATED, RobotController::GetSerialNumber());
}

TEST(RoboRioSimTest, SetComments) {
  const std::string COMMENTS =
      "Hello! These are comments in the roboRIO web interface!";

  RoboRioSim::ResetData();

  RoboRioSim::SetComments(COMMENTS);
  EXPECT_EQ(COMMENTS, RoboRioSim::GetComments());
  EXPECT_EQ(COMMENTS, RobotController::GetComments());

  const std::string COMMENTS_OVERFLOW =
      "Hello! These are comments in the roboRIO web interface! This comment "
      "exceeds 64 characters!";
  const std::string COMMENTS_TRUNCATED = COMMENTS_OVERFLOW.substr(0, 64);

  RoboRioSim::SetComments(COMMENTS_OVERFLOW);
  EXPECT_EQ(COMMENTS_TRUNCATED, RoboRioSim::GetComments());
  EXPECT_EQ(COMMENTS_TRUNCATED, RobotController::GetComments());
}

}  // namespace frc::sim
