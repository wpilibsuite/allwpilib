// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/framework/OpModeRobot.hpp"

#include <sys/types.h>

#include <gtest/gtest.h>

#include "wpi/simulation/DriverStationSim.hpp"
#include "wpi/simulation/SimHooks.hpp"
#include "wpi/util/Color.hpp"
#include "wpi/util/string.hpp"

namespace {
class OpModeRobotTest : public ::testing::Test {
 protected:
  void SetUp() override {
    wpi::sim::PauseTiming();
    wpi::sim::SetProgramStarted(false);
  }

  void TearDown() override { wpi::sim::ResumeTiming(); }
};

class MockRobot;

class MockOpMode : public wpi::OpMode {
 public:
  std::atomic<uint32_t> m_disabledPeriodicCount{0};
  std::atomic<uint32_t> m_opModeRunCount{0};
  std::atomic<uint32_t> m_opModeStopCount{0};

  MockOpMode() = default;
  void DisabledPeriodic() override { m_disabledPeriodicCount++; }
  void OpModeRun(int64_t opModeId) override { m_opModeRunCount++; }
  void OpModeStop() override { m_opModeStopCount++; }
};

class OneArgOpMode : public wpi::OpMode {
 public:
  explicit OneArgOpMode(MockRobot& robot) {}
  void OpModeRun(int64_t opModeId) override {}
  void OpModeStop() override {}
};

class MockRobot : public wpi::OpModeRobot<MockRobot> {
 public:
  std::atomic<uint32_t> m_driverStationConnectedCount{0};
  std::atomic<uint32_t> m_nonePeriodicCount{0};

  MockRobot() = default;

  void DriverStationConnected() override { m_driverStationConnectedCount++; }

  void NonePeriodic() override { m_nonePeriodicCount++; }
};
}  // namespace

static_assert(wpi::ConstructibleOpMode<MockOpMode, MockRobot>);
static_assert(wpi::ConstructibleOpMode<OneArgOpMode, MockRobot>);

TEST_F(OpModeRobotTest, AddOpMode) {
  struct MyMockRobot : public MockRobot {
    MyMockRobot() {
      AddOpMode<MockOpMode>(wpi::RobotMode::AUTONOMOUS, "NoArgOpMode-Auto",
                            "Group", "Description", wpi::util::Color::kWhite,
                            wpi::util::Color::kBlack);
      AddOpMode<OneArgOpMode>(wpi::RobotMode::TEST, "OneArgOpMode-Test",
                              "Group", "Description", wpi::util::Color::kWhite,
                              wpi::util::Color::kBlack);
      AddOpMode<MockOpMode>(wpi::RobotMode::TELEOPERATED, "NoArgOpMode");
      AddOpMode<OneArgOpMode>(wpi::RobotMode::TELEOPERATED, "OneArgOpMode");
      PublishOpModes();
    }
  };
  MyMockRobot robot;

  auto options = wpi::sim::DriverStationSim::GetOpModeOptions();
  ASSERT_EQ(options.size(), 4u);
  int indexes[4] = {-1, -1, -1, -1};
  for (size_t i = 0; i < options.size(); ++i) {
    auto name = wpi::util::to_string_view(&options[i].name);
    if (name == "NoArgOpMode-Auto") {
      indexes[0] = i;
    } else if (name == "OneArgOpMode-Test") {
      indexes[1] = i;
    } else if (name == "NoArgOpMode") {
      indexes[2] = i;
    } else if (name == "OneArgOpMode") {
      indexes[3] = i;
    }
  }

  int i = indexes[0];
  ASSERT_NE(i, -1);
  EXPECT_EQ(wpi::util::to_string_view(&options[i].group), "Group");
  EXPECT_EQ(wpi::util::to_string_view(&options[i].description), "Description");
  EXPECT_EQ(options[i].textColor, 0xffffff);
  EXPECT_EQ(options[i].backgroundColor, 0x000000);

  i = indexes[1];
  ASSERT_NE(i, -1);
  EXPECT_EQ(wpi::util::to_string_view(&options[i].group), "Group");
  EXPECT_EQ(wpi::util::to_string_view(&options[i].description), "Description");
  EXPECT_EQ(options[i].textColor, 0xffffff);
  EXPECT_EQ(options[i].backgroundColor, 0x000000);

  i = indexes[2];
  ASSERT_NE(i, -1);
  EXPECT_EQ(wpi::util::to_string_view(&options[i].group), "");
  EXPECT_EQ(wpi::util::to_string_view(&options[i].description), "");
  EXPECT_EQ(options[i].textColor, -1);
  EXPECT_EQ(options[i].backgroundColor, -1);

  i = indexes[3];
  ASSERT_NE(i, -1);
  EXPECT_EQ(wpi::util::to_string_view(&options[i].group), "");
  EXPECT_EQ(wpi::util::to_string_view(&options[i].description), "");
  EXPECT_EQ(options[i].textColor, -1);
  EXPECT_EQ(options[i].backgroundColor, -1);
}

TEST_F(OpModeRobotTest, ClearOpModes) {
  struct MyMockRobot : public MockRobot {
    MyMockRobot() {
      AddOpMode<MockOpMode>(wpi::RobotMode::TELEOPERATED, "NoArgOpMode");
      AddOpMode<OneArgOpMode>(wpi::RobotMode::TELEOPERATED, "OneArgOpMode");
      PublishOpModes();
    }
  };
  MyMockRobot robot;

  robot.ClearOpModes();
  auto options = wpi::sim::DriverStationSim::GetOpModeOptions();
  EXPECT_TRUE(options.empty());
}

TEST_F(OpModeRobotTest, RemoveOpMode) {
  struct MyMockRobot : public MockRobot {
    MyMockRobot() {
      AddOpMode<MockOpMode>(wpi::RobotMode::TELEOPERATED, "NoArgOpMode");
      AddOpMode<OneArgOpMode>(wpi::RobotMode::TELEOPERATED, "OneArgOpMode");
      PublishOpModes();
    }
  };
  MyMockRobot robot;

  robot.RemoveOpMode(wpi::RobotMode::TELEOPERATED, "NoArgOpMode");
  robot.PublishOpModes();
  auto options = wpi::sim::DriverStationSim::GetOpModeOptions();
  ASSERT_EQ(options.size(), 1u);
  EXPECT_EQ(wpi::util::to_string_view(&options[0].name), "OneArgOpMode");
}

TEST_F(OpModeRobotTest, NonePeriodic) {
  struct MyMockRobot : public MockRobot {
    MyMockRobot() {
      AddOpMode<MockOpMode>(wpi::RobotMode::TELEOPERATED, "NoArgOpMode");
      AddOpMode<OneArgOpMode>(wpi::RobotMode::TELEOPERATED, "OneArgOpMode");
      PublishOpModes();
    }
  };
  MyMockRobot robot;

  std::thread robotThread{[&] { robot.StartCompetition(); }};
  wpi::sim::WaitForProgramStart();

  // Time step to get periodic calls on 50 ms timeout
  wpi::sim::StepTiming(110_ms);
  EXPECT_EQ(robot.m_nonePeriodicCount.load(), 2u);

  robot.EndCompetition();
  robotThread.join();
}
