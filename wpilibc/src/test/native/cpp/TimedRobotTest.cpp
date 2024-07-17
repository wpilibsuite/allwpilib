// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/TimedRobot.h"  // NOLINT(build/include_order)

#include <stdint.h>

#include <atomic>
#include <thread>

#include <gtest/gtest.h>

#include "frc/livewindow/LiveWindow.h"
#include "frc/simulation/DriverStationSim.h"
#include "frc/simulation/SimHooks.h"

using namespace frc;

inline constexpr auto kPeriod = 20_ms;

namespace {
class TimedRobotTest : public ::testing::TestWithParam<bool> {
 protected:
  void SetUp() override { frc::sim::PauseTiming(); }

  void TearDown() override { frc::sim::ResumeTiming(); }
};

class MockRobot : public TimedRobot {
 public:
  std::atomic<uint32_t> m_robotInitCount{0};
  std::atomic<uint32_t> m_simulationInitCount{0};
  std::atomic<uint32_t> m_disabledInitCount{0};
  std::atomic<uint32_t> m_autonomousInitCount{0};
  std::atomic<uint32_t> m_teleopInitCount{0};
  std::atomic<uint32_t> m_testInitCount{0};

  std::atomic<uint32_t> m_disabledExitCount{0};
  std::atomic<uint32_t> m_autonomousExitCount{0};
  std::atomic<uint32_t> m_teleopExitCount{0};
  std::atomic<uint32_t> m_testExitCount{0};

  std::atomic<uint32_t> m_robotPeriodicCount{0};
  std::atomic<uint32_t> m_simulationPeriodicCount{0};
  std::atomic<uint32_t> m_disabledPeriodicCount{0};
  std::atomic<uint32_t> m_autonomousPeriodicCount{0};
  std::atomic<uint32_t> m_teleopPeriodicCount{0};
  std::atomic<uint32_t> m_testPeriodicCount{0};

  MockRobot() : TimedRobot{kPeriod} { m_robotInitCount++; }

  void SimulationInit() override { m_simulationInitCount++; }

  void DisabledInit() override { m_disabledInitCount++; }

  void AutonomousInit() override { m_autonomousInitCount++; }

  void TeleopInit() override { m_teleopInitCount++; }

  void TestInit() override { m_testInitCount++; }

  void RobotPeriodic() override { m_robotPeriodicCount++; }

  void SimulationPeriodic() override { m_simulationPeriodicCount++; }

  void DisabledPeriodic() override { m_disabledPeriodicCount++; }

  void AutonomousPeriodic() override { m_autonomousPeriodicCount++; }

  void TeleopPeriodic() override { m_teleopPeriodicCount++; }

  void TestPeriodic() override { m_testPeriodicCount++; }

  void DisabledExit() override { m_disabledExitCount++; }

  void AutonomousExit() override { m_autonomousExitCount++; }

  void TeleopExit() override { m_teleopExitCount++; }

  void TestExit() override { m_testExitCount++; }
};
}  // namespace

TEST_F(TimedRobotTest, DisabledMode) {
  MockRobot robot;

  std::thread robotThread{[&] { robot.StartCompetition(); }};

  frc::sim::DriverStationSim::SetEnabled(false);
  frc::sim::DriverStationSim::NotifyNewData();
  frc::sim::StepTiming(0_ms);  // Wait for Notifiers

  EXPECT_EQ(1u, robot.m_robotInitCount);
  EXPECT_EQ(1u, robot.m_simulationInitCount);
  EXPECT_EQ(0u, robot.m_disabledInitCount);
  EXPECT_EQ(0u, robot.m_autonomousInitCount);
  EXPECT_EQ(0u, robot.m_teleopInitCount);
  EXPECT_EQ(0u, robot.m_testInitCount);

  EXPECT_EQ(0u, robot.m_robotPeriodicCount);
  EXPECT_EQ(0u, robot.m_simulationPeriodicCount);
  EXPECT_EQ(0u, robot.m_disabledPeriodicCount);
  EXPECT_EQ(0u, robot.m_autonomousPeriodicCount);
  EXPECT_EQ(0u, robot.m_teleopPeriodicCount);
  EXPECT_EQ(0u, robot.m_testPeriodicCount);

  EXPECT_EQ(0u, robot.m_disabledExitCount);
  EXPECT_EQ(0u, robot.m_autonomousExitCount);
  EXPECT_EQ(0u, robot.m_teleopExitCount);
  EXPECT_EQ(0u, robot.m_testExitCount);

  frc::sim::StepTiming(kPeriod);

  EXPECT_EQ(1u, robot.m_robotInitCount);
  EXPECT_EQ(1u, robot.m_simulationInitCount);
  EXPECT_EQ(1u, robot.m_disabledInitCount);
  EXPECT_EQ(0u, robot.m_autonomousInitCount);
  EXPECT_EQ(0u, robot.m_teleopInitCount);
  EXPECT_EQ(0u, robot.m_testInitCount);

  EXPECT_EQ(1u, robot.m_robotPeriodicCount);
  EXPECT_EQ(1u, robot.m_simulationPeriodicCount);
  EXPECT_EQ(1u, robot.m_disabledPeriodicCount);
  EXPECT_EQ(0u, robot.m_autonomousPeriodicCount);
  EXPECT_EQ(0u, robot.m_teleopPeriodicCount);
  EXPECT_EQ(0u, robot.m_testPeriodicCount);

  EXPECT_EQ(0u, robot.m_disabledExitCount);
  EXPECT_EQ(0u, robot.m_autonomousExitCount);
  EXPECT_EQ(0u, robot.m_teleopExitCount);
  EXPECT_EQ(0u, robot.m_testExitCount);

  frc::sim::StepTiming(kPeriod);

  EXPECT_EQ(1u, robot.m_robotInitCount);
  EXPECT_EQ(1u, robot.m_simulationInitCount);
  EXPECT_EQ(1u, robot.m_disabledInitCount);
  EXPECT_EQ(0u, robot.m_autonomousInitCount);
  EXPECT_EQ(0u, robot.m_teleopInitCount);
  EXPECT_EQ(0u, robot.m_testInitCount);

  EXPECT_EQ(2u, robot.m_robotPeriodicCount);
  EXPECT_EQ(2u, robot.m_simulationPeriodicCount);
  EXPECT_EQ(2u, robot.m_disabledPeriodicCount);
  EXPECT_EQ(0u, robot.m_autonomousPeriodicCount);
  EXPECT_EQ(0u, robot.m_teleopPeriodicCount);
  EXPECT_EQ(0u, robot.m_testPeriodicCount);

  EXPECT_EQ(0u, robot.m_disabledExitCount);
  EXPECT_EQ(0u, robot.m_autonomousExitCount);
  EXPECT_EQ(0u, robot.m_teleopExitCount);
  EXPECT_EQ(0u, robot.m_testExitCount);

  robot.EndCompetition();
  robotThread.join();
}

TEST_F(TimedRobotTest, AutonomousMode) {
  MockRobot robot;

  std::thread robotThread{[&] { robot.StartCompetition(); }};

  frc::sim::DriverStationSim::SetEnabled(true);
  frc::sim::DriverStationSim::SetAutonomous(true);
  frc::sim::DriverStationSim::SetTest(false);
  frc::sim::DriverStationSim::NotifyNewData();
  frc::sim::StepTiming(0_ms);  // Wait for Notifiers

  EXPECT_EQ(1u, robot.m_robotInitCount);
  EXPECT_EQ(1u, robot.m_simulationInitCount);
  EXPECT_EQ(0u, robot.m_disabledInitCount);
  EXPECT_EQ(0u, robot.m_autonomousInitCount);
  EXPECT_EQ(0u, robot.m_teleopInitCount);
  EXPECT_EQ(0u, robot.m_testInitCount);

  EXPECT_EQ(0u, robot.m_robotPeriodicCount);
  EXPECT_EQ(0u, robot.m_simulationPeriodicCount);
  EXPECT_EQ(0u, robot.m_disabledPeriodicCount);
  EXPECT_EQ(0u, robot.m_autonomousPeriodicCount);
  EXPECT_EQ(0u, robot.m_teleopPeriodicCount);
  EXPECT_EQ(0u, robot.m_testPeriodicCount);

  EXPECT_EQ(0u, robot.m_disabledExitCount);
  EXPECT_EQ(0u, robot.m_autonomousExitCount);
  EXPECT_EQ(0u, robot.m_teleopExitCount);
  EXPECT_EQ(0u, robot.m_testExitCount);

  frc::sim::StepTiming(kPeriod);

  EXPECT_EQ(1u, robot.m_robotInitCount);
  EXPECT_EQ(1u, robot.m_simulationInitCount);
  EXPECT_EQ(0u, robot.m_disabledInitCount);
  EXPECT_EQ(1u, robot.m_autonomousInitCount);
  EXPECT_EQ(0u, robot.m_teleopInitCount);
  EXPECT_EQ(0u, robot.m_testInitCount);

  EXPECT_EQ(1u, robot.m_robotPeriodicCount);
  EXPECT_EQ(1u, robot.m_simulationPeriodicCount);
  EXPECT_EQ(0u, robot.m_disabledPeriodicCount);
  EXPECT_EQ(1u, robot.m_autonomousPeriodicCount);
  EXPECT_EQ(0u, robot.m_teleopPeriodicCount);
  EXPECT_EQ(0u, robot.m_testPeriodicCount);

  EXPECT_EQ(0u, robot.m_disabledExitCount);
  EXPECT_EQ(0u, robot.m_autonomousExitCount);
  EXPECT_EQ(0u, robot.m_teleopExitCount);
  EXPECT_EQ(0u, robot.m_testExitCount);

  frc::sim::StepTiming(kPeriod);

  EXPECT_EQ(1u, robot.m_robotInitCount);
  EXPECT_EQ(1u, robot.m_simulationInitCount);
  EXPECT_EQ(0u, robot.m_disabledInitCount);
  EXPECT_EQ(1u, robot.m_autonomousInitCount);
  EXPECT_EQ(0u, robot.m_teleopInitCount);
  EXPECT_EQ(0u, robot.m_testInitCount);

  EXPECT_EQ(2u, robot.m_robotPeriodicCount);
  EXPECT_EQ(2u, robot.m_simulationPeriodicCount);
  EXPECT_EQ(0u, robot.m_disabledPeriodicCount);
  EXPECT_EQ(2u, robot.m_autonomousPeriodicCount);
  EXPECT_EQ(0u, robot.m_teleopPeriodicCount);
  EXPECT_EQ(0u, robot.m_testPeriodicCount);

  EXPECT_EQ(0u, robot.m_disabledExitCount);
  EXPECT_EQ(0u, robot.m_autonomousExitCount);
  EXPECT_EQ(0u, robot.m_teleopExitCount);
  EXPECT_EQ(0u, robot.m_testExitCount);

  robot.EndCompetition();
  robotThread.join();
}

TEST_F(TimedRobotTest, TeleopMode) {
  MockRobot robot;

  std::thread robotThread{[&] { robot.StartCompetition(); }};

  frc::sim::DriverStationSim::SetEnabled(true);
  frc::sim::DriverStationSim::SetAutonomous(false);
  frc::sim::DriverStationSim::SetTest(false);
  frc::sim::DriverStationSim::NotifyNewData();
  frc::sim::StepTiming(0_ms);  // Wait for Notifiers

  EXPECT_EQ(1u, robot.m_robotInitCount);
  EXPECT_EQ(1u, robot.m_simulationInitCount);
  EXPECT_EQ(0u, robot.m_disabledInitCount);
  EXPECT_EQ(0u, robot.m_autonomousInitCount);
  EXPECT_EQ(0u, robot.m_teleopInitCount);
  EXPECT_EQ(0u, robot.m_testInitCount);

  EXPECT_EQ(0u, robot.m_robotPeriodicCount);
  EXPECT_EQ(0u, robot.m_simulationPeriodicCount);
  EXPECT_EQ(0u, robot.m_disabledPeriodicCount);
  EXPECT_EQ(0u, robot.m_autonomousPeriodicCount);
  EXPECT_EQ(0u, robot.m_teleopPeriodicCount);
  EXPECT_EQ(0u, robot.m_testPeriodicCount);

  EXPECT_EQ(0u, robot.m_disabledExitCount);
  EXPECT_EQ(0u, robot.m_autonomousExitCount);
  EXPECT_EQ(0u, robot.m_teleopExitCount);
  EXPECT_EQ(0u, robot.m_testExitCount);

  frc::sim::StepTiming(kPeriod);

  EXPECT_EQ(1u, robot.m_robotInitCount);
  EXPECT_EQ(1u, robot.m_simulationInitCount);
  EXPECT_EQ(0u, robot.m_disabledInitCount);
  EXPECT_EQ(0u, robot.m_autonomousInitCount);
  EXPECT_EQ(1u, robot.m_teleopInitCount);
  EXPECT_EQ(0u, robot.m_testInitCount);

  EXPECT_EQ(1u, robot.m_robotPeriodicCount);
  EXPECT_EQ(1u, robot.m_simulationPeriodicCount);
  EXPECT_EQ(0u, robot.m_disabledPeriodicCount);
  EXPECT_EQ(0u, robot.m_autonomousPeriodicCount);
  EXPECT_EQ(1u, robot.m_teleopPeriodicCount);
  EXPECT_EQ(0u, robot.m_testPeriodicCount);

  EXPECT_EQ(0u, robot.m_disabledExitCount);
  EXPECT_EQ(0u, robot.m_autonomousExitCount);
  EXPECT_EQ(0u, robot.m_teleopExitCount);
  EXPECT_EQ(0u, robot.m_testExitCount);

  frc::sim::StepTiming(kPeriod);

  EXPECT_EQ(1u, robot.m_robotInitCount);
  EXPECT_EQ(1u, robot.m_simulationInitCount);
  EXPECT_EQ(0u, robot.m_disabledInitCount);
  EXPECT_EQ(0u, robot.m_autonomousInitCount);
  EXPECT_EQ(1u, robot.m_teleopInitCount);
  EXPECT_EQ(0u, robot.m_testInitCount);

  EXPECT_EQ(2u, robot.m_robotPeriodicCount);
  EXPECT_EQ(2u, robot.m_simulationPeriodicCount);
  EXPECT_EQ(0u, robot.m_disabledPeriodicCount);
  EXPECT_EQ(0u, robot.m_autonomousPeriodicCount);
  EXPECT_EQ(2u, robot.m_teleopPeriodicCount);
  EXPECT_EQ(0u, robot.m_testPeriodicCount);

  EXPECT_EQ(0u, robot.m_disabledExitCount);
  EXPECT_EQ(0u, robot.m_autonomousExitCount);
  EXPECT_EQ(0u, robot.m_teleopExitCount);
  EXPECT_EQ(0u, robot.m_testExitCount);

  robot.EndCompetition();
  robotThread.join();
}

TEST_P(TimedRobotTest, TestMode) {
  bool isTestLW = GetParam();

  MockRobot robot;
  robot.EnableLiveWindowInTest(isTestLW);

  std::thread robotThread{[&] { robot.StartCompetition(); }};

  frc::sim::DriverStationSim::SetEnabled(true);
  frc::sim::DriverStationSim::SetAutonomous(false);
  frc::sim::DriverStationSim::SetTest(true);
  frc::sim::DriverStationSim::NotifyNewData();
  frc::sim::StepTiming(0_ms);  // Wait for Notifiers

  EXPECT_EQ(1u, robot.m_robotInitCount);
  EXPECT_EQ(1u, robot.m_simulationInitCount);
  EXPECT_EQ(0u, robot.m_disabledInitCount);
  EXPECT_EQ(0u, robot.m_autonomousInitCount);
  EXPECT_EQ(0u, robot.m_teleopInitCount);
  EXPECT_EQ(0u, robot.m_testInitCount);
  EXPECT_FALSE(frc::LiveWindow::IsEnabled());

  EXPECT_EQ(0u, robot.m_robotPeriodicCount);
  EXPECT_EQ(0u, robot.m_simulationPeriodicCount);
  EXPECT_EQ(0u, robot.m_disabledPeriodicCount);
  EXPECT_EQ(0u, robot.m_autonomousPeriodicCount);
  EXPECT_EQ(0u, robot.m_teleopPeriodicCount);
  EXPECT_EQ(0u, robot.m_testPeriodicCount);

  EXPECT_EQ(0u, robot.m_disabledExitCount);
  EXPECT_EQ(0u, robot.m_autonomousExitCount);
  EXPECT_EQ(0u, robot.m_teleopExitCount);
  EXPECT_EQ(0u, robot.m_testExitCount);

  frc::sim::StepTiming(kPeriod);

  EXPECT_EQ(1u, robot.m_robotInitCount);
  EXPECT_EQ(1u, robot.m_simulationInitCount);
  EXPECT_EQ(0u, robot.m_disabledInitCount);
  EXPECT_EQ(0u, robot.m_autonomousInitCount);
  EXPECT_EQ(0u, robot.m_teleopInitCount);
  EXPECT_EQ(1u, robot.m_testInitCount);
  EXPECT_EQ(isTestLW, frc::LiveWindow::IsEnabled());

  EXPECT_THROW(robot.EnableLiveWindowInTest(isTestLW), std::runtime_error);

  EXPECT_EQ(1u, robot.m_robotPeriodicCount);
  EXPECT_EQ(1u, robot.m_simulationPeriodicCount);
  EXPECT_EQ(0u, robot.m_disabledPeriodicCount);
  EXPECT_EQ(0u, robot.m_autonomousPeriodicCount);
  EXPECT_EQ(0u, robot.m_teleopPeriodicCount);
  EXPECT_EQ(1u, robot.m_testPeriodicCount);

  EXPECT_EQ(0u, robot.m_disabledExitCount);
  EXPECT_EQ(0u, robot.m_autonomousExitCount);
  EXPECT_EQ(0u, robot.m_teleopExitCount);
  EXPECT_EQ(0u, robot.m_testExitCount);

  frc::sim::StepTiming(kPeriod);

  EXPECT_EQ(1u, robot.m_robotInitCount);
  EXPECT_EQ(1u, robot.m_simulationInitCount);
  EXPECT_EQ(0u, robot.m_disabledInitCount);
  EXPECT_EQ(0u, robot.m_autonomousInitCount);
  EXPECT_EQ(0u, robot.m_teleopInitCount);
  EXPECT_EQ(1u, robot.m_testInitCount);

  EXPECT_EQ(2u, robot.m_robotPeriodicCount);
  EXPECT_EQ(2u, robot.m_simulationPeriodicCount);
  EXPECT_EQ(0u, robot.m_disabledPeriodicCount);
  EXPECT_EQ(0u, robot.m_autonomousPeriodicCount);
  EXPECT_EQ(0u, robot.m_teleopPeriodicCount);
  EXPECT_EQ(2u, robot.m_testPeriodicCount);

  EXPECT_EQ(0u, robot.m_disabledExitCount);
  EXPECT_EQ(0u, robot.m_autonomousExitCount);
  EXPECT_EQ(0u, robot.m_teleopExitCount);
  EXPECT_EQ(0u, robot.m_testExitCount);

  frc::sim::DriverStationSim::SetEnabled(false);
  frc::sim::DriverStationSim::SetAutonomous(false);
  frc::sim::DriverStationSim::SetTest(false);
  frc::sim::DriverStationSim::NotifyNewData();
  frc::sim::StepTiming(20_ms);  // Wait for Notifiers

  EXPECT_EQ(1u, robot.m_robotInitCount);
  EXPECT_EQ(1u, robot.m_simulationInitCount);
  EXPECT_EQ(1u, robot.m_disabledInitCount);
  EXPECT_EQ(0u, robot.m_autonomousInitCount);
  EXPECT_EQ(0u, robot.m_teleopInitCount);
  EXPECT_EQ(1u, robot.m_testInitCount);
  EXPECT_FALSE(frc::LiveWindow::IsEnabled());

  EXPECT_EQ(3u, robot.m_robotPeriodicCount);
  EXPECT_EQ(3u, robot.m_simulationPeriodicCount);
  EXPECT_EQ(1u, robot.m_disabledPeriodicCount);
  EXPECT_EQ(0u, robot.m_autonomousPeriodicCount);
  EXPECT_EQ(0u, robot.m_teleopPeriodicCount);
  EXPECT_EQ(2u, robot.m_testPeriodicCount);

  EXPECT_EQ(0u, robot.m_disabledExitCount);
  EXPECT_EQ(0u, robot.m_autonomousExitCount);
  EXPECT_EQ(0u, robot.m_teleopExitCount);
  EXPECT_EQ(1u, robot.m_testExitCount);

  robot.EndCompetition();
  robotThread.join();
}

TEST_F(TimedRobotTest, ModeChange) {
  MockRobot robot;

  std::thread robotThread{[&] { robot.StartCompetition(); }};

  // Start in disabled
  frc::sim::DriverStationSim::SetEnabled(false);
  frc::sim::DriverStationSim::SetAutonomous(false);
  frc::sim::DriverStationSim::SetTest(false);
  frc::sim::DriverStationSim::NotifyNewData();
  frc::sim::StepTiming(0_ms);  // Wait for Notifiers

  EXPECT_EQ(0u, robot.m_disabledInitCount);
  EXPECT_EQ(0u, robot.m_autonomousInitCount);
  EXPECT_EQ(0u, robot.m_teleopInitCount);
  EXPECT_EQ(0u, robot.m_testInitCount);

  EXPECT_EQ(0u, robot.m_disabledExitCount);
  EXPECT_EQ(0u, robot.m_autonomousExitCount);
  EXPECT_EQ(0u, robot.m_teleopExitCount);
  EXPECT_EQ(0u, robot.m_testExitCount);

  frc::sim::StepTiming(kPeriod);

  EXPECT_EQ(1u, robot.m_disabledInitCount);
  EXPECT_EQ(0u, robot.m_autonomousInitCount);
  EXPECT_EQ(0u, robot.m_teleopInitCount);
  EXPECT_EQ(0u, robot.m_testInitCount);

  EXPECT_EQ(0u, robot.m_disabledExitCount);
  EXPECT_EQ(0u, robot.m_autonomousExitCount);
  EXPECT_EQ(0u, robot.m_teleopExitCount);
  EXPECT_EQ(0u, robot.m_testExitCount);

  // Transition to autonomous
  frc::sim::DriverStationSim::SetEnabled(true);
  frc::sim::DriverStationSim::SetAutonomous(true);
  frc::sim::DriverStationSim::SetTest(false);
  frc::sim::DriverStationSim::NotifyNewData();

  frc::sim::StepTiming(kPeriod);

  EXPECT_EQ(1u, robot.m_disabledInitCount);
  EXPECT_EQ(1u, robot.m_autonomousInitCount);
  EXPECT_EQ(0u, robot.m_teleopInitCount);
  EXPECT_EQ(0u, robot.m_testInitCount);

  EXPECT_EQ(1u, robot.m_disabledExitCount);
  EXPECT_EQ(0u, robot.m_autonomousExitCount);
  EXPECT_EQ(0u, robot.m_teleopExitCount);
  EXPECT_EQ(0u, robot.m_testExitCount);

  // Transition to teleop
  frc::sim::DriverStationSim::SetEnabled(true);
  frc::sim::DriverStationSim::SetAutonomous(false);
  frc::sim::DriverStationSim::SetTest(false);
  frc::sim::DriverStationSim::NotifyNewData();

  frc::sim::StepTiming(kPeriod);

  EXPECT_EQ(1u, robot.m_disabledInitCount);
  EXPECT_EQ(1u, robot.m_autonomousInitCount);
  EXPECT_EQ(1u, robot.m_teleopInitCount);
  EXPECT_EQ(0u, robot.m_testInitCount);

  EXPECT_EQ(1u, robot.m_disabledExitCount);
  EXPECT_EQ(1u, robot.m_autonomousExitCount);
  EXPECT_EQ(0u, robot.m_teleopExitCount);
  EXPECT_EQ(0u, robot.m_testExitCount);

  // Transition to test
  frc::sim::DriverStationSim::SetEnabled(true);
  frc::sim::DriverStationSim::SetAutonomous(false);
  frc::sim::DriverStationSim::SetTest(true);
  frc::sim::DriverStationSim::NotifyNewData();

  frc::sim::StepTiming(kPeriod);

  EXPECT_EQ(1u, robot.m_disabledInitCount);
  EXPECT_EQ(1u, robot.m_autonomousInitCount);
  EXPECT_EQ(1u, robot.m_teleopInitCount);
  EXPECT_EQ(1u, robot.m_testInitCount);

  EXPECT_EQ(1u, robot.m_disabledExitCount);
  EXPECT_EQ(1u, robot.m_autonomousExitCount);
  EXPECT_EQ(1u, robot.m_teleopExitCount);
  EXPECT_EQ(0u, robot.m_testExitCount);

  // Transition to disabled
  frc::sim::DriverStationSim::SetEnabled(false);
  frc::sim::DriverStationSim::SetAutonomous(false);
  frc::sim::DriverStationSim::SetTest(false);
  frc::sim::DriverStationSim::NotifyNewData();

  frc::sim::StepTiming(kPeriod);

  EXPECT_EQ(2u, robot.m_disabledInitCount);
  EXPECT_EQ(1u, robot.m_autonomousInitCount);
  EXPECT_EQ(1u, robot.m_teleopInitCount);
  EXPECT_EQ(1u, robot.m_testInitCount);

  EXPECT_EQ(1u, robot.m_disabledExitCount);
  EXPECT_EQ(1u, robot.m_autonomousExitCount);
  EXPECT_EQ(1u, robot.m_teleopExitCount);
  EXPECT_EQ(1u, robot.m_testExitCount);

  robot.EndCompetition();
  robotThread.join();
}

TEST_F(TimedRobotTest, AddPeriodic) {
  MockRobot robot;

  std::atomic<uint32_t> callbackCount{0};
  robot.AddPeriodic([&] { callbackCount++; }, kPeriod / 2.0);

  std::thread robotThread{[&] { robot.StartCompetition(); }};

  frc::sim::DriverStationSim::SetEnabled(false);
  frc::sim::DriverStationSim::NotifyNewData();
  frc::sim::StepTiming(0_ms);  // Wait for Notifiers

  EXPECT_EQ(0u, robot.m_disabledInitCount);
  EXPECT_EQ(0u, robot.m_disabledPeriodicCount);
  EXPECT_EQ(0u, callbackCount);

  frc::sim::StepTiming(kPeriod / 2.0);

  EXPECT_EQ(0u, robot.m_disabledInitCount);
  EXPECT_EQ(0u, robot.m_disabledPeriodicCount);
  EXPECT_EQ(1u, callbackCount);

  frc::sim::StepTiming(kPeriod / 2.0);

  EXPECT_EQ(1u, robot.m_disabledInitCount);
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

  frc::sim::DriverStationSim::SetEnabled(false);
  frc::sim::DriverStationSim::NotifyNewData();
  frc::sim::StepTiming(0_ms);  // Wait for Notifiers

  EXPECT_EQ(0u, robot.m_disabledInitCount);
  EXPECT_EQ(0u, robot.m_disabledPeriodicCount);
  EXPECT_EQ(0u, callbackCount);

  frc::sim::StepTiming(kPeriod * 3.0 / 8.0);

  EXPECT_EQ(0u, robot.m_disabledInitCount);
  EXPECT_EQ(0u, robot.m_disabledPeriodicCount);
  EXPECT_EQ(0u, callbackCount);

  frc::sim::StepTiming(kPeriod * 3.0 / 8.0);

  EXPECT_EQ(0u, robot.m_disabledInitCount);
  EXPECT_EQ(0u, robot.m_disabledPeriodicCount);
  EXPECT_EQ(1u, callbackCount);

  frc::sim::StepTiming(kPeriod / 4.0);

  EXPECT_EQ(1u, robot.m_disabledInitCount);
  EXPECT_EQ(1u, robot.m_disabledPeriodicCount);
  EXPECT_EQ(1u, callbackCount);

  frc::sim::StepTiming(kPeriod / 4.0);

  EXPECT_EQ(1u, robot.m_disabledInitCount);
  EXPECT_EQ(1u, robot.m_disabledPeriodicCount);
  EXPECT_EQ(2u, callbackCount);

  robot.EndCompetition();
  robotThread.join();
}

INSTANTIATE_TEST_SUITE_P(TimedRobotTests, TimedRobotTest, testing::Bool());
