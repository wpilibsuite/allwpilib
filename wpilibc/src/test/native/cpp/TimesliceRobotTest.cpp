// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/framework/TimesliceRobot.hpp"

#include <stdint.h>

#include <atomic>
#include <thread>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "wpi/simulation/DriverStationSim.hpp"
#include "wpi/simulation/SimHooks.hpp"

using namespace wpi;

namespace {
class TimesliceRobotTest {
 public:
  TimesliceRobotTest() {
    wpi::sim::PauseTiming();
    wpi::sim::SetProgramStarted(false);
  }

  ~TimesliceRobotTest() {
    wpi::sim::ResumeTiming();
    wpi::nt::ResetInstance(wpi::nt::GetDefaultInstance());
  }
};

class MockRobot : public TimesliceRobot {
 public:
  std::atomic<uint32_t> m_robotPeriodicCount{0};

  MockRobot() : TimesliceRobot{2_ms, 5_ms} {}

  void RobotPeriodic() override { m_robotPeriodicCount++; }
};
}  // namespace

TEST_CASE_METHOD(TimesliceRobotTest, "TimesliceRobotTest Schedule",
                 "[wpilibc]") {
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
  wpi::sim::WaitForProgramStart();

  wpi::sim::DriverStationSim::SetEnabled(false);
  wpi::sim::DriverStationSim::NotifyNewData();

  // Functions scheduled with addPeriodic() are delayed by one period before
  // their first run (5 ms for this test's callbacks here and 20 ms for
  // robotPeriodic()).
  wpi::sim::StepTiming(5_ms);

  CHECK(0u == robot.m_robotPeriodicCount);
  CHECK(0u == callbackCount1);
  CHECK(0u == callbackCount2);

  // Step to 1.5 ms
  wpi::sim::StepTiming(1.5_ms);
  CHECK(0u == robot.m_robotPeriodicCount);
  CHECK(0u == callbackCount1);
  CHECK(0u == callbackCount2);

  // Step to 2.25 ms
  wpi::sim::StepTiming(0.75_ms);
  CHECK(0u == robot.m_robotPeriodicCount);
  CHECK(1u == callbackCount1);
  CHECK(0u == callbackCount2);

  // Step to 2.75 ms
  wpi::sim::StepTiming(0.5_ms);
  CHECK(0u == robot.m_robotPeriodicCount);
  CHECK(1u == callbackCount1);
  CHECK(1u == callbackCount2);

  robot.EndCompetition();
  robotThread.join();
}

TEST_CASE_METHOD(TimesliceRobotTest, "TimesliceRobotTest ScheduleOverrun",
                 "[wpilibc]") {
  MockRobot robot;

  robot.Schedule([] {}, 0.5_ms);
  robot.Schedule([] {}, 1_ms);

  // offset = 2 ms + 0.5 ms + 1 ms = 3.5 ms
  // 3.5 ms + 3 ms allocation = 6.5 ms > max of 5 ms
  CHECK_THROWS_AS(robot.Schedule([] {}, 3_ms), std::runtime_error);

  robot.EndCompetition();
}
