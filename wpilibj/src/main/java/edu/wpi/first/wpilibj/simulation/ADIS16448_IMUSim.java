// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.simulation;

import edu.wpi.first.hal.SimDouble;
import edu.wpi.first.wpilibj.ADIS16448_IMU;

/** Class to control a simulated ADIS16448 gyroscope. */
@SuppressWarnings({"TypeName", "AbbreviationAsWordInName"})
public class ADIS16448_IMUSim {
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
   * Constructs from an ADIS16448_IMU object.
   *
   * @param gyro ADIS16448_IMU to simulate
   */
  public ADIS16448_IMUSim(ADIS16448_IMU gyro) {
    SimDeviceSim wrappedSimDevice = new SimDeviceSim("Gyro:ADIS16448" + "[" + gyro.getPort() + "]");
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
   * Sets the X axis angle (CCW positive).
   *
   * @param angleDegrees The angle.
   */
  public void setGyroAngleX(double angleDegrees) {
    m_simGyroAngleX.set(angleDegrees);
  }

  /**
   * Sets the Y axis angle (CCW positive).
   *
   * @param angleDegrees The angle.
   */
  public void setGyroAngleY(double angleDegrees) {
    m_simGyroAngleY.set(angleDegrees);
  }

  /**
   * Sets the Z axis angle (CCW positive).
   *
   * @param angleDegrees The angle.
   */
  public void setGyroAngleZ(double angleDegrees) {
    m_simGyroAngleZ.set(angleDegrees);
  }

  /**
   * Sets the X axis angular rate (CCW positive).
   *
   * @param rateDegreesPerSecond The angular rate.
   */
  public void setGyroRateX(double rateDegreesPerSecond) {
    m_simGyroRateX.set(rateDegreesPerSecond);
  }

  /**
   * Sets the Y axis angular rate (CCW positive).
   *
   * @param rateDegreesPerSecond The angular rate.
   */
  public void setGyroRateY(double rateDegreesPerSecond) {
    m_simGyroRateY.set(rateDegreesPerSecond);
  }

  /**
   * Sets the Z axis angular rate (CCW positive).
   *
   * @param rateDegreesPerSecond The angular rate.
   */
  public void setGyroRateZ(double rateDegreesPerSecond) {
    m_simGyroRateZ.set(rateDegreesPerSecond);
  }

  /**
   * Sets the X axis acceleration.
   *
   * @param accelMetersPerSecond The acceleration.
   */
  public void setAccelX(double accelMetersPerSecond) {
    m_simAccelX.set(accelMetersPerSecond / 9.81);
  }

  /**
   * Sets the Y axis acceleration.
   *
   * @param accelMetersPerSecond The acceleration.
   */
  public void setAccelY(double accelMetersPerSecond) {
    m_simAccelY.set(accelMetersPerSecond / 9.81);
  }

  /**
   * Sets the Z axis acceleration.
   *
   * @param accelMetersPerSecond The acceleration.
   */
  public void setAccelZ(double accelMetersPerSecond) {
    m_simAccelZ.set(accelMetersPerSecond / 9.81);
  }
}
