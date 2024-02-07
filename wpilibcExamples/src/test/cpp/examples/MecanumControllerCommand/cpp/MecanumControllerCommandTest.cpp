// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <thread>

#include <frc/simulation/DriverStationSim.h>
#include <frc/simulation/SimHooks.h>
#include <gtest/gtest.h>
#include <units/time.h>

#include "Robot.h"

class MecanumControllerCommandTest : public testing::Test {
  Robot m_robot;
  std::optional<std::thread> m_thread;
  bool joystickWarning;

 public:
  void SetUp() override {
    frc::sim::PauseTiming();
    joystickWarning = frc::DriverStation::IsJoystickConnectionWarningSilenced();
    frc::DriverStation::SilenceJoystickConnectionWarning(true);

    m_thread = std::thread([&] { m_robot.StartCompetition(); });
    frc::sim::StepTiming(0.0_ms);  // Wait for Notifiers
  }

  void TearDown() override {
    m_robot.EndCompetition();
    m_thread->join();

    frc::sim::DriverStationSim::ResetData();
    frc::DriverStation::SilenceJoystickConnectionWarning(joystickWarning);
  }
};

TEST_F(MecanumControllerCommandTest, Match) {
  // auto
  frc::sim::DriverStationSim::SetAutonomous(true);
  frc::sim::DriverStationSim::SetEnabled(true);
  frc::sim::DriverStationSim::NotifyNewData();

  frc::sim::StepTiming(15_s);

  // brief disabled period- exact duration shouldn't matter
  frc::sim::DriverStationSim::SetAutonomous(false);
  frc::sim::DriverStationSim::SetEnabled(false);
  frc::sim::DriverStationSim::NotifyNewData();

  frc::sim::StepTiming(3_s);

  // teleop
  frc::sim::DriverStationSim::SetAutonomous(false);
  frc::sim::DriverStationSim::SetEnabled(true);
  frc::sim::DriverStationSim::NotifyNewData();

  frc::sim::StepTiming(135_s);

  // end of match
  frc::sim::DriverStationSim::SetAutonomous(false);
  frc::sim::DriverStationSim::SetEnabled(false);
  frc::sim::DriverStationSim::NotifyNewData();
}
