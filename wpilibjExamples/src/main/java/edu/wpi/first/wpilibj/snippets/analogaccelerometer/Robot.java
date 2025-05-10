// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.snippets.analogaccelerometer;

import edu.wpi.first.wpilibj.AnalogAccelerometer;
import edu.wpi.first.wpilibj.TimedRobot;

/**
 * AnalogAccelerometer snippets for frc-docs.
 * https://docs.wpilib.org/en/stable/docs/software/hardware-apis/sensors/accelerometers-software.html
 */
public class Robot extends TimedRobot {
  // Creates an analog accelerometer on analog input 0
  AnalogAccelerometer m_accelerometer = new AnalogAccelerometer(0);

  /** Called once at the beginning of the robot program. */
  public Robot() {
    // Sets the sensitivity of the accelerometer to 1 volt per G
    m_accelerometer.setSensitivity(1);
    // Sets the zero voltage of the accelerometer to 3 volts
    m_accelerometer.setZero(3);
  }

  @Override
  public void teleopPeriodic() {
    // Gets the current acceleration
    m_accelerometer.getAcceleration();
  }
}
