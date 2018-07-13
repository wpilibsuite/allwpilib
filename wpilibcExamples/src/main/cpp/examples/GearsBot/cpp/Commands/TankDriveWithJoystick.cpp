/*----------------------------------------------------------------------------*/
/* Use and modify this template how you wish, it's public domain.             */
/*----------------------------------------------------------------------------*/

#include "Commands/TankDriveWithJoystick.h"

#include "Robot.h"

TankDriveWithJoystick::TankDriveWithJoystick()
    : frc::Command("TankDriveWithJoystick") {
  Requires(&Robot::drivetrain);
}

// Called repeatedly when this Command is scheduled to run
void TankDriveWithJoystick::Execute() {
  auto& joystick = Robot::oi.GetJoystick();
  Robot::drivetrain.Drive(-joystick.GetY(), -joystick.GetRawAxis(4));
}

// Make this return true when this Command no longer needs to run execute()
bool TankDriveWithJoystick::IsFinished() { return false; }

// Called once after isFinished returns true
void TankDriveWithJoystick::End() { Robot::drivetrain.Drive(0, 0); }
