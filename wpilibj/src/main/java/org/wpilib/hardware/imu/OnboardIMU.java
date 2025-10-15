// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.hardware.imu;

import org.wpilib.hardware.hal.IMUJNI;
import org.wpilib.math.geometry.Quaternion;
import org.wpilib.math.geometry.Rotation2d;
import org.wpilib.math.geometry.Rotation3d;

/** SystemCore onboard IMU. */
public class OnboardIMU {
  /** A mount orientation of SystemCore. */
  public enum MountOrientation {
    /** Flat (mounted parallel to the ground). */
    kFlat,
    /** Landscape (vertically mounted with long edge of SystemCore parallel to the ground). */
    kLandscape,
    /** Portrait (vertically mounted with the short edge of SystemCore parallel to the ground). */
    kPortrait
  }

  /**
   * Constructs a handle to the SystemCore onboard IMU.
   *
   * @param mountOrientation the mount orientation of SystemCore to determine yaw.
   */
  public OnboardIMU(MountOrientation mountOrientation) {
    m_mountOrientation = mountOrientation;
  }

  /**
   * Get the yaw value in radians.
   *
   * @return yaw value in radians
   */
  public double getYawRadians() {
    return getYawNoOffset() - m_yawOffset;
  }

  /**
   * Reset the current yaw value to 0. Future reads of the yaw value will be relative to the current
   * orientation.
   */
  public void resetYaw() {
    m_yawOffset = getYawNoOffset();
  }

  /**
   * Get the yaw as a Rotation2d.
   *
   * @return yaw
   */
  public Rotation2d getRotation2d() {
    return new Rotation2d(getYawRadians());
  }

  /**
   * Get the 3D orientation as a Rotation3d.
   *
   * @return 3D orientation
   */
  public Rotation3d getRotation3d() {
    return new Rotation3d(getQuaternion());
  }

  /**
   * Get the 3D orientation as a Quaternion.
   *
   * @return 3D orientation
   */
  public Quaternion getQuaternion() {
    double[] quatRaw = new double[4];
    IMUJNI.getIMUQuaternion(quatRaw);
    return new Quaternion(quatRaw[0], quatRaw[1], quatRaw[2], quatRaw[3]);
  }

  /**
   * Get the angle about the X axis of the IMU in radians.
   *
   * @return angle about the X axis in radians
   */
  public double getAngleX() {
    return getRawEulerAngles()[0];
  }

  /**
   * Get the angle about the Y axis of the IMU in radians.
   *
   * @return angle about the Y axis in radians
   */
  public double getAngleY() {
    return getRawEulerAngles()[1];
  }

  /**
   * Get the angle about the Z axis of the IMU in radians.
   *
   * @return angle about the Z axis in radians
   */
  public double getAngleZ() {
    return getRawEulerAngles()[2];
  }

  /**
   * Get the angular rate about the X axis of the IMU in radians per second.
   *
   * @return angular rate about the X axis in radians per second
   */
  public double getGyroRateX() {
    return getRawGyroRates()[0];
  }

  /**
   * Get the angular rate about the Y axis of the IMU in radians per second.
   *
   * @return angular rate about the Y axis in radians per second
   */
  public double getGyroRateY() {
    return getRawGyroRates()[1];
  }

  /**
   * Get the angular rate about the Z axis of the IMU in radians per second.
   *
   * @return angular rate about the Z axis in radians per second
   */
  public double getGyroRateZ() {
    return getRawGyroRates()[2];
  }

  /**
   * Get the acceleration along the X axis of the IMU in meters per second squared.
   *
   * @return acceleration along the X axis in meters per second squared
   */
  public double getAccelX() {
    return getRawAccels()[0];
  }

  /**
   * Get the acceleration along the X axis of the IMU in meters per second squared.
   *
   * @return acceleration along the X axis in meters per second squared
   */
  public double getAccelY() {
    return getRawAccels()[1];
  }

  /**
   * Get the acceleration along the X axis of the IMU in meters per second squared.
   *
   * @return acceleration along the X axis in meters per second squared
   */
  public double getAccelZ() {
    return getRawAccels()[2];
  }

  private double[] getRawEulerAngles() {
    double[] anglesRaw = new double[3];
    switch (m_mountOrientation) {
      case kFlat -> IMUJNI.getIMUEulerAnglesFlat(anglesRaw);
      case kLandscape -> IMUJNI.getIMUEulerAnglesLandscape(anglesRaw);
      case kPortrait -> IMUJNI.getIMUEulerAnglesPortrait(anglesRaw);
      default -> {
        // NOP
      }
    }
    return anglesRaw;
  }

  private double[] getRawGyroRates() {
    double[] ratesRaw = new double[3];
    IMUJNI.getIMUGyroRates(ratesRaw);
    return ratesRaw;
  }

  private double[] getRawAccels() {
    double[] accelsRaw = new double[3];
    IMUJNI.getIMUAcceleration(accelsRaw);
    return accelsRaw;
  }

  private double getYawNoOffset() {
    return switch (m_mountOrientation) {
      case kFlat -> IMUJNI.getIMUYawFlat();
      case kLandscape -> IMUJNI.getIMUYawLandscape();
      case kPortrait -> IMUJNI.getIMUYawPortrait();
      default -> 0;
    };
  }

  private final MountOrientation m_mountOrientation;
  private double m_yawOffset;
}
