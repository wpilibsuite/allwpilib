// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "Robot.h"

/**
 * This function is called periodically during operator control.
 */
void Robot::TeleopPeriodic() {
  // Activate the intake while the trigger is held
  if (m_joystick.GetTrigger()) {
    m_intake.Activate(IntakeConstants::kIntakeSpeed);
  } else {
    m_intake.Activate(0);
  }

  // Toggle deploying the intake when the top button is pressed
  if (m_joystick.GetTop()) {
    if (m_intake.IsDeployed()) {
      m_intake.Retract();
    } else {
      m_intake.Deploy();
    }
  }
}

#ifndef RUNNING_FRC_TESTS
int main() {
  return frc::StartRobot<Robot>();
}
#endif
