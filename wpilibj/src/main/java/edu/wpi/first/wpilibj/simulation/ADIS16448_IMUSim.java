// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.simulation;

import edu.wpi.first.hal.SimDouble;
import edu.wpi.first.math.geometry.Rotation3d;
import edu.wpi.first.wpilibj.ADIS16448_IMU;

/** Class to control a simulated ADIS16448 gyroscope. */
@SuppressWarnings("TypeName")
public class ADIS16448_IMUSim {
  private final SimDouble m_simOrientationW;
  private final SimDouble m_simOrientationX;
  private final SimDouble m_simOrientationY;
  private final SimDouble m_simOrientationZ;
  private final SimDouble m_simGyroRateX;
  private final SimDouble m_simGyroRateY;
  private final SimDouble m_simGyroRateZ;
  private final SimDouble m_simAccelX;
  private final SimDouble m_simAccelY;
  private final SimDouble m_simAccelZ;

  /**
   * Constructs from an ADIS16448_IMU object.
   *
   * @param gyro ADIS16448_IMU to simulate
   */
  public ADIS16448_IMUSim(ADIS16448_IMU gyro) {
    SimDeviceSim wrappedSimDevice = new SimDeviceSim("Gyro:ADIS16448" + "[" + gyro.getPort() + "]");
    m_simOrientationW = wrappedSimDevice.getDouble("gyro_quat_w");
    m_simOrientationX = wrappedSimDevice.getDouble("gyro_quat_x");
    m_simOrientationY = wrappedSimDevice.getDouble("gyro_quat_y");
    m_simOrientationZ = wrappedSimDevice.getDouble("gyro_quat_z");
    m_simGyroRateX = wrappedSimDevice.getDouble("gyro_rate_x");
    m_simGyroRateY = wrappedSimDevice.getDouble("gyro_rate_y");
    m_simGyroRateZ = wrappedSimDevice.getDouble("gyro_rate_z");
    m_simAccelX = wrappedSimDevice.getDouble("accel_x");
    m_simAccelY = wrappedSimDevice.getDouble("accel_y");
    m_simAccelZ = wrappedSimDevice.getDouble("accel_z");
  }

  /**
   * Sets the orientation of the gyro.
   *
   * @param orientation The orientation of the gyro as a Rotation3d.
   */
  public void setGyroOrientation(Rotation3d orientation) {
    m_simOrientationW.set(orientation.getQuaternion().getW());
    m_simOrientationX.set(orientation.getQuaternion().getX());
    m_simOrientationY.set(orientation.getQuaternion().getY());
    m_simOrientationZ.set(orientation.getQuaternion().getZ());
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
