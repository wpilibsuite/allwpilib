// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc2/command/CommandRobot.h"

#include <frc/smartdashboard/SmartDashboard.h>
#include <units/time.h>

frc2::CommandRobot::CommandRobot(units::second_t period)
    : frc::TimedRobot(period) {
  AddPeriodic([this] { m_scheduler.Run(); }, 20_ms);
}

void frc2::CommandRobot::RobotInit() {}

void frc2::CommandRobot::RobotPeriodic() {}

void frc2::CommandRobot::AutonomousInit() {}

void frc2::CommandRobot::AutonomousPeriodic() {}

void frc2::CommandRobot::AutonomousExit() {}

void frc2::CommandRobot::TeleopInit() {}

void frc2::CommandRobot::TeleopPeriodic() {}

void frc2::CommandRobot::TeleopExit() {}

void frc2::CommandRobot::DisabledInit() {}

void frc2::CommandRobot::DisabledPeriodic() {}

void frc2::CommandRobot::DisabledExit() {}
