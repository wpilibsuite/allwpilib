// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <string>
#include <thread>

#include <frc/simulation/DriverStationSim.h>
#include <frc/simulation/SimHooks.h>
#include <gtest/gtest.h>
#include <hal/simulation/I2CData.h>
#include <units/time.h>

#include "Robot.h"

static std::string gString;

void callback(const char* name, void* param, const unsigned char* buffer,
              unsigned int count) {
  gString.assign(reinterpret_cast<const char*>(buffer),
                 static_cast<int>(count));
}

template <typename T>
class I2CCommunicationTest : public testing::TestWithParam<T> {
 public:
  Robot m_robot;
  std::optional<std::thread> m_thread;
  int32_t m_callback;
  int32_t m_port;

  void SetUp() override {
    gString = std::string();
    frc::sim::PauseTiming();
    frc::sim::DriverStationSim::ResetData();
    m_port = static_cast<int32_t>(Robot::PORT);

    m_callback = HALSIM_RegisterI2CWriteCallback(m_port, &callback, nullptr);

    m_thread = std::thread([&] { m_robot.StartCompetition(); });
    frc::sim::StepTiming(0.0_ms);  // Wait for Notifiers
  }

  void TearDown() override {
    m_robot.EndCompetition();
    m_thread->join();

    HALSIM_CancelI2CWriteCallback(m_port, m_callback);
    HALSIM_ResetI2CData(m_port);
  }
};

class AllianceTest : public I2CCommunicationTest<HAL_AllianceStationID> {};

TEST_P(AllianceTest, Alliance) {
  auto alliance = GetParam();
  frc::sim::DriverStationSim::SetAllianceStationId(alliance);
  frc::sim::DriverStationSim::NotifyNewData();

  EXPECT_TRUE(HALSIM_GetI2CInitialized(m_port));

  frc::sim::StepTiming(20_ms);

  char expected = 'U';
  switch (alliance) {
    case HAL_AllianceStationID_Blue1:
    case HAL_AllianceStationID_Blue2:
    case HAL_AllianceStationID_Blue3:
      expected = 'B';
      break;
    case HAL_AllianceStationID_Red1:
    case HAL_AllianceStationID_Red2:
    case HAL_AllianceStationID_Red3:
      expected = 'R';
      break;
    case HAL_AllianceStationID_Unknown:
      expected = 'U';
      break;
  }
  EXPECT_EQ(expected, gString.at(0));
}

INSTANTIATE_TEST_SUITE_P(
    I2CCommunicationTests, AllianceTest,
    testing::Values<HAL_AllianceStationID>(
        HAL_AllianceStationID_Red1, HAL_AllianceStationID_Red2,
        HAL_AllianceStationID_Red3, HAL_AllianceStationID_Blue1,
        HAL_AllianceStationID_Blue2, HAL_AllianceStationID_Blue3,
        HAL_AllianceStationID_Unknown),
    [](const testing::TestParamInfo<AllianceTest::ParamType>& info) {
      switch (info.param) {
        case HAL_AllianceStationID_Blue1:
          return std::string{"Blue1"};
        case HAL_AllianceStationID_Blue2:
          return std::string{"Blue2"};
        case HAL_AllianceStationID_Blue3:
          return std::string{"Blue3"};
        case HAL_AllianceStationID_Red1:
          return std::string{"Red1"};
        case HAL_AllianceStationID_Red2:
          return std::string{"Red2"};
        case HAL_AllianceStationID_Red3:
          return std::string{"Red3"};
        case HAL_AllianceStationID_Unknown:
          return std::string{"Unknown"};
      }
      return std::string{"Error"};
    });

class EnabledTest : public I2CCommunicationTest<bool> {};

TEST_P(EnabledTest, Enabled) {
  auto enabled = GetParam();
  frc::sim::DriverStationSim::SetEnabled(enabled);
  frc::sim::DriverStationSim::NotifyNewData();

  EXPECT_TRUE(HALSIM_GetI2CInitialized(m_port));

  frc::sim::StepTiming(20_ms);

  char expected = enabled ? 'E' : 'D';
  EXPECT_EQ(expected, gString.at(1));
}

INSTANTIATE_TEST_SUITE_P(I2CCommunicationTests, EnabledTest, testing::Bool(),
                         testing::PrintToStringParamName());

class AutonomousTest : public I2CCommunicationTest<bool> {};

TEST_P(AutonomousTest, Autonomous) {
  auto autonomous = GetParam();
  frc::sim::DriverStationSim::SetAutonomous(autonomous);
  frc::sim::DriverStationSim::NotifyNewData();

  EXPECT_TRUE(HALSIM_GetI2CInitialized(m_port));

  frc::sim::StepTiming(20_ms);

  char expected = autonomous ? 'A' : 'T';
  EXPECT_EQ(expected, gString.at(2));
}

INSTANTIATE_TEST_SUITE_P(I2CCommunicationTests, AutonomousTest, testing::Bool(),
                         testing::PrintToStringParamName());

class MatchTimeTest : public I2CCommunicationTest<int> {};

TEST_P(MatchTimeTest, Alert) {
  auto matchTime = GetParam();
  frc::sim::DriverStationSim::SetMatchTime(matchTime);
  frc::sim::DriverStationSim::NotifyNewData();

  EXPECT_TRUE(HALSIM_GetI2CInitialized(m_port));

  frc::sim::StepTiming(20_ms);

  std::string expected = fmt::format("{:03}", matchTime);
  EXPECT_EQ(expected, gString.substr(3));
}

INSTANTIATE_TEST_SUITE_P(
    I2CCommunicationTests, MatchTimeTest, testing::Values(112, 45, 27, 23, 3),
    [](const testing::TestParamInfo<int>& info) {
      return testing::PrintToString(info.param).append("_s");
    });
