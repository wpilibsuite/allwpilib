// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.snippets.builtinaccelerometer;

import edu.wpi.first.wpilibj.BuiltInAccelerometer;
import edu.wpi.first.wpilibj.TimedRobot;

/**
 * Built-In Accelerometer snippets for frc-docs.
 * https://docs.wpilib.org/en/stable/docs/software/hardware-apis/sensors/accelerometers-software.html
 */
public class Robot extends TimedRobot {
  // Creates an object for the built-in accelerometer
  // Range defaults to +- 8 G's
  BuiltInAccelerometer m_accelerometer = new BuiltInAccelerometer();

  /** Called once at the beginning of the robot program. */
  public Robot() {
    // Sets the accelerometer to measure between -8 and 8 G's
    m_accelerometer.setRange(BuiltInAccelerometer.Range.k8G);
  }

  @Override
  public void teleopPeriodic() {
    // Gets the current acceleration in the X axis
    m_accelerometer.getX();
    // Gets the current acceleration in the Y axis
    m_accelerometer.getY();
    // Gets the current acceleration in the Z axis
    m_accelerometer.getZ();
  }
}
