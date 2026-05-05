// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.simulation;

import org.wpilib.hardware.hal.simulation.IMUDataJNI;

/** Class to control a simulated onboard IMU. */
public final class OnboardIMUSim {
  private OnboardIMUSim() {
    throw new UnsupportedOperationException("This is a utility class!");
  }

  /**
   * Change the x-angle.
   *
   * @param angleRad the new angle
   */
  public static void setAngleX(double angleRad) {
    IMUDataJNI.setAngleX(angleRad);
  }

  /**
   * Change the y-angle.
   *
   * @param angleRad the new angle
   */
  public static void setAngleY(double angleRad) {
    IMUDataJNI.setAngleY(angleRad);
  }

  /**
   * Change the z-angle.
   *
   * @param angleRad the new angle
   */
  public static void setAngleZ(double angleRad) {
    IMUDataJNI.setAngleZ(angleRad);
  }

  /**
   * Change the x-gyro rate.
   *
   * @param rateRad the new rate
   */
  public static void setGyroRateX(double rateRad) {
    IMUDataJNI.setGyroRateX(rateRad);
  }

  /**
   * Change the y-gyro rate.
   *
   * @param rateRad the new rate
   */
  public static void setGyroRateY(double rateRad) {
    IMUDataJNI.setGyroRateY(rateRad);
  }

  /**
   * Change the z-gyro rate.
   *
   * @param rateRad the new rate
   */
  public static void setGyroRateZ(double rateRad) {
    IMUDataJNI.setGyroRateZ(rateRad);
  }

  /**
   * Change the x-accel rate.
   *
   * @param accelRad the new acceleration
   */
  public static void setAccelX(double accelRad) {
    IMUDataJNI.setAccelX(accelRad);
  }

  /**
   * Change the y-accel rate.
   *
   * @param accelRad the new acceleration
   */
  public static void setAccelY(double accelRad) {
    IMUDataJNI.setAccelY(accelRad);
  }

  /**
   * Change the z-accel rate.
   *
   * @param accelRad the new acceleration
   */
  public static void setAccelZ(double accelRad) {
    IMUDataJNI.setAccelZ(accelRad);
  }

  /**
   * Change the yaw.
   *
   * @param angleRad the new yaw angle
   */
  public static void setYaw(double angleRad) {
    IMUDataJNI.setYaw(angleRad);
  }
}
