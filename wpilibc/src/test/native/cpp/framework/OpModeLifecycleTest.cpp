// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <atomic>
#include <memory>
#include <thread>

#include <gtest/gtest.h>

#include "wpi/driverstation/RobotState.hpp"
#include "wpi/framework/OpModeRobot.hpp"
#include "wpi/hal/DriverStationTypes.h"
#include "wpi/simulation/DriverStationSim.hpp"
#include "wpi/simulation/SimHooks.hpp"
#include "wpi/util/string.hpp"

using namespace wpi::units;

namespace {
struct Counts {
  std::atomic<uint32_t> constructed{0};
  std::atomic<uint32_t> disabledPeriodic{0};
  std::atomic<uint32_t> start{0};
  std::atomic<uint32_t> periodic{0};
  std::atomic<uint32_t> end{0};
  std::atomic<uint32_t> destructed{0};
};

class LifecycleOpMode : public wpi::OpMode {
 public:
  explicit LifecycleOpMode(Counts& counts) : m_counts(counts) {
    m_counts.constructed++;
  }

  ~LifecycleOpMode() override { m_counts.destructed++; }

  void DisabledPeriodic() override { m_counts.disabledPeriodic++; }
  void Start() override { m_counts.start++; }
  void Periodic() override { m_counts.periodic++; }
  void End() override { m_counts.end++; }

 private:
  Counts& m_counts;
};

class LifecycleRobot : public wpi::OpModeRobot<LifecycleRobot> {
 public:
  LifecycleRobot() = default;
};

class OpModeLifecycleTest : public ::testing::Test {
 protected:
  void SetUp() override {
    wpi::sim::PauseTiming();
    wpi::sim::SetProgramStarted(false);
    wpi::sim::DriverStationSim::ResetData();
    wpi::sim::DriverStationSim::SetDsAttached(true);
    wpi::sim::DriverStationSim::SetEnabled(false);
    wpi::RobotState::ClearOpModes();
  }

  void TearDown() override { wpi::sim::ResumeTiming(); }

  static int64_t MakeOpModeId(wpi::RobotMode mode, std::string_view name) {
    return HAL_MakeOpModeId(static_cast<HAL_RobotMode>(mode),
                            std::hash<std::string_view>{}(name));
  }
};

TEST_F(OpModeLifecycleTest, EnabledTransition) {
  Counts counts;
  LifecycleRobot robot;
  robot.AddOpModeFactory(
      [&] { return std::make_unique<LifecycleOpMode>(counts); },
      wpi::RobotMode::TELEOPERATED, "TestOpMode");
  robot.PublishOpModes();

  std::thread robotThread{[&] { robot.StartCompetition(); }};
  wpi::sim::WaitForProgramStart();

  // Ensure DS is attached after program starts
  wpi::sim::DriverStationSim::SetDsAttached(true);
  wpi::sim::DriverStationSim::NotifyNewData();
  wpi::sim::StepTiming(20_ms);  // Let the DS attached state propagate

  // 1. Selected, but disabled
  wpi::sim::DriverStationSim::SetRobotMode(HAL_ROBOT_MODE_TELEOPERATED);
  wpi::sim::DriverStationSim::SetOpMode(
      MakeOpModeId(wpi::RobotMode::TELEOPERATED, "TestOpMode"));
  wpi::sim::DriverStationSim::NotifyNewData();
  wpi::sim::StepTiming(20_ms);
  EXPECT_EQ(counts.constructed.load(), 1u);
  EXPECT_EQ(counts.disabledPeriodic.load(), 1u);

  // 2. Transition to enabled
  wpi::sim::DriverStationSim::SetEnabled(true);
  wpi::sim::DriverStationSim::NotifyNewData();
  wpi::sim::StepTiming(
      40_ms);  // Step twice like Java test to get periodic callback
  EXPECT_EQ(counts.start.load(), 1u);
  EXPECT_EQ(counts.periodic.load(), 1u);

  // 3. Transition to disabled
  wpi::sim::DriverStationSim::SetEnabled(false);
  wpi::sim::DriverStationSim::NotifyNewData();
  wpi::sim::StepTiming(20_ms);
  EXPECT_EQ(counts.end.load(), 1u);
  EXPECT_EQ(counts.destructed.load(), 1u);

  robot.EndCompetition();
  robotThread.join();
}

TEST_F(OpModeLifecycleTest, OpModeChangeWhileEnabled) {
  Counts counts1;
  Counts counts2;
  LifecycleRobot robot;
  robot.AddOpModeFactory(
      [&] { return std::make_unique<LifecycleOpMode>(counts1); },
      wpi::RobotMode::TELEOPERATED, "OpMode1");
  robot.AddOpModeFactory(
      [&] { return std::make_unique<LifecycleOpMode>(counts2); },
      wpi::RobotMode::TELEOPERATED, "OpMode2");
  robot.PublishOpModes();

  std::thread robotThread{[&] { robot.StartCompetition(); }};
  wpi::sim::WaitForProgramStart();

  // Ensure DS is attached after program starts
  wpi::sim::DriverStationSim::SetDsAttached(true);
  wpi::sim::DriverStationSim::NotifyNewData();

  // 1. Select OpMode1 and enable
  wpi::sim::DriverStationSim::SetRobotMode(HAL_ROBOT_MODE_TELEOPERATED);
  wpi::sim::DriverStationSim::SetOpMode(
      MakeOpModeId(wpi::RobotMode::TELEOPERATED, "OpMode1"));
  wpi::sim::DriverStationSim::SetEnabled(true);
  wpi::sim::DriverStationSim::NotifyNewData();
  wpi::sim::StepTiming(40_ms);  // Need two iterations for periodic callback
  EXPECT_EQ(counts1.constructed.load(), 1u);
  EXPECT_EQ(counts1.start.load(), 1u);
  EXPECT_EQ(counts1.periodic.load(), 1u);

  // 2. Change to OpMode2 while enabled
  wpi::sim::DriverStationSim::SetOpMode(
      MakeOpModeId(wpi::RobotMode::TELEOPERATED, "OpMode2"));
  wpi::sim::DriverStationSim::NotifyNewData();
  wpi::sim::StepTiming(40_ms);
  // OpMode1 should be ended and destructed
  EXPECT_EQ(counts1.end.load(), 1u);
  EXPECT_EQ(counts1.destructed.load(), 1u);
  // OpMode2 should be started
  EXPECT_EQ(counts2.constructed.load(), 1u);
  EXPECT_EQ(counts2.start.load(), 1u);
  EXPECT_EQ(counts2.periodic.load(), 2u);

  robot.EndCompetition();
  robotThread.join();
}

TEST_F(OpModeLifecycleTest, OpModeChangeWhileDisabled) {
  Counts counts1;
  Counts counts2;
  LifecycleRobot robot;
  robot.AddOpModeFactory(
      [&] { return std::make_unique<LifecycleOpMode>(counts1); },
      wpi::RobotMode::TELEOPERATED, "OpMode1");
  robot.AddOpModeFactory(
      [&] { return std::make_unique<LifecycleOpMode>(counts2); },
      wpi::RobotMode::TELEOPERATED, "OpMode2");
  robot.PublishOpModes();

  std::thread robotThread{[&] { robot.StartCompetition(); }};
  wpi::sim::WaitForProgramStart();

  // Ensure DS is attached after program starts
  wpi::sim::DriverStationSim::SetDsAttached(true);

  // 1. Select OpMode1 while disabled
  wpi::sim::DriverStationSim::SetRobotMode(HAL_ROBOT_MODE_TELEOPERATED);
  wpi::sim::DriverStationSim::SetOpMode(
      MakeOpModeId(wpi::RobotMode::TELEOPERATED, "OpMode1"));
  wpi::sim::DriverStationSim::NotifyNewData();
  wpi::sim::StepTiming(20_ms);
  EXPECT_EQ(counts1.constructed.load(), 1u);
  EXPECT_EQ(counts1.disabledPeriodic.load(), 1u);

  // 2. Change to OpMode2 while disabled
  wpi::sim::DriverStationSim::SetOpMode(
      MakeOpModeId(wpi::RobotMode::TELEOPERATED, "OpMode2"));
  wpi::sim::DriverStationSim::NotifyNewData();
  wpi::sim::StepTiming(20_ms);
  // OpMode1 should be destructed, but NOT ended
  EXPECT_EQ(counts1.destructed.load(), 1u);
  EXPECT_EQ(counts1.end.load(), 0u);
  // OpMode2 should be selected
  EXPECT_EQ(counts2.constructed.load(), 1u);
  EXPECT_EQ(counts2.disabledPeriodic.load(), 1u);

  robot.EndCompetition();
  robotThread.join();
}

}  // namespace
