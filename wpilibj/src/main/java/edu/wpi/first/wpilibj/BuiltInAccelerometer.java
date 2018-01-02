/*----------------------------------------------------------------------------*/
/* Copyright (c) 2014-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import edu.wpi.first.wpilibj.hal.AccelerometerJNI;
import edu.wpi.first.wpilibj.hal.FRCNetComm.tResourceType;
import edu.wpi.first.wpilibj.hal.HAL;
import edu.wpi.first.wpilibj.interfaces.Accelerometer;
import edu.wpi.first.wpilibj.smartdashboard.SendableBuilder;

/**
 * Built-in accelerometer.
 *
 * <p>This class allows access to the roboRIO's internal accelerometer.
 */
public class BuiltInAccelerometer extends SensorBase implements Accelerometer, Sendable {
  /**
   * Constructor.
   *
   * @param range The range the accelerometer will measure
   */
  public BuiltInAccelerometer(Range range) {
    setRange(range);
    HAL.report(tResourceType.kResourceType_Accelerometer, 0, 0, "Built-in accelerometer");
    setName("BuiltInAccel", 0);
  }

  /**
   * Constructor. The accelerometer will measure +/-8 g-forces
   */
  public BuiltInAccelerometer() {
    this(Range.k8G);
  }

  @Override
  public void setRange(Range range) {
    AccelerometerJNI.setAccelerometerActive(false);

    switch (range) {
      case k2G:
        AccelerometerJNI.setAccelerometerRange(0);
        break;
      case k4G:
        AccelerometerJNI.setAccelerometerRange(1);
        break;
      case k8G:
        AccelerometerJNI.setAccelerometerRange(2);
        break;
      case k16G:
      default:
        throw new IllegalArgumentException(range + " range not supported (use k2G, k4G, or k8G)");

    }

    AccelerometerJNI.setAccelerometerActive(true);
  }

  /**
   * The acceleration in the X axis.
   *
   * @return The acceleration of the roboRIO along the X axis in g-forces
   */
  @Override
  public double getX() {
    return AccelerometerJNI.getAccelerometerX();
  }

  /**
   * The acceleration in the Y axis.
   *
   * @return The acceleration of the roboRIO along the Y axis in g-forces
   */
  @Override
  public double getY() {
    return AccelerometerJNI.getAccelerometerY();
  }

  /**
   * The acceleration in the Z axis.
   *
   * @return The acceleration of the roboRIO along the Z axis in g-forces
   */
  @Override
  public double getZ() {
    return AccelerometerJNI.getAccelerometerZ();
  }

  @Override
  public void initSendable(SendableBuilder builder) {
    builder.setSmartDashboardType("3AxisAccelerometer");
    builder.addDoubleProperty("X", this::getX, null);
    builder.addDoubleProperty("Y", this::getY, null);
    builder.addDoubleProperty("Z", this::getZ, null);
  }
}
