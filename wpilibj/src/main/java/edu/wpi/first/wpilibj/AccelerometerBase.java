/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import edu.wpi.first.wpilibj.interfaces.Accelerometer;
import edu.wpi.first.wpilibj.smartdashboard.SendableBuilder;

/**
 * Interface for 3-axis accelerometers that can be displayed.
 */
public abstract class AccelerometerBase extends SendableBase implements Accelerometer {
  @Override
  public void initSendable(SendableBuilder builder) {
    builder.setSmartDashboardType("3AxisAccelerometer");
    builder.addDoubleProperty("X", this::getX, null);
    builder.addDoubleProperty("Y", this::getY, null);
    builder.addDoubleProperty("Z", this::getZ, null);
  }
}
