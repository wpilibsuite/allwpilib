// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.examples.unittest;

import edu.wpi.first.wpilibj.Joystick;
import edu.wpi.first.wpilibj.TimedRobot;
import edu.wpi.first.wpilibj.examples.unittest.Constants.IntakeConstants;
import edu.wpi.first.wpilibj.examples.unittest.subsystems.Intake;

/**
 * The methods in this class are called automatically corresponding to each mode, as described in
 * the TimedRobot documentation. If you change the name of this class or the package after creating
 * this project, you must also update the Main.java file in the project.
 */
public class Robot extends TimedRobot {
  private final Intake m_intake = new Intake();
  private final Joystick m_joystick = new Joystick(Constants.kJoystickIndex);

  /** This function is called periodically during operator control. */
  @Override
  public void teleopPeriodic() {
    // Activate the intake while the trigger is held
    if (m_joystick.getTrigger()) {
      m_intake.activate(IntakeConstants.kIntakeSpeed);
    } else {
      m_intake.activate(0);
    }

    // Toggle deploying the intake when the top button is pressed
    if (m_joystick.getTop()) {
      if (m_intake.isDeployed()) {
        m_intake.retract();
      } else {
        m_intake.deploy();
      }
    }
  }
}
