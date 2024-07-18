// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc2/command/CommandRobot.h"

#include <frc/smartdashboard/SmartDashboard.h>
#include <units/time.h>

frc2::CommandRobot::CommandRobot(units::second_t period)
    : frc::TimedRobot(period) {
  AddPeriodic([this] { m_scheduler.Run(); }, 20_ms);
  m_autoChooser.SetDefaultOption("No auto configured.", m_defaultCommand.get());
  frc::SmartDashboard::PutData(&m_autoChooser);
}

void frc2::CommandRobot::RobotInit() {}

void frc2::CommandRobot::RobotPeriodic() {}

void frc2::CommandRobot::AutonomousInit() {
  m_autonomousCommand = m_autoChooser.GetSelected();
  if (m_autonomousCommand) {
    m_scheduler.Schedule(m_autonomousCommand);
  }
}

void frc2::CommandRobot::AutonomousPeriodic() {}

void frc2::CommandRobot::AutonomousExit() {}

void frc2::CommandRobot::TeleopInit() {
  if (m_autonomousCommand) {
    m_scheduler.Cancel(m_autonomousCommand);
  }
}

void frc2::CommandRobot::TeleopPeriodic() {}

void frc2::CommandRobot::TeleopExit() {}

void frc2::CommandRobot::DisabledInit() {}

void frc2::CommandRobot::DisabledPeriodic() {}

void frc2::CommandRobot::DisabledExit() {}
