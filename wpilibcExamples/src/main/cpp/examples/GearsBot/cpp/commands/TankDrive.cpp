// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "commands/TankDrive.h"

#include <utility>

#include "Robot.h"

TankDrive::TankDrive(std::function<double()> left,
                     std::function<double()> right, DriveTrain* drivetrain)
    : m_left(std::move(left)),
      m_right(std::move(right)),
      m_drivetrain(drivetrain) {
  SetName("TankDrive");
  AddRequirements({m_drivetrain});
}

// Called repeatedly when this Command is scheduled to run
void TankDrive::Execute() {
  m_drivetrain->Drive(m_left(), m_right());
}

// Make this return true when this Command no longer needs to run execute()
bool TankDrive::IsFinished() {
  return false;
}

// Called once after isFinished returns true
void TankDrive::End(bool) {
  m_drivetrain->Drive(0, 0);
}
