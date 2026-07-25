// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <format>
#include <string>
#include <thread>

#include <gtest/gtest.h>

#include "Robot.hpp"
#include "wpi/hal/DriverStationTypes.h"
#include "wpi/hal/simulation/I2CData.h"
#include "wpi/simulation/DriverStationSim.hpp"
#include "wpi/simulation/SimHooks.hpp"
#include "wpi/units/time.hpp"

static std::string gString;

void callback(const char* name, void* param, const unsigned char* buffer,
              unsigned int count) {
  gString.assign(reinterpret_cast<const char*>(buffer),
                 static_cast<int>(count));
}

template <typename T>
class I2CCommunicationTest : public testing::TestWithParam<T> {
 public:
  Robot robot;
  std::optional<std::thread> thread;
  int32_t callbackHandle;
  int32_t port;

  void SetUp() override {
    gString = std::string();
    wpi::sim::PauseTiming();
    wpi::sim::SetProgramStarted(false);
    wpi::sim::DriverStationSim::ResetData();
    port = static_cast<int32_t>(Robot::kPort);

    callbackHandle = HALSIM_RegisterI2CWriteCallback(port, &callback, nullptr);

    thread = std::thread([&] { robot.StartCompetition(); });
    wpi::sim::WaitForProgramStart();
  }

  void TearDown() override {
    robot.EndCompetition();
    thread->join();

    HALSIM_CancelI2CWriteCallback(port, callbackHandle);
    HALSIM_ResetI2CData(port);
  }
};

class AllianceTest : public I2CCommunicationTest<wpi::hal::AllianceStationID> {
};

TEST_P(AllianceTest, Alliance) {
  auto alliance = GetParam();
  wpi::sim::DriverStationSim::SetAllianceStationId(alliance);
  wpi::sim::DriverStationSim::NotifyNewData();

  EXPECT_TRUE(HALSIM_GetI2CInitialized(port));

  wpi::sim::StepTiming(20_ms);

  char expected = 'U';
  switch (alliance) {
    case wpi::hal::AllianceStationID::BLUE_1:
    case wpi::hal::AllianceStationID::BLUE_2:
    case wpi::hal::AllianceStationID::BLUE_3:
      expected = 'B';
      break;
    case wpi::hal::AllianceStationID::RED_1:
    case wpi::hal::AllianceStationID::RED_2:
    case wpi::hal::AllianceStationID::RED_3:
      expected = 'R';
      break;
    case wpi::hal::AllianceStationID::UNKNOWN:
      expected = 'U';
      break;
  }
  EXPECT_EQ(expected, gString.at(0));
}

INSTANTIATE_TEST_SUITE_P(
    I2CCommunicationTests, AllianceTest,
    testing::Values<wpi::hal::AllianceStationID>(
        wpi::hal::AllianceStationID::RED_1, wpi::hal::AllianceStationID::RED_2,
        wpi::hal::AllianceStationID::RED_3, wpi::hal::AllianceStationID::BLUE_1,
        wpi::hal::AllianceStationID::BLUE_2,
        wpi::hal::AllianceStationID::BLUE_3,
        wpi::hal::AllianceStationID::UNKNOWN),
    [](const testing::TestParamInfo<AllianceTest::ParamType>& info) {
      switch (info.param) {
        case wpi::hal::AllianceStationID::BLUE_1:
          return std::string{"Blue1"};
        case wpi::hal::AllianceStationID::BLUE_2:
          return std::string{"Blue2"};
        case wpi::hal::AllianceStationID::BLUE_3:
          return std::string{"Blue3"};
        case wpi::hal::AllianceStationID::RED_1:
          return std::string{"Red1"};
        case wpi::hal::AllianceStationID::RED_2:
          return std::string{"Red2"};
        case wpi::hal::AllianceStationID::RED_3:
          return std::string{"Red3"};
        case wpi::hal::AllianceStationID::UNKNOWN:
          return std::string{"Unknown"};
      }
      return std::string{"Error"};
    });

class EnabledTest : public I2CCommunicationTest<bool> {};

TEST_P(EnabledTest, Enabled) {
  auto enabled = GetParam();
  wpi::sim::DriverStationSim::SetEnabled(enabled);
  wpi::sim::DriverStationSim::NotifyNewData();

  EXPECT_TRUE(HALSIM_GetI2CInitialized(port));

  wpi::sim::StepTiming(20_ms);

  char expected = enabled ? 'E' : 'D';
  EXPECT_EQ(expected, gString.at(1));
}

INSTANTIATE_TEST_SUITE_P(I2CCommunicationTests, EnabledTest, testing::Bool(),
                         testing::PrintToStringParamName());

class AutonomousTest : public I2CCommunicationTest<bool> {};

TEST_P(AutonomousTest, Autonomous) {
  auto autonomous = GetParam();
  wpi::sim::DriverStationSim::SetRobotMode(
      autonomous ? wpi::hal::RobotMode::AUTONOMOUS
                 : wpi::hal::RobotMode::TELEOPERATED);
  wpi::sim::DriverStationSim::NotifyNewData();

  EXPECT_TRUE(HALSIM_GetI2CInitialized(port));

  wpi::sim::StepTiming(20_ms);

  char expected = autonomous ? 'A' : 'T';
  EXPECT_EQ(expected, gString.at(2));
}

INSTANTIATE_TEST_SUITE_P(I2CCommunicationTests, AutonomousTest, testing::Bool(),
                         testing::PrintToStringParamName());

class MatchTimeTest : public I2CCommunicationTest<int> {};

TEST_P(MatchTimeTest, Alert) {
  auto matchTime = GetParam();
  wpi::sim::DriverStationSim::SetMatchTime(matchTime);
  wpi::sim::DriverStationSim::NotifyNewData();

  EXPECT_TRUE(HALSIM_GetI2CInitialized(port));

  wpi::sim::StepTiming(20_ms);

  std::string expected = std::format("{:03}", matchTime);
  EXPECT_EQ(expected, gString.substr(3));
}

INSTANTIATE_TEST_SUITE_P(
    I2CCommunicationTests, MatchTimeTest, testing::Values(112, 45, 27, 23, 3),
    [](const testing::TestParamInfo<int>& info) {
      return testing::PrintToString(info.param).append("_s");
    });
