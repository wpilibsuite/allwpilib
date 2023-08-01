// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.simulation;

import edu.wpi.first.hal.SimDouble;
import edu.wpi.first.wpilibj.ADIS16470_IMU;

/** Class to control a simulated ADIS16470 gyroscope. */
@SuppressWarnings("TypeName")
public class ADIS16470_IMUSim {
  private final SimDouble m_simGyroAngleX;
  private final SimDouble m_simGyroAngleY;
  private final SimDouble m_simGyroAngleZ;
  private final SimDouble m_simGyroRateX;
  private final SimDouble m_simGyroRateY;
  private final SimDouble m_simGyroRateZ;
  private final SimDouble m_simAccelX;
  private final SimDouble m_simAccelY;
  private final SimDouble m_simAccelZ;

  /**
   * Constructs from an ADIS16470_IMU object.
   *
   * @param gyro ADIS16470_IMU to simulate
   */
  public ADIS16470_IMUSim(ADIS16470_IMU gyro) {
    SimDeviceSim wrappedSimDevice = new SimDeviceSim("Gyro:ADIS16470" + "[" + gyro.getPort() + "]");
    m_simGyroAngleX = wrappedSimDevice.getDouble("gyro_angle_x");
    m_simGyroAngleY = wrappedSimDevice.getDouble("gyro_angle_y");
    m_simGyroAngleZ = wrappedSimDevice.getDouble("gyro_angle_z");
    m_simGyroRateX = wrappedSimDevice.getDouble("gyro_rate_x");
    m_simGyroRateY = wrappedSimDevice.getDouble("gyro_rate_y");
    m_simGyroRateZ = wrappedSimDevice.getDouble("gyro_rate_z");
    m_simAccelX = wrappedSimDevice.getDouble("accel_x");
    m_simAccelY = wrappedSimDevice.getDouble("accel_y");
    m_simAccelZ = wrappedSimDevice.getDouble("accel_z");
  }

  /**
   * Sets the X axis angle in degrees (CCW positive).
   *
   * @param angleDegrees The angle.
   */
  public void setGyroAngleX(double angleDegrees) {
    m_simGyroAngleX.set(angleDegrees);
  }

  /**
   * Sets the Y axis angle in degrees (CCW positive).
   *
   * @param angleDegrees The angle.
   */
  public void setGyroAngleY(double angleDegrees) {
    m_simGyroAngleY.set(angleDegrees);
  }

  /**
   * Sets the Z axis angle in degrees (CCW positive).
   *
   * @param angleDegrees The angle.
   */
  public void setGyroAngleZ(double angleDegrees) {
    m_simGyroAngleZ.set(angleDegrees);
  }

  /**
   * Sets the X axis angle in degrees per second (CCW positive).
   *
   * @param angularRateDegreesPerSecond The angular rate.
   */
  public void setGyroRateX(double angularRateDegreesPerSecond) {
    m_simGyroRateX.set(angularRateDegreesPerSecond);
  }

  /**
   * Sets the Y axis angle in degrees per second (CCW positive).
   *
   * @param angularRateDegreesPerSecond The angular rate.
   */
  public void setGyroRateY(double angularRateDegreesPerSecond) {
    m_simGyroRateY.set(angularRateDegreesPerSecond);
  }

  /**
   * Sets the Z axis angle in degrees per second (CCW positive).
   *
   * @param angularRateDegreesPerSecond The angular rate.
   */
  public void setGyroRateZ(double angularRateDegreesPerSecond) {
    m_simGyroRateZ.set(angularRateDegreesPerSecond);
  }

  /**
   * Sets the X axis acceleration in meters per second squared.
   *
   * @param accelMetersPerSecondSquared The acceleration.
   */
  public void setAccelX(double accelMetersPerSecondSquared) {
    m_simAccelX.set(accelMetersPerSecondSquared);
  }

  /**
   * Sets the Y axis acceleration in meters per second squared.
   *
   * @param accelMetersPerSecondSquared The acceleration.
   */
  public void setAccelY(double accelMetersPerSecondSquared) {
    m_simAccelY.set(accelMetersPerSecondSquared);
  }

  /**
   * Sets the Z axis acceleration in meters per second squared.
   *
   * @param accelMetersPerSecondSquared The acceleration.
   */
  public void setAccelZ(double accelMetersPerSecondSquared) {
    m_simAccelZ.set(accelMetersPerSecondSquared);
  }
}
