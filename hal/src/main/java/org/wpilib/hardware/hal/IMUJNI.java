// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal;

/**
 * IMU Functions.
 *
 * @see "hal/IMU.h"
 */
public class IMUJNI extends JNIWrapper {
  /**
   * Get the acceleration along the axes of the IMU in meters per second squared.
   *
   * @param accel array of size >= 3 to place the acceleration data in. The data will be in the form
   *     [x, y, z].
   */
  public static native void getIMUAcceleration(double[] accel);

  /**
   * Get the angular rate about the axes of the IMU in radians per second.
   *
   * @param rates array of size >= 3 to place the angular rate data in. The data will be in the form
   *     [x, y, z].
   */
  public static native void getIMUGyroRates(double[] rates);

  /**
   * Get the angle, in radians, about the axes of the IMU in the "flat" orientation.
   *
   * @param angles array of size >= 3 to place the angle data in. The data will be in the form [x,
   *     y, z].
   */
  public static native void getIMUEulerAnglesFlat(double[] angles);

  /**
   * Get the angle, in radians, about the axes of the IMU in the "landscape" orientation.
   *
   * @param angles array of size >= 3 to place the angle data in. The data will be in the form [x,
   *     y, z].
   */
  public static native void getIMUEulerAnglesLandscape(double[] angles);

  /**
   * Get the angle, in radians, about the axes of the IMU in the "portrait" orientation.
   *
   * @param angles array of size >= 3 to place the angle data in. The data will be in the form [x,
   *     y, z].
   */
  public static native void getIMUEulerAnglesPortrait(double[] angles);

  /**
   * Get the orientation of the IMU as a quaternion.
   *
   * @param quat array of size >= 4 to place the quaternion data in. The data will be in the form
   *     [w, x, y, z].
   */
  public static native void getIMUQuaternion(double[] quat);

  /**
   * Get the yaw value, in radians, of the IMU in the "flat" orientation.
   *
   * @return flat orientation yaw
   */
  public static native double getIMUYawFlat();

  /**
   * Get the yaw value, in radians, of the IMU in the "landscape" orientation.
   *
   * @return landscape orientation yaw
   */
  public static native double getIMUYawLandscape();

  /**
   * Get the yaw value, in radians, of the IMU in the "portrait" orientation.
   *
   * @return portrait orientation yaw
   */
  public static native double getIMUYawPortrait();
}
