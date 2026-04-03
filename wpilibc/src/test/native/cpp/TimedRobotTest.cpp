// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/framework/TimedRobot.hpp"

#include <stdint.h>

#include <atomic>
#include <thread>

#include <gtest/gtest.h>

#include "wpi/hal/DriverStationTypes.h"
#include "wpi/simulation/DriverStationSim.hpp"
#include "wpi/simulation/SimHooks.hpp"

using namespace wpi;

inline constexpr auto kPeriod = 20_ms;

namespace {
class TimedRobotTest : public ::testing::Test {
 protected:
  void SetUp() override {
    wpi::sim::PauseTiming();
    wpi::sim::SetProgramStarted(false);
  }

  void TearDown() override {
    wpi::sim::ResumeTiming();
    wpi::nt::ResetInstance(wpi::nt::GetDefaultInstance());
  }
};

class MockRobot : public TimedRobot {
 public:
  std::atomic<uint32_t> m_simulationInitCount{0};
  std::atomic<uint32_t> m_disabledEnterCount{0};
  std::atomic<uint32_t> m_autonomousEnterCount{0};
  std::atomic<uint32_t> m_teleopEnterCount{0};
  std::atomic<uint32_t> m_utilityEnterCount{0};

  std::atomic<uint32_t> m_disabledExitCount{0};
  std::atomic<uint32_t> m_autonomousExitCount{0};
  std::atomic<uint32_t> m_teleopExitCount{0};
  std::atomic<uint32_t> m_utilityExitCount{0};

  std::atomic<uint32_t> m_robotPeriodicCount{0};
  std::atomic<uint32_t> m_simulationPeriodicCount{0};
  std::atomic<uint32_t> m_disabledPeriodicCount{0};
  std::atomic<uint32_t> m_autonomousPeriodicCount{0};
  std::atomic<uint32_t> m_teleopPeriodicCount{0};
  std::atomic<uint32_t> m_utilityPeriodicCount{0};

  MockRobot() : TimedRobot{kPeriod} {}

  void SimulationInit() override { m_simulationInitCount++; }

  void DisabledEnter() override { m_disabledEnterCount++; }

  void AutonomousEnter() override { m_autonomousEnterCount++; }

  void TeleopEnter() override { m_teleopEnterCount++; }

  void UtilityEnter() override { m_utilityEnterCount++; }

  void RobotPeriodic() override { m_robotPeriodicCount++; }

  void SimulationPeriodic() override { m_simulationPeriodicCount++; }

  void DisabledPeriodic() override { m_disabledPeriodicCount++; }

  void AutonomousPeriodic() override { m_autonomousPeriodicCount++; }

  void TeleopPeriodic() override { m_teleopPeriodicCount++; }

  void UtilityPeriodic() override { m_utilityPeriodicCount++; }

  void DisabledExit() override { m_disabledExitCount++; }

  void AutonomousExit() override { m_autonomousExitCount++; }

  void TeleopExit() override { m_teleopExitCount++; }

  void UtilityExit() override { m_utilityExitCount++; }
};
}  // namespace

TEST_F(TimedRobotTest, DisabledMode) {
  MockRobot robot;

  std::thread robotThread{[&] { robot.StartCompetition(); }};
  wpi::sim::WaitForProgramStart();

  wpi::sim::DriverStationSim::SetEnabled(false);
  wpi::sim::DriverStationSim::NotifyNewData();

  EXPECT_EQ(1u, robot.m_simulationInitCount);
  EXPECT_EQ(0u, robot.m_disabledEnterCount);
  EXPECT_EQ(0u, robot.m_autonomousEnterCount);
  EXPECT_EQ(0u, robot.m_teleopEnterCount);
  EXPECT_EQ(0u, robot.m_utilityEnterCount);

  EXPECT_EQ(0u, robot.m_robotPeriodicCount);
  EXPECT_EQ(0u, robot.m_simulationPeriodicCount);
  EXPECT_EQ(0u, robot.m_disabledPeriodicCount);
  EXPECT_EQ(0u, robot.m_autonomousPeriodicCount);
  EXPECT_EQ(0u, robot.m_teleopPeriodicCount);
  EXPECT_EQ(0u, robot.m_utilityPeriodicCount);

  EXPECT_EQ(0u, robot.m_disabledExitCount);
  EXPECT_EQ(0u, robot.m_autonomousExitCount);
  EXPECT_EQ(0u, robot.m_teleopExitCount);
  EXPECT_EQ(0u, robot.m_utilityExitCount);

  wpi::sim::StepTiming(kPeriod);

  EXPECT_EQ(1u, robot.m_simulationInitCount);
  EXPECT_EQ(1u, robot.m_disabledEnterCount);
  EXPECT_EQ(0u, robot.m_autonomousEnterCount);
  EXPECT_EQ(0u, robot.m_teleopEnterCount);
  EXPECT_EQ(0u, robot.m_utilityEnterCount);

  EXPECT_EQ(1u, robot.m_robotPeriodicCount);
  EXPECT_EQ(1u, robot.m_simulationPeriodicCount);
  EXPECT_EQ(1u, robot.m_disabledPeriodicCount);
  EXPECT_EQ(0u, robot.m_autonomousPeriodicCount);
  EXPECT_EQ(0u, robot.m_teleopPeriodicCount);
  EXPECT_EQ(0u, robot.m_utilityPeriodicCount);

  EXPECT_EQ(0u, robot.m_disabledExitCount);
  EXPECT_EQ(0u, robot.m_autonomousExitCount);
  EXPECT_EQ(0u, robot.m_teleopExitCount);
  EXPECT_EQ(0u, robot.m_utilityExitCount);

  wpi::sim::StepTiming(kPeriod);

  EXPECT_EQ(1u, robot.m_simulationInitCount);
  EXPECT_EQ(1u, robot.m_disabledEnterCount);
  EXPECT_EQ(0u, robot.m_autonomousEnterCount);
  EXPECT_EQ(0u, robot.m_teleopEnterCount);
  EXPECT_EQ(0u, robot.m_utilityEnterCount);

  EXPECT_EQ(2u, robot.m_robotPeriodicCount);
  EXPECT_EQ(2u, robot.m_simulationPeriodicCount);
  EXPECT_EQ(2u, robot.m_disabledPeriodicCount);
  EXPECT_EQ(0u, robot.m_autonomousPeriodicCount);
  EXPECT_EQ(0u, robot.m_teleopPeriodicCount);
  EXPECT_EQ(0u, robot.m_utilityPeriodicCount);

  EXPECT_EQ(0u, robot.m_disabledExitCount);
  EXPECT_EQ(0u, robot.m_autonomousExitCount);
  EXPECT_EQ(0u, robot.m_teleopExitCount);
  EXPECT_EQ(0u, robot.m_utilityExitCount);

  robot.EndCompetition();
  robotThread.join();
}

TEST_F(TimedRobotTest, AutonomousMode) {
  MockRobot robot;

  std::thread robotThread{[&] { robot.StartCompetition(); }};
  wpi::sim::WaitForProgramStart();

  wpi::sim::DriverStationSim::SetEnabled(true);
  wpi::sim::DriverStationSim::SetRobotMode(hal::RobotMode::AUTONOMOUS);
  wpi::sim::DriverStationSim::NotifyNewData();

  EXPECT_EQ(1u, robot.m_simulationInitCount);
  EXPECT_EQ(0u, robot.m_disabledEnterCount);
  EXPECT_EQ(0u, robot.m_autonomousEnterCount);
  EXPECT_EQ(0u, robot.m_teleopEnterCount);
  EXPECT_EQ(0u, robot.m_utilityEnterCount);

  EXPECT_EQ(0u, robot.m_robotPeriodicCount);
  EXPECT_EQ(0u, robot.m_simulationPeriodicCount);
  EXPECT_EQ(0u, robot.m_disabledPeriodicCount);
  EXPECT_EQ(0u, robot.m_autonomousPeriodicCount);
  EXPECT_EQ(0u, robot.m_teleopPeriodicCount);
  EXPECT_EQ(0u, robot.m_utilityPeriodicCount);

  EXPECT_EQ(0u, robot.m_disabledExitCount);
  EXPECT_EQ(0u, robot.m_autonomousExitCount);
  EXPECT_EQ(0u, robot.m_teleopExitCount);
  EXPECT_EQ(0u, robot.m_utilityExitCount);

  wpi::sim::StepTiming(kPeriod);

  EXPECT_EQ(1u, robot.m_simulationInitCount);
  EXPECT_EQ(0u, robot.m_disabledEnterCount);
  EXPECT_EQ(1u, robot.m_autonomousEnterCount);
  EXPECT_EQ(0u, robot.m_teleopEnterCount);
  EXPECT_EQ(0u, robot.m_utilityEnterCount);

  EXPECT_EQ(1u, robot.m_robotPeriodicCount);
  EXPECT_EQ(1u, robot.m_simulationPeriodicCount);
  EXPECT_EQ(0u, robot.m_disabledPeriodicCount);
  EXPECT_EQ(1u, robot.m_autonomousPeriodicCount);
  EXPECT_EQ(0u, robot.m_teleopPeriodicCount);
  EXPECT_EQ(0u, robot.m_utilityPeriodicCount);

  EXPECT_EQ(0u, robot.m_disabledExitCount);
  EXPECT_EQ(0u, robot.m_autonomousExitCount);
  EXPECT_EQ(0u, robot.m_teleopExitCount);
  EXPECT_EQ(0u, robot.m_utilityExitCount);

  wpi::sim::StepTiming(kPeriod);

  EXPECT_EQ(1u, robot.m_simulationInitCount);
  EXPECT_EQ(0u, robot.m_disabledEnterCount);
  EXPECT_EQ(1u, robot.m_autonomousEnterCount);
  EXPECT_EQ(0u, robot.m_teleopEnterCount);
  EXPECT_EQ(0u, robot.m_utilityEnterCount);

  EXPECT_EQ(2u, robot.m_robotPeriodicCount);
  EXPECT_EQ(2u, robot.m_simulationPeriodicCount);
  EXPECT_EQ(0u, robot.m_disabledPeriodicCount);
  EXPECT_EQ(2u, robot.m_autonomousPeriodicCount);
  EXPECT_EQ(0u, robot.m_teleopPeriodicCount);
  EXPECT_EQ(0u, robot.m_utilityPeriodicCount);

  EXPECT_EQ(0u, robot.m_disabledExitCount);
  EXPECT_EQ(0u, robot.m_autonomousExitCount);
  EXPECT_EQ(0u, robot.m_teleopExitCount);
  EXPECT_EQ(0u, robot.m_utilityExitCount);

  robot.EndCompetition();
  robotThread.join();
}

TEST_F(TimedRobotTest, TeleopMode) {
  MockRobot robot;

  std::thread robotThread{[&] { robot.StartCompetition(); }};
  wpi::sim::WaitForProgramStart();

  wpi::sim::DriverStationSim::SetEnabled(true);
  wpi::sim::DriverStationSim::SetRobotMode(hal::RobotMode::TELEOPERATED);
  wpi::sim::DriverStationSim::NotifyNewData();

  EXPECT_EQ(1u, robot.m_simulationInitCount);
  EXPECT_EQ(0u, robot.m_disabledEnterCount);
  EXPECT_EQ(0u, robot.m_autonomousEnterCount);
  EXPECT_EQ(0u, robot.m_teleopEnterCount);
  EXPECT_EQ(0u, robot.m_utilityEnterCount);

  EXPECT_EQ(0u, robot.m_robotPeriodicCount);
  EXPECT_EQ(0u, robot.m_simulationPeriodicCount);
  EXPECT_EQ(0u, robot.m_disabledPeriodicCount);
  EXPECT_EQ(0u, robot.m_autonomousPeriodicCount);
  EXPECT_EQ(0u, robot.m_teleopPeriodicCount);
  EXPECT_EQ(0u, robot.m_utilityPeriodicCount);

  EXPECT_EQ(0u, robot.m_disabledExitCount);
  EXPECT_EQ(0u, robot.m_autonomousExitCount);
  EXPECT_EQ(0u, robot.m_teleopExitCount);
  EXPECT_EQ(0u, robot.m_utilityExitCount);

  wpi::sim::StepTiming(kPeriod);

  EXPECT_EQ(1u, robot.m_simulationInitCount);
  EXPECT_EQ(0u, robot.m_disabledEnterCount);
  EXPECT_EQ(0u, robot.m_autonomousEnterCount);
  EXPECT_EQ(1u, robot.m_teleopEnterCount);
  EXPECT_EQ(0u, robot.m_utilityEnterCount);

  EXPECT_EQ(1u, robot.m_robotPeriodicCount);
  EXPECT_EQ(1u, robot.m_simulationPeriodicCount);
  EXPECT_EQ(0u, robot.m_disabledPeriodicCount);
  EXPECT_EQ(0u, robot.m_autonomousPeriodicCount);
  EXPECT_EQ(1u, robot.m_teleopPeriodicCount);
  EXPECT_EQ(0u, robot.m_utilityPeriodicCount);

  EXPECT_EQ(0u, robot.m_disabledExitCount);
  EXPECT_EQ(0u, robot.m_autonomousExitCount);
  EXPECT_EQ(0u, robot.m_teleopExitCount);
  EXPECT_EQ(0u, robot.m_utilityExitCount);

  wpi::sim::StepTiming(kPeriod);

  EXPECT_EQ(1u, robot.m_simulationInitCount);
  EXPECT_EQ(0u, robot.m_disabledEnterCount);
  EXPECT_EQ(0u, robot.m_autonomousEnterCount);
  EXPECT_EQ(1u, robot.m_teleopEnterCount);
  EXPECT_EQ(0u, robot.m_utilityEnterCount);

  EXPECT_EQ(2u, robot.m_robotPeriodicCount);
  EXPECT_EQ(2u, robot.m_simulationPeriodicCount);
  EXPECT_EQ(0u, robot.m_disabledPeriodicCount);
  EXPECT_EQ(0u, robot.m_autonomousPeriodicCount);
  EXPECT_EQ(2u, robot.m_teleopPeriodicCount);
  EXPECT_EQ(0u, robot.m_utilityPeriodicCount);

  EXPECT_EQ(0u, robot.m_disabledExitCount);
  EXPECT_EQ(0u, robot.m_autonomousExitCount);
  EXPECT_EQ(0u, robot.m_teleopExitCount);
  EXPECT_EQ(0u, robot.m_utilityExitCount);

  robot.EndCompetition();
  robotThread.join();
}

TEST_F(TimedRobotTest, UtilityMode) {
  MockRobot robot;
  std::thread robotThread{[&] { robot.StartCompetition(); }};
  wpi::sim::WaitForProgramStart();

  wpi::sim::DriverStationSim::SetEnabled(true);
  wpi::sim::DriverStationSim::SetRobotMode(hal::RobotMode::UTILITY);
  wpi::sim::DriverStationSim::NotifyNewData();

  EXPECT_EQ(1u, robot.m_simulationInitCount);
  EXPECT_EQ(0u, robot.m_disabledEnterCount);
  EXPECT_EQ(0u, robot.m_autonomousEnterCount);
  EXPECT_EQ(0u, robot.m_teleopEnterCount);
  EXPECT_EQ(0u, robot.m_utilityEnterCount);

  EXPECT_EQ(0u, robot.m_robotPeriodicCount);
  EXPECT_EQ(0u, robot.m_simulationPeriodicCount);
  EXPECT_EQ(0u, robot.m_disabledPeriodicCount);
  EXPECT_EQ(0u, robot.m_autonomousPeriodicCount);
  EXPECT_EQ(0u, robot.m_teleopPeriodicCount);
  EXPECT_EQ(0u, robot.m_utilityPeriodicCount);

  EXPECT_EQ(0u, robot.m_disabledExitCount);
  EXPECT_EQ(0u, robot.m_autonomousExitCount);
  EXPECT_EQ(0u, robot.m_teleopExitCount);
  EXPECT_EQ(0u, robot.m_utilityExitCount);

  wpi::sim::StepTiming(kPeriod);

  EXPECT_EQ(1u, robot.m_simulationInitCount);
  EXPECT_EQ(0u, robot.m_disabledEnterCount);
  EXPECT_EQ(0u, robot.m_autonomousEnterCount);
  EXPECT_EQ(0u, robot.m_teleopEnterCount);
  EXPECT_EQ(1u, robot.m_utilityEnterCount);

  EXPECT_EQ(1u, robot.m_robotPeriodicCount);
  EXPECT_EQ(1u, robot.m_simulationPeriodicCount);
  EXPECT_EQ(0u, robot.m_disabledPeriodicCount);
  EXPECT_EQ(0u, robot.m_autonomousPeriodicCount);
  EXPECT_EQ(0u, robot.m_teleopPeriodicCount);
  EXPECT_EQ(1u, robot.m_utilityPeriodicCount);

  EXPECT_EQ(0u, robot.m_disabledExitCount);
  EXPECT_EQ(0u, robot.m_autonomousExitCount);
  EXPECT_EQ(0u, robot.m_teleopExitCount);
  EXPECT_EQ(0u, robot.m_utilityExitCount);

  wpi::sim::StepTiming(kPeriod);

  EXPECT_EQ(1u, robot.m_simulationInitCount);
  EXPECT_EQ(0u, robot.m_disabledEnterCount);
  EXPECT_EQ(0u, robot.m_autonomousEnterCount);
  EXPECT_EQ(0u, robot.m_teleopEnterCount);
  EXPECT_EQ(1u, robot.m_utilityEnterCount);

  EXPECT_EQ(2u, robot.m_robotPeriodicCount);
  EXPECT_EQ(2u, robot.m_simulationPeriodicCount);
  EXPECT_EQ(0u, robot.m_disabledPeriodicCount);
  EXPECT_EQ(0u, robot.m_autonomousPeriodicCount);
  EXPECT_EQ(0u, robot.m_teleopPeriodicCount);
  EXPECT_EQ(2u, robot.m_utilityPeriodicCount);

  EXPECT_EQ(0u, robot.m_disabledExitCount);
  EXPECT_EQ(0u, robot.m_autonomousExitCount);
  EXPECT_EQ(0u, robot.m_teleopExitCount);
  EXPECT_EQ(0u, robot.m_utilityExitCount);

  wpi::sim::DriverStationSim::SetEnabled(false);
  wpi::sim::DriverStationSim::NotifyNewData();
  wpi::sim::StepTiming(20_ms);  // Wait for Notifiers

  EXPECT_EQ(1u, robot.m_simulationInitCount);
  EXPECT_EQ(1u, robot.m_disabledEnterCount);
  EXPECT_EQ(0u, robot.m_autonomousEnterCount);
  EXPECT_EQ(0u, robot.m_teleopEnterCount);
  EXPECT_EQ(1u, robot.m_utilityEnterCount);

  EXPECT_EQ(3u, robot.m_robotPeriodicCount);
  EXPECT_EQ(3u, robot.m_simulationPeriodicCount);
  EXPECT_EQ(1u, robot.m_disabledPeriodicCount);
  EXPECT_EQ(0u, robot.m_autonomousPeriodicCount);
  EXPECT_EQ(0u, robot.m_teleopPeriodicCount);
  EXPECT_EQ(2u, robot.m_utilityPeriodicCount);

  EXPECT_EQ(0u, robot.m_disabledExitCount);
  EXPECT_EQ(0u, robot.m_autonomousExitCount);
  EXPECT_EQ(0u, robot.m_teleopExitCount);
  EXPECT_EQ(1u, robot.m_utilityExitCount);

  robot.EndCompetition();
  robotThread.join();
}

TEST_F(TimedRobotTest, ModeChange) {
  MockRobot robot;

  std::thread robotThread{[&] { robot.StartCompetition(); }};
  wpi::sim::WaitForProgramStart();

  // Start in disabled
  wpi::sim::DriverStationSim::SetEnabled(false);
  wpi::sim::DriverStationSim::NotifyNewData();

  EXPECT_EQ(0u, robot.m_disabledEnterCount);
  EXPECT_EQ(0u, robot.m_autonomousEnterCount);
  EXPECT_EQ(0u, robot.m_teleopEnterCount);
  EXPECT_EQ(0u, robot.m_utilityEnterCount);

  EXPECT_EQ(0u, robot.m_disabledExitCount);
  EXPECT_EQ(0u, robot.m_autonomousExitCount);
  EXPECT_EQ(0u, robot.m_teleopExitCount);
  EXPECT_EQ(0u, robot.m_utilityExitCount);

  wpi::sim::StepTiming(kPeriod);

  EXPECT_EQ(1u, robot.m_disabledEnterCount);
  EXPECT_EQ(0u, robot.m_autonomousEnterCount);
  EXPECT_EQ(0u, robot.m_teleopEnterCount);
  EXPECT_EQ(0u, robot.m_utilityEnterCount);

  EXPECT_EQ(0u, robot.m_disabledExitCount);
  EXPECT_EQ(0u, robot.m_autonomousExitCount);
  EXPECT_EQ(0u, robot.m_teleopExitCount);
  EXPECT_EQ(0u, robot.m_utilityExitCount);

  // Transition to autonomous
  wpi::sim::DriverStationSim::SetEnabled(true);
  wpi::sim::DriverStationSim::SetRobotMode(hal::RobotMode::AUTONOMOUS);
  wpi::sim::DriverStationSim::NotifyNewData();

  wpi::sim::StepTiming(kPeriod);

  EXPECT_EQ(1u, robot.m_disabledEnterCount);
  EXPECT_EQ(1u, robot.m_autonomousEnterCount);
  EXPECT_EQ(0u, robot.m_teleopEnterCount);
  EXPECT_EQ(0u, robot.m_utilityEnterCount);

  EXPECT_EQ(1u, robot.m_disabledExitCount);
  EXPECT_EQ(0u, robot.m_autonomousExitCount);
  EXPECT_EQ(0u, robot.m_teleopExitCount);
  EXPECT_EQ(0u, robot.m_utilityExitCount);

  // Transition to teleop
  wpi::sim::DriverStationSim::SetEnabled(true);
  wpi::sim::DriverStationSim::SetRobotMode(hal::RobotMode::TELEOPERATED);
  wpi::sim::DriverStationSim::NotifyNewData();

  wpi::sim::StepTiming(kPeriod);

  EXPECT_EQ(1u, robot.m_disabledEnterCount);
  EXPECT_EQ(1u, robot.m_autonomousEnterCount);
  EXPECT_EQ(1u, robot.m_teleopEnterCount);
  EXPECT_EQ(0u, robot.m_utilityEnterCount);

  EXPECT_EQ(1u, robot.m_disabledExitCount);
  EXPECT_EQ(1u, robot.m_autonomousExitCount);
  EXPECT_EQ(0u, robot.m_teleopExitCount);
  EXPECT_EQ(0u, robot.m_utilityExitCount);

  // Transition to utility
  wpi::sim::DriverStationSim::SetEnabled(true);
  wpi::sim::DriverStationSim::SetRobotMode(hal::RobotMode::UTILITY);
  wpi::sim::DriverStationSim::NotifyNewData();

  wpi::sim::StepTiming(kPeriod);

  EXPECT_EQ(1u, robot.m_disabledEnterCount);
  EXPECT_EQ(1u, robot.m_autonomousEnterCount);
  EXPECT_EQ(1u, robot.m_teleopEnterCount);
  EXPECT_EQ(1u, robot.m_utilityEnterCount);

  EXPECT_EQ(1u, robot.m_disabledExitCount);
  EXPECT_EQ(1u, robot.m_autonomousExitCount);
  EXPECT_EQ(1u, robot.m_teleopExitCount);
  EXPECT_EQ(0u, robot.m_utilityExitCount);

  // Transition to disabled
  wpi::sim::DriverStationSim::SetEnabled(false);
  wpi::sim::DriverStationSim::NotifyNewData();

  wpi::sim::StepTiming(kPeriod);

  EXPECT_EQ(2u, robot.m_disabledEnterCount);
  EXPECT_EQ(1u, robot.m_autonomousEnterCount);
  EXPECT_EQ(1u, robot.m_teleopEnterCount);
  EXPECT_EQ(1u, robot.m_utilityEnterCount);

  EXPECT_EQ(1u, robot.m_disabledExitCount);
  EXPECT_EQ(1u, robot.m_autonomousExitCount);
  EXPECT_EQ(1u, robot.m_teleopExitCount);
  EXPECT_EQ(1u, robot.m_utilityExitCount);

  robot.EndCompetition();
  robotThread.join();
}

TEST_F(TimedRobotTest, AddPeriodic) {
  MockRobot robot;

  std::atomic<uint32_t> callbackCount{0};
  robot.AddPeriodic([&] { callbackCount++; }, kPeriod / 2.0);

  std::thread robotThread{[&] { robot.StartCompetition(); }};
  wpi::sim::WaitForProgramStart();

  wpi::sim::DriverStationSim::SetEnabled(false);
  wpi::sim::DriverStationSim::NotifyNewData();

  EXPECT_EQ(0u, robot.m_disabledEnterCount);
  EXPECT_EQ(0u, robot.m_disabledPeriodicCount);
  EXPECT_EQ(0u, callbackCount);

  wpi::sim::StepTiming(kPeriod / 2.0);

  EXPECT_EQ(0u, robot.m_disabledEnterCount);
  EXPECT_EQ(0u, robot.m_disabledPeriodicCount);
  EXPECT_EQ(1u, callbackCount);

  wpi::sim::StepTiming(kPeriod / 2.0);

  EXPECT_EQ(1u, robot.m_disabledEnterCount);
  EXPECT_EQ(1u, robot.m_disabledPeriodicCount);
  EXPECT_EQ(2u, callbackCount);

  robot.EndCompetition();
  robotThread.join();
}

TEST_F(TimedRobotTest, AddPeriodicWithOffset) {
  MockRobot robot;

  std::atomic<uint32_t> callbackCount{0};
  robot.AddPeriodic([&] { callbackCount++; }, kPeriod / 2.0, kPeriod / 4.0);

  // Expirations in this test (ms)
  //
  // Robot | Callback
  // ================
  //     p |    0.75p
  //    2p |    1.25p

  std::thread robotThread{[&] { robot.StartCompetition(); }};
  wpi::sim::WaitForProgramStart();

  wpi::sim::DriverStationSim::SetEnabled(false);
  wpi::sim::DriverStationSim::NotifyNewData();

  EXPECT_EQ(0u, robot.m_disabledEnterCount);
  EXPECT_EQ(0u, robot.m_disabledPeriodicCount);
  EXPECT_EQ(0u, callbackCount);

  wpi::sim::StepTiming(kPeriod * 3.0 / 8.0);

  EXPECT_EQ(0u, robot.m_disabledEnterCount);
  EXPECT_EQ(0u, robot.m_disabledPeriodicCount);
  EXPECT_EQ(0u, callbackCount);

  wpi::sim::StepTiming(kPeriod * 3.0 / 8.0);

  EXPECT_EQ(0u, robot.m_disabledEnterCount);
  EXPECT_EQ(0u, robot.m_disabledPeriodicCount);
  EXPECT_EQ(1u, callbackCount);

  wpi::sim::StepTiming(kPeriod / 4.0);

  EXPECT_EQ(1u, robot.m_disabledEnterCount);
  EXPECT_EQ(1u, robot.m_disabledPeriodicCount);
  EXPECT_EQ(1u, callbackCount);

  wpi::sim::StepTiming(kPeriod / 4.0);

  EXPECT_EQ(1u, robot.m_disabledEnterCount);
  EXPECT_EQ(1u, robot.m_disabledPeriodicCount);
  EXPECT_EQ(2u, callbackCount);

  robot.EndCompetition();
  robotThread.join();
}
