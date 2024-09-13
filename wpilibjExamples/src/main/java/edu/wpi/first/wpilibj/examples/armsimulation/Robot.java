// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.examples.armsimulation;

import edu.wpi.first.wpilibj.Joystick;
import edu.wpi.first.wpilibj.TimedRobot;
import edu.wpi.first.wpilibj.examples.armsimulation.subsystems.Arm;

/** This is a sample program to demonstrate the use of arm simulation with existing code. */
public class Robot extends TimedRobot {
  private final Arm m_arm = new Arm();
  private final Joystick m_joystick = new Joystick(Constants.kJoystickPort);

  public Robot() {}

  @Override
  public void simulationPeriodic() {
    m_arm.simulationPeriodic();
  }

  @Override
  public void teleopInit() {
    m_arm.loadPreferences();
  }

  @Override
  public void teleopPeriodic() {
    if (m_joystick.getTrigger()) {
      // Here, we run PID control like normal.
      m_arm.reachSetpoint();
    } else {
      // Otherwise, we disable the motor.
      m_arm.stop();
    }
  }

  @Override
  public void close() {
    m_arm.close();
    super.close();
  }

  @Override
  public void disabledInit() {
    // This just makes sure that our simulation code knows that the motor's off.
    m_arm.stop();
  }
}
