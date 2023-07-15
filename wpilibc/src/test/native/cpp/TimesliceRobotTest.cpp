// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/TimesliceRobot.h"  // NOLINT(build/include_order)

#include <stdint.h>

#include <atomic>
#include <thread>

#include "frc/simulation/DriverStationSim.h"
#include "frc/simulation/SimHooks.h"
#include "gtest/gtest.h"

using namespace frc;

namespace {
class TimesliceRobotTest : public ::testing::Test {
 protected:
  void SetUp() override { frc::sim::PauseTiming(); }

  void TearDown() override { frc::sim::ResumeTiming(); }
};

class MockRobot : public TimesliceRobot {
 public:
  std::atomic<uint32_t> m_robotPeriodicCount{0};

  MockRobot() : TimesliceRobot{2_ms, 5_ms} {}

  void RobotPeriodic() override { m_robotPeriodicCount++; }
};
}  // namespace

TEST_F(TimesliceRobotTest, Schedule) {
  MockRobot robot;

  std::atomic<uint32_t> callbackCount1{0};
  std::atomic<uint32_t> callbackCount2{0};

  // Timeslice allocation table
  //
  // |       Name      | Offset (ms) | Allocation (ms)|
  // |-----------------|-------------|----------------|
  // | RobotPeriodic() |           0 |              2 |
  // | Callback 1      |           2 |            0.5 |
  // | Callback 2      |         2.5 |              1 |
  robot.Schedule([&] { callbackCount1++; }, 0.5_ms);
  robot.Schedule([&] { callbackCount2++; }, 1_ms);

  std::thread robotThread{[&] { robot.StartCompetition(); }};

  frc::sim::DriverStationSim::SetEnabled(false);
  frc::sim::DriverStationSim::NotifyNewData();
  frc::sim::StepTiming(0_ms);  // Wait for Notifiers

  // Functions scheduled with addPeriodic() are delayed by one period before
  // their first run (5 ms for this test's callbacks here and 20 ms for
  // robotPeriodic()).
  frc::sim::StepTiming(5_ms);

  EXPECT_EQ(0u, robot.m_robotPeriodicCount);
  EXPECT_EQ(0u, callbackCount1);
  EXPECT_EQ(0u, callbackCount2);

  // Step to 1.5 ms
  frc::sim::StepTiming(1.5_ms);
  EXPECT_EQ(0u, robot.m_robotPeriodicCount);
  EXPECT_EQ(0u, callbackCount1);
  EXPECT_EQ(0u, callbackCount2);

  // Step to 2.25 ms
  frc::sim::StepTiming(0.75_ms);
  EXPECT_EQ(0u, robot.m_robotPeriodicCount);
  EXPECT_EQ(1u, callbackCount1);
  EXPECT_EQ(0u, callbackCount2);

  // Step to 2.75 ms
  frc::sim::StepTiming(0.5_ms);
  EXPECT_EQ(0u, robot.m_robotPeriodicCount);
  EXPECT_EQ(1u, callbackCount1);
  EXPECT_EQ(1u, callbackCount2);

  robot.EndCompetition();
  robotThread.join();
}

TEST_F(TimesliceRobotTest, ScheduleOverrun) {
  MockRobot robot;

  robot.Schedule([] {}, 0.5_ms);
  robot.Schedule([] {}, 1_ms);

  // offset = 2 ms + 0.5 ms + 1 ms = 3.5 ms
  // 3.5 ms + 3 ms allocation = 6.5 ms > max of 5 ms
  EXPECT_THROW(robot.Schedule([] {}, 3_ms), std::runtime_error);

  robot.EndCompetition();
}
