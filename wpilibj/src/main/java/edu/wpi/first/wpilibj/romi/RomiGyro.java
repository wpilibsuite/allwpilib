// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.romi;

import edu.wpi.first.hal.SimDevice;
import edu.wpi.first.hal.SimDevice.Direction;
import edu.wpi.first.hal.SimDouble;
import edu.wpi.first.wpilibj.GyroBase;

public class RomiGyro extends GyroBase {
  private final SimDevice m_simDevice;
  private SimDouble m_simRateX;
  private SimDouble m_simRateY;
  private SimDouble m_simRateZ;
  private SimDouble m_simAngleX;
  private SimDouble m_simAngleY;
  private SimDouble m_simAngleZ;

  private double m_angleXOffset;
  private double m_angleYOffset;
  private double m_angleZOffset;

  /** Create a new RomiGyro. */
  public RomiGyro() {
    m_simDevice = SimDevice.create("Gyro:RomiGyro");
    if (m_simDevice != null) {
      m_simDevice.createBoolean("init", Direction.kOutput, true);
      m_simRateX = m_simDevice.createDouble("rate_x", Direction.kInput, 0.0);
      m_simRateY = m_simDevice.createDouble("rate_y", Direction.kInput, 0.0);
      m_simRateZ = m_simDevice.createDouble("rate_z", Direction.kInput, 0.0);

      m_simAngleX = m_simDevice.createDouble("angle_x", Direction.kInput, 0.0);
      m_simAngleY = m_simDevice.createDouble("angle_y", Direction.kInput, 0.0);
      m_simAngleZ = m_simDevice.createDouble("angle_z", Direction.kInput, 0.0);
    }
  }

  /**
   * Get the rate of turn in degrees-per-second around the X-axis.
   *
   * @return rate of turn in degrees-per-second
   */
  public double getRateX() {
    if (m_simRateX != null) {
      return m_simRateX.get();
    }

    return 0.0;
  }

  /**
   * Get the rate of turn in degrees-per-second around the Y-axis.
   *
   * @return rate of turn in degrees-per-second
   */
  public double getRateY() {
    if (m_simRateY != null) {
      return m_simRateY.get();
    }

    return 0.0;
  }

  /**
   * Get the rate of turn in degrees-per-second around the Z-axis.
   *
   * @return rate of turn in degrees-per-second
   */
  public double getRateZ() {
    if (m_simRateZ != null) {
      return m_simRateZ.get();
    }

    return 0.0;
  }

  /**
   * Get the currently reported angle around the X-axis.
   *
   * @return current angle around X-axis in degrees
   */
  public double getAngleX() {
    if (m_simAngleX != null) {
      return m_simAngleX.get() - m_angleXOffset;
    }

    return 0.0;
  }

  /**
   * Get the currently reported angle around the X-axis.
   *
   * @return current angle around Y-axis in degrees
   */
  public double getAngleY() {
    if (m_simAngleY != null) {
      return m_simAngleY.get() - m_angleYOffset;
    }

    return 0.0;
  }

  /**
   * Get the currently reported angle around the Z-axis.
   *
   * @return current angle around Z-axis in degrees
   */
  public double getAngleZ() {
    if (m_simAngleZ != null) {
      return m_simAngleZ.get() - m_angleZOffset;
    }

    return 0.0;
  }

  /** Reset the gyro angles to 0. */
  @Override
  public void reset() {
    if (m_simAngleX != null) {
      m_angleXOffset = m_simAngleX.get();
      m_angleYOffset = m_simAngleY.get();
      m_angleZOffset = m_simAngleZ.get();
    }
  }

  @Override
  public void calibrate() {
    // no-op - Romi Gyro calibration is done via web UI
  }

  @Override
  public double getAngle() {
    return getAngleZ();
  }

  @Override
  public double getRate() {
    return getRateZ();
  }

  @Override
  public void close() throws Exception {
    if (m_simDevice != null) {
      m_simDevice.close();
    }
  }
}
