// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.snippets.accelerometercollision;

import edu.wpi.first.wpilibj.BuiltInAccelerometer;
import edu.wpi.first.wpilibj.TimedRobot;
import edu.wpi.first.wpilibj.smartdashboard.SmartDashboard;

/**
 * Collision detection snippets for frc-docs.
 * https://docs.wpilib.org/en/stable/docs/software/hardware-apis/sensors/accelerometers-software.html
 */
@SuppressWarnings("checkstyle:VariableDeclarationUsageDistance")
public class Robot extends TimedRobot {
  double m_prevXAccel;
  double m_prevYAccel;
  BuiltInAccelerometer m_accelerometer = new BuiltInAccelerometer();

  @Override
  public void robotPeriodic() {
    // Gets the current accelerations in the X and Y directions
    double xAccel = m_accelerometer.getX();
    double yAccel = m_accelerometer.getY();
    // Calculates the jerk in the X and Y directions
    // Divides by .02 because default loop timing is 20ms
    double xJerk = (xAccel - m_prevXAccel) / 0.02;
    double yJerk = (yAccel - m_prevYAccel) / 0.02;
    m_prevXAccel = xAccel;
    m_prevYAccel = yAccel;

    SmartDashboard.putNumber("X Jerk", xJerk);
    SmartDashboard.putNumber("Y Jerk", yJerk);
  }
}
