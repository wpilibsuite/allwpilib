// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.examples.armsimulation;

import org.wpilib.driverstation.Joystick;
import org.wpilib.examples.armsimulation.subsystems.Arm;
import org.wpilib.framework.TimedRobot;

/** This is a sample program to demonstrate the use of arm simulation with existing code. */
public class Robot extends TimedRobot {
  private final Arm arm = new Arm();
  private final Joystick joystick = new Joystick(Constants.kJoystickPort);

  public Robot() {}

  @Override
  public void simulationPeriodic() {
    arm.simulationPeriodic();
  }

  @Override
  public void teleopInit() {
    arm.loadPreferences();
  }

  @Override
  public void teleopPeriodic() {
    if (joystick.getTrigger()) {
      // Here, we run PID control like normal.
      arm.reachSetpoint();
    } else {
      // Otherwise, we disable the motor.
      arm.stop();
    }
  }

  @Override
  public void close() {
    arm.close();
    super.close();
  }

  @Override
  public void disabledInit() {
    // This just makes sure that our simulation code knows that the motor's off.
    arm.stop();
  }
}
