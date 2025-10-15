// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <string>
#include <thread>

#include <gtest/gtest.h>
#include <wpi/simulation/DIOSim.hpp>
#include <wpi/simulation/DriverStationSim.hpp>
#include <wpi/simulation/SimHooks.hpp>
#include <wpi/units/time.hpp>

#include "Robot.hpp"

template <typename T>
class DigitalCommunicationTest : public testing::TestWithParam<T> {
 public:
  frc::sim::DIOSim m_allianceOutput{Robot::kAlliancePort};
  frc::sim::DIOSim m_enabledOutput{Robot::kEnabledPort};
  frc::sim::DIOSim m_autonomousOutput{Robot::kAutonomousPort};
  frc::sim::DIOSim m_alertOutput{Robot::kAlertPort};
  Robot m_robot;
  std::optional<std::thread> m_thread;

  void SetUp() override {
    frc::sim::PauseTiming();
    frc::sim::DriverStationSim::ResetData();

    m_thread = std::thread([&] { m_robot.StartCompetition(); });
    frc::sim::StepTiming(0.0_ms);
    // SimHooks.stepTiming(0.0); // Wait for Notifiers
  }

  void TearDown() override {
    m_robot.EndCompetition();
    m_thread->join();
    m_allianceOutput.ResetData();
    m_enabledOutput.ResetData();
    m_autonomousOutput.ResetData();
    m_alertOutput.ResetData();
  }
};

class AllianceTest : public DigitalCommunicationTest<HAL_AllianceStationID> {};

TEST_P(AllianceTest, Alliance) {
  auto alliance = GetParam();
  frc::sim::DriverStationSim::SetAllianceStationId(alliance);
  frc::sim::DriverStationSim::NotifyNewData();

  EXPECT_TRUE(m_allianceOutput.GetInitialized());
  EXPECT_FALSE(m_allianceOutput.GetIsInput());

  frc::sim::StepTiming(20_ms);

  bool isRed = false;
  switch (alliance) {
    case HAL_AllianceStationID_kBlue1:
    case HAL_AllianceStationID_kBlue2:
    case HAL_AllianceStationID_kBlue3:
    case HAL_AllianceStationID_kUnknown:
      isRed = false;
      break;
    case HAL_AllianceStationID_kRed1:
    case HAL_AllianceStationID_kRed2:
    case HAL_AllianceStationID_kRed3:
      isRed = true;
      break;
  }
  EXPECT_EQ(isRed, m_allianceOutput.GetValue());
}

INSTANTIATE_TEST_SUITE_P(
    DigitalCommunicationTests, AllianceTest,
    testing::Values<HAL_AllianceStationID>(
        HAL_AllianceStationID_kRed1, HAL_AllianceStationID_kRed2,
        HAL_AllianceStationID_kRed3, HAL_AllianceStationID_kBlue1,
        HAL_AllianceStationID_kBlue2, HAL_AllianceStationID_kBlue3,
        HAL_AllianceStationID_kUnknown),
    [](const testing::TestParamInfo<AllianceTest::ParamType>& info) {
      switch (info.param) {
        case HAL_AllianceStationID_kBlue1:
          return std::string{"Blue1"};
        case HAL_AllianceStationID_kBlue2:
          return std::string{"Blue2"};
        case HAL_AllianceStationID_kBlue3:
          return std::string{"Blue3"};
        case HAL_AllianceStationID_kRed1:
          return std::string{"Red1"};
        case HAL_AllianceStationID_kRed2:
          return std::string{"Red2"};
        case HAL_AllianceStationID_kRed3:
          return std::string{"Red3"};
        case HAL_AllianceStationID_kUnknown:
          return std::string{"Unknown"};
      }
      return std::string{"Error"};
    });

class EnabledTest : public DigitalCommunicationTest<bool> {};

TEST_P(EnabledTest, Enabled) {
  auto enabled = GetParam();
  frc::sim::DriverStationSim::SetEnabled(enabled);
  frc::sim::DriverStationSim::NotifyNewData();

  EXPECT_TRUE(m_enabledOutput.GetInitialized());
  EXPECT_FALSE(m_enabledOutput.GetIsInput());

  frc::sim::StepTiming(20_ms);

  EXPECT_EQ(enabled, m_enabledOutput.GetValue());
}

INSTANTIATE_TEST_SUITE_P(DigitalCommunicationTests, EnabledTest,
                         testing::Bool(), testing::PrintToStringParamName());

class AutonomousTest : public DigitalCommunicationTest<bool> {};

TEST_P(AutonomousTest, Autonomous) {
  auto autonomous = GetParam();
  frc::sim::DriverStationSim::SetAutonomous(autonomous);
  frc::sim::DriverStationSim::NotifyNewData();

  EXPECT_TRUE(m_autonomousOutput.GetInitialized());
  EXPECT_FALSE(m_autonomousOutput.GetIsInput());

  frc::sim::StepTiming(20_ms);

  EXPECT_EQ(autonomous, m_autonomousOutput.GetValue());
}

INSTANTIATE_TEST_SUITE_P(DigitalCommunicationTests, AutonomousTest,
                         testing::Bool(), testing::PrintToStringParamName());

class AlertTest : public DigitalCommunicationTest<double> {};

TEST_P(AlertTest, Alert) {
  auto matchTime = GetParam();
  frc::sim::DriverStationSim::SetMatchTime(matchTime);
  frc::sim::DriverStationSim::NotifyNewData();

  EXPECT_TRUE(m_alertOutput.GetInitialized());
  EXPECT_FALSE(m_alertOutput.GetIsInput());

  frc::sim::StepTiming(20_ms);

  EXPECT_EQ(matchTime <= 30 && matchTime >= 25, m_alertOutput.GetValue());
}

INSTANTIATE_TEST_SUITE_P(
    DigitalCommunicationTests, AlertTest, testing::Values(45.0, 27.0, 23.0),
    [](const testing::TestParamInfo<double>& info) {
      return testing::PrintToString(info.param).append("_s");
    });
