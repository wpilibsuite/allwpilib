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
  constexpr double kTestVoltage = 1.91;

  RoboRioSim::SetVInVoltage(wpi::units::volt_t{kTestVoltage});
  CHECK(voltageCallback.WasTriggered());
  CHECK(kTestVoltage == voltageCallback.GetLastValue());
  CHECK(kTestVoltage == RoboRioSim::GetVInVoltage().value());
  CHECK(kTestVoltage == RobotController::GetInputVoltage());
}

TEST_CASE("RoboRioSimTest SetBrownout", "[wpilibc][simulation]") {
  RoboRioSim::ResetData();

  DoubleCallback brownoutVoltageCallback;
  DoubleCallback recoveryVoltageCallback;
  auto brownoutVoltageCb = RoboRioSim::RegisterBrownoutVoltageCallback(
      brownoutVoltageCallback.GetCallback(), false);
  auto recoveryVoltageCb =
      RoboRioSim::RegisterBrownoutRecoveryVoltageCallback(
          recoveryVoltageCallback.GetCallback(), false);
  constexpr double kTestBrownoutVoltage = 7.501;
  constexpr double kTestRecoveryVoltage = 8.001;

  RobotController::SetBrownoutVoltages(
      wpi::units::volt_t{kTestBrownoutVoltage},
      wpi::units::volt_t{kTestRecoveryVoltage});
  CHECK(brownoutVoltageCallback.WasTriggered());
  CHECK(recoveryVoltageCallback.WasTriggered());
  CHECK(kTestBrownoutVoltage == brownoutVoltageCallback.GetLastValue());
  CHECK(kTestRecoveryVoltage == recoveryVoltageCallback.GetLastValue());
  CHECK(kTestBrownoutVoltage == RoboRioSim::GetBrownoutVoltage().value());
  CHECK(kTestRecoveryVoltage ==
        RoboRioSim::GetBrownoutRecoveryVoltage().value());
}

TEST_CASE("RoboRioSimTest Rejects invalid brownout thresholds",
          "[wpilibc][simulation]") {
  constexpr double kDefaultBrownoutVoltage = 6.75;
  constexpr double kDefaultRecoveryVoltage = 7.25;

  auto checkInvalidPair = [](double brownoutVoltage, double recoveryVoltage) {
    RoboRioSim::ResetData();
    int32_t status = 0;

    HAL_SetBrownoutVoltages(brownoutVoltage, recoveryVoltage, &status);

    CHECK(status == HAL_PARAMETER_OUT_OF_RANGE);
    CHECK(RoboRioSim::GetBrownoutVoltage().value() ==
          kDefaultBrownoutVoltage);
    CHECK(RoboRioSim::GetBrownoutRecoveryVoltage().value() ==
          kDefaultRecoveryVoltage);
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
  constexpr double kTestVoltage = 22.9;
  constexpr double kTestCurrent = 174;
  constexpr int kTestFaults = 229;

  RoboRioSim::SetUserVoltage3V3(wpi::units::volt_t{kTestVoltage});
  CHECK(voltageCallback.WasTriggered());
  CHECK(kTestVoltage == voltageCallback.GetLastValue());
  CHECK(kTestVoltage == RoboRioSim::GetUserVoltage3V3().value());
  CHECK(kTestVoltage == RobotController::GetVoltage3V3());

  RoboRioSim::SetUserCurrent3V3(wpi::units::ampere_t{kTestCurrent});
  CHECK(currentCallback.WasTriggered());
  CHECK(kTestCurrent == currentCallback.GetLastValue());
  CHECK(kTestCurrent == RoboRioSim::GetUserCurrent3V3().value());
  CHECK(kTestCurrent == RobotController::GetCurrent3V3());

  RoboRioSim::SetUserActive3V3(false);
  CHECK(activeCallback.WasTriggered());
  CHECK_FALSE(activeCallback.GetLastValue());
  CHECK_FALSE(RoboRioSim::GetUserActive3V3());
  CHECK_FALSE(RobotController::GetEnabled3V3());

  RoboRioSim::SetUserFaults3V3(kTestFaults);
  CHECK(faultCallback.WasTriggered());
  CHECK(kTestFaults == faultCallback.GetLastValue());
  CHECK(kTestFaults == RoboRioSim::GetUserFaults3V3());
  CHECK(kTestFaults == RobotController::GetFaultCount3V3());
}

TEST_CASE("RoboRioSimTest SetCPUTemp", "[wpilibc][simulation]") {
  RoboRioSim::ResetData();

  DoubleCallback callback;
  auto cbHandle =
      RoboRioSim::RegisterCPUTempCallback(callback.GetCallback(), false);
  constexpr double kCPUTemp = 100.0;

  RoboRioSim::SetCPUTemp(wpi::units::celsius_t{kCPUTemp});
  CHECK(callback.WasTriggered());
  CHECK(kCPUTemp == callback.GetLastValue());
  CHECK(kCPUTemp == RoboRioSim::GetCPUTemp().value());
  CHECK(kCPUTemp == RobotController::GetCPUTemp().value());
}

TEST_CASE("RoboRioSimTest SetTeamNumber", "[wpilibc][simulation]") {
  RoboRioSim::ResetData();

  IntCallback callback;
  auto cbHandle =
      RoboRioSim::RegisterTeamNumberCallback(callback.GetCallback(), false);
  constexpr int kTeamNumber = 9999;

  RoboRioSim::SetTeamNumber(kTeamNumber);
  CHECK(callback.WasTriggered());
  CHECK(kTeamNumber == callback.GetLastValue());
  CHECK(kTeamNumber == RoboRioSim::GetTeamNumber());
  CHECK(kTeamNumber == RobotController::GetTeamNumber());
}

TEST_CASE("RoboRioSimTest SetSerialNumber", "[wpilibc][simulation]") {
  const std::string kSerialNum = "Hello";

  RoboRioSim::ResetData();

  RoboRioSim::SetSerialNumber(kSerialNum);
  CHECK(kSerialNum == RoboRioSim::GetSerialNumber());
  CHECK(kSerialNum == RobotController::GetSerialNumber());

  const std::string kSerialNumberOverflow = "SerialNumber";
  const std::string kSerialNumberTruncated = kSerialNumberOverflow.substr(0, 8);

  RoboRioSim::SetSerialNumber(kSerialNumberOverflow);
  CHECK(kSerialNumberTruncated == RoboRioSim::GetSerialNumber());
  CHECK(kSerialNumberTruncated == RobotController::GetSerialNumber());
}

TEST_CASE("RoboRioSimTest SetComments", "[wpilibc][simulation]") {
  const std::string kComments =
      "Hello! These are comments in the roboRIO web interface!";

  RoboRioSim::ResetData();

  RoboRioSim::SetComments(kComments);
  CHECK(kComments == RoboRioSim::GetComments());
  CHECK(kComments == RobotController::GetComments());

  const std::string kCommentsOverflow =
      "Hello! These are comments in the roboRIO web interface! This comment "
      "exceeds 64 characters!";
  const std::string kCommentsTruncated = kCommentsOverflow.substr(0, 64);

  RoboRioSim::SetComments(kCommentsOverflow);
  CHECK(kCommentsTruncated == RoboRioSim::GetComments());
  CHECK(kCommentsTruncated == RobotController::GetComments());
}

}  // namespace wpi::sim
