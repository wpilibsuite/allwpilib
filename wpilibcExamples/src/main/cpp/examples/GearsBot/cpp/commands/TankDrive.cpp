/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "commands/TankDrive.h"

#include "Robot.h"

TankDrive::TankDrive(std::function<double()> left,
                     std::function<double()> right, DriveTrain* drivetrain)
    : m_left(left), m_right(right), m_drivetrain(drivetrain) {
  SetName("TankDrive");
  AddRequirements({m_drivetrain});
}

// Called repeatedly when this Command is scheduled to run
void TankDrive::Execute() { m_drivetrain->Drive(m_left(), m_right()); }

// Make this return true when this Command no longer needs to run execute()
bool TankDrive::IsFinished() { return false; }

// Called once after isFinished returns true
void TankDrive::End(bool) { m_drivetrain->Drive(0, 0); }
