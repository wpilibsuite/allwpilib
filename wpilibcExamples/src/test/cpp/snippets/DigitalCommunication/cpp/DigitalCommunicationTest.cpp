// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <string>
#include <thread>

#include <gtest/gtest.h>

#include "Robot.hpp"
#include "wpi/hal/DriverStationTypes.h"
#include "wpi/simulation/DIOSim.hpp"
#include "wpi/simulation/DriverStationSim.hpp"
#include "wpi/simulation/SimHooks.hpp"
#include "wpi/units/time.hpp"

template <typename T>
class DigitalCommunicationTest : public testing::TestWithParam<T> {
 public:
  wpi::sim::DIOSim allianceOutput{Robot::kAlliancePort};
  wpi::sim::DIOSim enabledOutput{Robot::kEnabledPort};
  wpi::sim::DIOSim autonomousOutput{Robot::kAutonomousPort};
  wpi::sim::DIOSim alertOutput{Robot::kAlertPort};
  Robot robot;
  std::optional<std::thread> thread;

  void SetUp() override {
    wpi::sim::PauseTiming();
    wpi::sim::SetProgramStarted(false);
    wpi::sim::DriverStationSim::ResetData();

    thread = std::thread([&] { robot.StartCompetition(); });
    wpi::sim::WaitForProgramStart();
  }

  void TearDown() override {
    robot.EndCompetition();
    thread->join();
    allianceOutput.ResetData();
    enabledOutput.ResetData();
    autonomousOutput.ResetData();
    alertOutput.ResetData();
  }
};

class AllianceTest : public DigitalCommunicationTest<HAL_AllianceStationID> {};

TEST_P(AllianceTest, Alliance) {
  auto alliance = GetParam();
  wpi::sim::DriverStationSim::SetAllianceStationId(alliance);
  wpi::sim::DriverStationSim::NotifyNewData();

  EXPECT_TRUE(allianceOutput.GetInitialized());
  EXPECT_FALSE(allianceOutput.GetIsInput());

  wpi::sim::StepTiming(20_ms);

  bool isRed = false;
  switch (alliance) {
    case HAL_ALLIANCE_STATION_BLUE_1:
    case HAL_ALLIANCE_STATION_BLUE_2:
    case HAL_ALLIANCE_STATION_BLUE_3:
    case HAL_ALLIANCE_STATION_UNKNOWN:
      isRed = false;
      break;
    case HAL_ALLIANCE_STATION_RED_1:
    case HAL_ALLIANCE_STATION_RED_2:
    case HAL_ALLIANCE_STATION_RED_3:
      isRed = true;
      break;
  }
  EXPECT_EQ(isRed, allianceOutput.GetValue());
}

INSTANTIATE_TEST_SUITE_P(
    DigitalCommunicationTests, AllianceTest,
    testing::Values<HAL_AllianceStationID>(
        HAL_ALLIANCE_STATION_RED_1, HAL_ALLIANCE_STATION_RED_2,
        HAL_ALLIANCE_STATION_RED_3, HAL_ALLIANCE_STATION_BLUE_1,
        HAL_ALLIANCE_STATION_BLUE_2, HAL_ALLIANCE_STATION_BLUE_3,
        HAL_ALLIANCE_STATION_UNKNOWN),
    [](const testing::TestParamInfo<AllianceTest::ParamType>& info) {
      switch (info.param) {
        case HAL_ALLIANCE_STATION_BLUE_1:
          return std::string{"Blue1"};
        case HAL_ALLIANCE_STATION_BLUE_2:
          return std::string{"Blue2"};
        case HAL_ALLIANCE_STATION_BLUE_3:
          return std::string{"Blue3"};
        case HAL_ALLIANCE_STATION_RED_1:
          return std::string{"Red1"};
        case HAL_ALLIANCE_STATION_RED_2:
          return std::string{"Red2"};
        case HAL_ALLIANCE_STATION_RED_3:
          return std::string{"Red3"};
        case HAL_ALLIANCE_STATION_UNKNOWN:
          return std::string{"Unknown"};
      }
      return std::string{"Error"};
    });

class EnabledTest : public DigitalCommunicationTest<bool> {};

TEST_P(EnabledTest, Enabled) {
  auto enabled = GetParam();
  wpi::sim::DriverStationSim::SetEnabled(enabled);
  wpi::sim::DriverStationSim::NotifyNewData();

  EXPECT_TRUE(enabledOutput.GetInitialized());
  EXPECT_FALSE(enabledOutput.GetIsInput());

  wpi::sim::StepTiming(20_ms);

  EXPECT_EQ(enabled, enabledOutput.GetValue());
}

INSTANTIATE_TEST_SUITE_P(DigitalCommunicationTests, EnabledTest,
                         testing::Bool(), testing::PrintToStringParamName());

class AutonomousTest : public DigitalCommunicationTest<bool> {};

TEST_P(AutonomousTest, Autonomous) {
  auto autonomous = GetParam();
  wpi::sim::DriverStationSim::SetRobotMode(
      autonomous ? HAL_ROBOT_MODE_AUTONOMOUS : HAL_ROBOT_MODE_TELEOPERATED);
  wpi::sim::DriverStationSim::NotifyNewData();

  EXPECT_TRUE(autonomousOutput.GetInitialized());
  EXPECT_FALSE(autonomousOutput.GetIsInput());

  wpi::sim::StepTiming(20_ms);

  EXPECT_EQ(autonomous, autonomousOutput.GetValue());
}

INSTANTIATE_TEST_SUITE_P(DigitalCommunicationTests, AutonomousTest,
                         testing::Bool(), testing::PrintToStringParamName());

class AlertTest : public DigitalCommunicationTest<double> {};

TEST_P(AlertTest, Alert) {
  auto matchTime = GetParam();
  wpi::sim::DriverStationSim::SetMatchTime(matchTime);
  wpi::sim::DriverStationSim::NotifyNewData();

  EXPECT_TRUE(alertOutput.GetInitialized());
  EXPECT_FALSE(alertOutput.GetIsInput());

  wpi::sim::StepTiming(20_ms);

  EXPECT_EQ(matchTime <= 30 && matchTime >= 25, alertOutput.GetValue());
}

INSTANTIATE_TEST_SUITE_P(
    DigitalCommunicationTests, AlertTest, testing::Values(45.0, 27.0, 23.0),
    [](const testing::TestParamInfo<double>& info) {
      return testing::PrintToString(info.param).append("_s");
    });
