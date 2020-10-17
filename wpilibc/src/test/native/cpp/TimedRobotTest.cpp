/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/TimedRobot.h"  // NOLINT(build/include_order)

#include <stdint.h>

#include <atomic>
#include <thread>

#include "frc/simulation/DriverStationSim.h"
#include "frc/simulation/SimHooks.h"
#include "gtest/gtest.h"

using namespace frc;

namespace {
class TimedRobotTest : public ::testing::Test {
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

  std::atomic<uint32_t> m_robotPeriodicCount{0};
  std::atomic<uint32_t> m_simulationPeriodicCount{0};
  std::atomic<uint32_t> m_disabledPeriodicCount{0};
  std::atomic<uint32_t> m_autonomousPeriodicCount{0};
  std::atomic<uint32_t> m_teleopPeriodicCount{0};
  std::atomic<uint32_t> m_testPeriodicCount{0};

  void RobotInit() override { m_robotInitCount++; }

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
};
}  // namespace

TEST_F(TimedRobotTest, Disabled) {
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

  frc::sim::StepTiming(20_ms);

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

  frc::sim::StepTiming(20_ms);

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

  robot.EndCompetition();
  robotThread.join();
}

TEST_F(TimedRobotTest, Autonomous) {
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

  frc::sim::StepTiming(20_ms);

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

  frc::sim::StepTiming(20_ms);

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

  robot.EndCompetition();
  robotThread.join();
}

TEST_F(TimedRobotTest, Teleop) {
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

  frc::sim::StepTiming(20_ms);

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

  frc::sim::StepTiming(20_ms);

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

  robot.EndCompetition();
  robotThread.join();
}

TEST_F(TimedRobotTest, Test) {
  MockRobot robot;

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

  EXPECT_EQ(0u, robot.m_robotPeriodicCount);
  EXPECT_EQ(0u, robot.m_simulationPeriodicCount);
  EXPECT_EQ(0u, robot.m_disabledPeriodicCount);
  EXPECT_EQ(0u, robot.m_autonomousPeriodicCount);
  EXPECT_EQ(0u, robot.m_teleopPeriodicCount);
  EXPECT_EQ(0u, robot.m_testPeriodicCount);

  frc::sim::StepTiming(20_ms);

  EXPECT_EQ(1u, robot.m_robotInitCount);
  EXPECT_EQ(1u, robot.m_simulationInitCount);
  EXPECT_EQ(0u, robot.m_disabledInitCount);
  EXPECT_EQ(0u, robot.m_autonomousInitCount);
  EXPECT_EQ(0u, robot.m_teleopInitCount);
  EXPECT_EQ(1u, robot.m_testInitCount);

  EXPECT_EQ(1u, robot.m_robotPeriodicCount);
  EXPECT_EQ(1u, robot.m_simulationPeriodicCount);
  EXPECT_EQ(0u, robot.m_disabledPeriodicCount);
  EXPECT_EQ(0u, robot.m_autonomousPeriodicCount);
  EXPECT_EQ(0u, robot.m_teleopPeriodicCount);
  EXPECT_EQ(1u, robot.m_testPeriodicCount);

  frc::sim::StepTiming(20_ms);

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

  robot.EndCompetition();
  robotThread.join();
}

TEST_F(TimedRobotTest, AddPeriodic) {
  MockRobot robot;

  std::atomic<uint32_t> callbackCount{0};
  robot.AddPeriodic([&] { callbackCount++; }, 10_ms);

  std::thread robotThread{[&] { robot.StartCompetition(); }};

  frc::sim::DriverStationSim::SetEnabled(false);
  frc::sim::DriverStationSim::NotifyNewData();
  frc::sim::StepTiming(0_ms);  // Wait for Notifiers

  EXPECT_EQ(0u, robot.m_disabledInitCount);
  EXPECT_EQ(0u, robot.m_disabledPeriodicCount);
  EXPECT_EQ(0u, callbackCount);

  frc::sim::StepTiming(10_ms);

  EXPECT_EQ(0u, robot.m_disabledInitCount);
  EXPECT_EQ(0u, robot.m_disabledPeriodicCount);
  EXPECT_EQ(1u, callbackCount);

  frc::sim::StepTiming(10_ms);

  EXPECT_EQ(1u, robot.m_disabledInitCount);
  EXPECT_EQ(1u, robot.m_disabledPeriodicCount);
  EXPECT_EQ(2u, callbackCount);

  robot.EndCompetition();
  robotThread.join();
}

TEST_F(TimedRobotTest, AddPeriodicWithOffset) {
  MockRobot robot;

  std::atomic<uint32_t> callbackCount{0};
  robot.AddPeriodic([&] { callbackCount++; }, 10_ms, 5_ms);

  // Expirations in this test (ms)
  //
  // Robot | Callback
  // ================
  //    20 |      15
  //    40 |      25

  std::thread robotThread{[&] { robot.StartCompetition(); }};

  frc::sim::DriverStationSim::SetEnabled(false);
  frc::sim::DriverStationSim::NotifyNewData();
  frc::sim::StepTiming(0_ms);  // Wait for Notifiers

  EXPECT_EQ(0u, robot.m_disabledInitCount);
  EXPECT_EQ(0u, robot.m_disabledPeriodicCount);
  EXPECT_EQ(0u, callbackCount);

  frc::sim::StepTiming(7.5_ms);

  EXPECT_EQ(0u, robot.m_disabledInitCount);
  EXPECT_EQ(0u, robot.m_disabledPeriodicCount);
  EXPECT_EQ(0u, callbackCount);

  frc::sim::StepTiming(7.5_ms);

  EXPECT_EQ(0u, robot.m_disabledInitCount);
  EXPECT_EQ(0u, robot.m_disabledPeriodicCount);
  EXPECT_EQ(1u, callbackCount);

  frc::sim::StepTiming(5_ms);

  EXPECT_EQ(1u, robot.m_disabledInitCount);
  EXPECT_EQ(1u, robot.m_disabledPeriodicCount);
  EXPECT_EQ(1u, callbackCount);

  frc::sim::StepTiming(5_ms);

  EXPECT_EQ(1u, robot.m_disabledInitCount);
  EXPECT_EQ(1u, robot.m_disabledPeriodicCount);
  EXPECT_EQ(2u, callbackCount);

  robot.EndCompetition();
  robotThread.join();
}
