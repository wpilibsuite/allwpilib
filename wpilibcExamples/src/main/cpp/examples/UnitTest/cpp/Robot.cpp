// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "Robot.hpp"

/**
 * This function is called periodically during operator control.
 */
void Robot::TeleopPeriodic() {
  // Activate the intake while the trigger is held
  if (joystick.GetTrigger()) {
    intake.Activate(IntakeConstants::kIntakeVelocity);
  } else {
    intake.Activate(0);
  }

  // Toggle deploying the intake when the top button is pressed
  if (joystick.GetTop()) {
    if (intake.IsDeployed()) {
      intake.Retract();
    } else {
      intake.Deploy();
    }
  }
}

#ifndef RUNNING_WPILIB_TESTS
int main() {
  return wpi::StartRobot<Robot>();
}
#endif
