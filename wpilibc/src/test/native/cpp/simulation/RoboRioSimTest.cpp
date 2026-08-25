// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/simulation/RoboRioSim.hpp"

#include <string>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "callback_helpers/TestCallbackHelpers.hpp"
#include "wpi/hal/Errors.h"
#include "wpi/hal/Power.h"
#include "wpi/system/RobotController.hpp"

namespace wpi::sim {

TEST_CASE("RoboRioSimTest SetVin", "[wpilibc][simulation]") {
  RoboRioSim::ResetData();

  DoubleCallback voltageCallback;
  auto voltageCb = RoboRioSim::RegisterVInVoltageCallback(
      voltageCallback.GetCallback(), false);
  constexpr double TEST_VOLTAGE = 1.91;

  RoboRioSim::SetVInVoltage(wpi::units::volts<>{TEST_VOLTAGE});
  CHECK(voltageCallback.WasTriggered());
  CHECK(TEST_VOLTAGE == voltageCallback.GetLastValue());
  CHECK(TEST_VOLTAGE == RoboRioSim::GetVInVoltage().value());
  CHECK(TEST_VOLTAGE == RobotController::GetInputVoltage());
}

TEST_CASE("RoboRioSimTest SetBrownout", "[wpilibc][simulation]") {
  RoboRioSim::ResetData();

  DoubleCallback brownoutVoltageCallback;
  DoubleCallback recoveryVoltageCallback;
  auto brownoutVoltageCb = RoboRioSim::RegisterBrownoutVoltageCallback(
      brownoutVoltageCallback.GetCallback(), false);
  auto recoveryVoltageCb = RoboRioSim::RegisterBrownoutRecoveryVoltageCallback(
      recoveryVoltageCallback.GetCallback(), false);
  double recoveryVoltageDuringBrownoutCallback = 0.0;
  double brownoutVoltageDuringRecoveryCallback = 0.0;
  auto brownoutConsistencyCb = RoboRioSim::RegisterBrownoutVoltageCallback(
      [&](std::string_view, const HAL_Value*) {
        recoveryVoltageDuringBrownoutCallback =
            RoboRioSim::GetBrownoutRecoveryVoltage().value();
      },
      false);
  auto recoveryConsistencyCb =
      RoboRioSim::RegisterBrownoutRecoveryVoltageCallback(
          [&](std::string_view, const HAL_Value*) {
            brownoutVoltageDuringRecoveryCallback =
                RoboRioSim::GetBrownoutVoltage().value();
          },
          false);
  constexpr double REQUESTED_BROWNOUT_VOLTAGE = 7.5004;
  constexpr double REQUESTED_RECOVERY_VOLTAGE = 8.0003;
  constexpr double EXPECTED_BROWNOUT_VOLTAGE = 7.5;
  constexpr double EXPECTED_RECOVERY_VOLTAGE = 8.0;

  RobotController::SetBrownoutVoltages(
      wpi::units::volts<>{REQUESTED_BROWNOUT_VOLTAGE},
      wpi::units::volts<>{REQUESTED_RECOVERY_VOLTAGE});
  CHECK(brownoutVoltageCallback.WasTriggered());
  CHECK(recoveryVoltageCallback.WasTriggered());
  CHECK(EXPECTED_BROWNOUT_VOLTAGE == brownoutVoltageCallback.GetLastValue());
  CHECK(EXPECTED_RECOVERY_VOLTAGE == recoveryVoltageCallback.GetLastValue());
  CHECK(EXPECTED_RECOVERY_VOLTAGE == recoveryVoltageDuringBrownoutCallback);
  CHECK(EXPECTED_BROWNOUT_VOLTAGE == brownoutVoltageDuringRecoveryCallback);
  CHECK(EXPECTED_BROWNOUT_VOLTAGE == RoboRioSim::GetBrownoutVoltage().value());
  CHECK(EXPECTED_RECOVERY_VOLTAGE ==
        RoboRioSim::GetBrownoutRecoveryVoltage().value());
}

TEST_CASE("RoboRioSimTest Rejects invalid brownout thresholds",
          "[wpilibc][simulation]") {
  constexpr double DEFAULT_BROWNOUT_VOLTAGE = 6.75;
  constexpr double DEFAULT_RECOVERY_VOLTAGE = 7.25;

  auto checkInvalidPair = [](double brownoutVoltage, double recoveryVoltage) {
    RoboRioSim::ResetData();
    int32_t status = 0;

    HAL_SetBrownoutVoltages(brownoutVoltage, recoveryVoltage, &status);

    CHECK(status == HAL_PARAMETER_OUT_OF_RANGE);
    CHECK(RoboRioSim::GetBrownoutVoltage().value() == DEFAULT_BROWNOUT_VOLTAGE);
    CHECK(RoboRioSim::GetBrownoutRecoveryVoltage().value() ==
          DEFAULT_RECOVERY_VOLTAGE);
  };

  checkInvalidPair(4.99, 7.0);
  checkInvalidPair(6.5, 8.51);
  checkInvalidPair(6.5, 6.99);
}

TEST_CASE("RoboRioSimTest Set3V3", "[wpilibc][simulation]") {
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

  RoboRioSim::SetUserVoltage3V3(wpi::units::volts<>{TEST_VOLTAGE});
  CHECK(voltageCallback.WasTriggered());
  CHECK(TEST_VOLTAGE == voltageCallback.GetLastValue());
  CHECK(TEST_VOLTAGE == RoboRioSim::GetUserVoltage3V3().value());
  CHECK(TEST_VOLTAGE == RobotController::GetVoltage3V3());

  RoboRioSim::SetUserCurrent3V3(wpi::units::amperes<>{TEST_CURRENT});
  CHECK(currentCallback.WasTriggered());
  CHECK(TEST_CURRENT == currentCallback.GetLastValue());
  CHECK(TEST_CURRENT == RoboRioSim::GetUserCurrent3V3().value());
  CHECK(TEST_CURRENT == RobotController::GetCurrent3V3());

  RoboRioSim::SetUserActive3V3(false);
  CHECK(activeCallback.WasTriggered());
  CHECK_FALSE(activeCallback.GetLastValue());
  CHECK_FALSE(RoboRioSim::GetUserActive3V3());
  CHECK_FALSE(RobotController::GetEnabled3V3());

  RoboRioSim::SetUserFaults3V3(TEST_FAULTS);
  CHECK(faultCallback.WasTriggered());
  CHECK(TEST_FAULTS == faultCallback.GetLastValue());
  CHECK(TEST_FAULTS == RoboRioSim::GetUserFaults3V3());
  CHECK(TEST_FAULTS == RobotController::GetFaultCount3V3());
}

TEST_CASE("RoboRioSimTest SetCPUTemp", "[wpilibc][simulation]") {
  RoboRioSim::ResetData();

  DoubleCallback callback;
  auto cbHandle =
      RoboRioSim::RegisterCPUTempCallback(callback.GetCallback(), false);
  constexpr double CPU_TEMP = 100.0;

  RoboRioSim::SetCPUTemp(wpi::units::celsius<>{CPU_TEMP});
  CHECK(callback.WasTriggered());
  CHECK(CPU_TEMP == callback.GetLastValue());
  CHECK(CPU_TEMP == RoboRioSim::GetCPUTemp().value());
  CHECK(CPU_TEMP == RobotController::GetCPUTemp().value());
}

TEST_CASE("RoboRioSimTest SetTeamNumber", "[wpilibc][simulation]") {
  RoboRioSim::ResetData();

  IntCallback callback;
  auto cbHandle =
      RoboRioSim::RegisterTeamNumberCallback(callback.GetCallback(), false);
  constexpr int TEAM_NUMBER = 9999;

  RoboRioSim::SetTeamNumber(TEAM_NUMBER);
  CHECK(callback.WasTriggered());
  CHECK(TEAM_NUMBER == callback.GetLastValue());
  CHECK(TEAM_NUMBER == RoboRioSim::GetTeamNumber());
  CHECK(TEAM_NUMBER == RobotController::GetTeamNumber());
}

TEST_CASE("RoboRioSimTest SetSerialNumber", "[wpilibc][simulation]") {
  const std::string SERIAL_NUM = "Hello";

  RoboRioSim::ResetData();

  RoboRioSim::SetSerialNumber(SERIAL_NUM);
  CHECK(SERIAL_NUM == RoboRioSim::GetSerialNumber());
  CHECK(SERIAL_NUM == RobotController::GetSerialNumber());

  const std::string SERIAL_NUMBER_OVERFLOW = "SerialNumber";
  const std::string SERIAL_NUMBER_TRUNCATED =
      SERIAL_NUMBER_OVERFLOW.substr(0, 8);

  RoboRioSim::SetSerialNumber(SERIAL_NUMBER_OVERFLOW);
  CHECK(SERIAL_NUMBER_TRUNCATED == RoboRioSim::GetSerialNumber());
  CHECK(SERIAL_NUMBER_TRUNCATED == RobotController::GetSerialNumber());
}

TEST_CASE("RoboRioSimTest SetComments", "[wpilibc][simulation]") {
  const std::string COMMENTS =
      "Hello! These are comments in the roboRIO web interface!";

  RoboRioSim::ResetData();

  RoboRioSim::SetComments(COMMENTS);
  CHECK(COMMENTS == RoboRioSim::GetComments());
  CHECK(COMMENTS == RobotController::GetComments());

  const std::string COMMENTS_OVERFLOW =
      "Hello! These are comments in the roboRIO web interface! This comment "
      "exceeds 64 characters!";
  const std::string COMMENTS_TRUNCATED = COMMENTS_OVERFLOW.substr(0, 64);

  RoboRioSim::SetComments(COMMENTS_OVERFLOW);
  CHECK(COMMENTS_TRUNCATED == RoboRioSim::GetComments());
  CHECK(COMMENTS_TRUNCATED == RobotController::GetComments());
}

}  // namespace wpi::sim
