// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import edu.wpi.first.hal.AccelerometerJNI;
import edu.wpi.first.hal.FRCNetComm.tResourceType;
import edu.wpi.first.hal.HAL;
import edu.wpi.first.util.sendable.Sendable;
import edu.wpi.first.util.sendable.SendableBuilder;
import edu.wpi.first.util.sendable.SendableRegistry;

/**
 * Built-in accelerometer.
 *
 * <p>This class allows access to the roboRIO's internal accelerometer.
 */
public class BuiltInAccelerometer implements Sendable, AutoCloseable {
  /** Accelerometer range. */
  public enum Range {
    /** 2 Gs max. */
    k2G,
    /** 4 Gs max. */
    k4G,
    /** 8 Gs max. */
    k8G
  }

  /**
   * Constructor.
   *
   * @param range The range the accelerometer will measure
   */
  @SuppressWarnings("this-escape")
  public BuiltInAccelerometer(Range range) {
    setRange(range);
    HAL.report(tResourceType.kResourceType_Accelerometer, 0, 0, "Built-in accelerometer");
    SendableRegistry.addLW(this, "BuiltInAccel", 0);
  }

  /** Constructor. The accelerometer will measure +/-8 g-forces */
  public BuiltInAccelerometer() {
    this(Range.k8G);
  }

  @Override
  public void close() {
    SendableRegistry.remove(this);
  }

  /**
   * Set the measuring range of the accelerometer.
   *
   * @param range The maximum acceleration, positive or negative, that the accelerometer will
   *     measure.
   */
  public final void setRange(Range range) {
    AccelerometerJNI.setAccelerometerActive(false);

    int rangeValue =
        switch (range) {
          case k2G -> 0;
          case k4G -> 1;
          case k8G -> 2;
        };

    AccelerometerJNI.setAccelerometerRange(rangeValue);
    AccelerometerJNI.setAccelerometerActive(true);
  }

  /**
   * The acceleration in the X axis.
   *
   * @return The acceleration of the roboRIO along the X axis in g-forces
   */
  public double getX() {
    return AccelerometerJNI.getAccelerometerX();
  }

  /**
   * The acceleration in the Y axis.
   *
   * @return The acceleration of the roboRIO along the Y axis in g-forces
   */
  public double getY() {
    return AccelerometerJNI.getAccelerometerY();
  }

  /**
   * The acceleration in the Z axis.
   *
   * @return The acceleration of the roboRIO along the Z axis in g-forces
   */
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
