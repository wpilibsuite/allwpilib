// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.snippets.accelerometercollision;

import org.wpilib.framework.TimedRobot;
import org.wpilib.hardware.imu.OnboardIMU;
import org.wpilib.hardware.imu.OnboardIMU.MountOrientation;
import org.wpilib.smartdashboard.SmartDashboard;

/**
 * Collision detection snippets for frc-docs.
 * https://docs.wpilib.org/en/stable/docs/software/hardware-apis/sensors/accelerometers-software.html
 */
@SuppressWarnings("checkstyle:VariableDeclarationUsageDistance")
public class Robot extends TimedRobot {
  double prevXAccel;
  double prevYAccel;
  OnboardIMU accelerometer = new OnboardIMU(MountOrientation.FLAT);

  @Override
  public void robotPeriodic() {
    // Gets the current accelerations in the X and Y directions
    double xAccel = accelerometer.getAccelX();
    double yAccel = accelerometer.getAccelY();
    // Calculates the jerk in the X and Y directions
    // Divides by .02 because default loop timing is 20ms
    double xJerk = (xAccel - prevXAccel) / 0.02;
    double yJerk = (yAccel - prevYAccel) / 0.02;
    prevXAccel = xAccel;
    prevYAccel = yAccel;

    SmartDashboard.putNumber("X Jerk", xJerk);
    SmartDashboard.putNumber("Y Jerk", yJerk);
  }
}
