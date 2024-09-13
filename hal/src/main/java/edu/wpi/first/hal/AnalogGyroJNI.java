// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal;

/**
 * Analog Gyro JNI Functions.
 *
 * @see "hal/AnalogGyro.h"
 */
public class AnalogGyroJNI extends JNIWrapper {
  /**
   * Initializes an analog gyro.
   *
   * @param halAnalogInputHandle handle to the analog input port
   * @return the initialized gyro handle
   * @see "HAL_InitializeAnalogGyro"
   */
  public static native int initializeAnalogGyro(int halAnalogInputHandle);

  /**
   * Sets up an analog gyro with the proper offsets and settings for the KOP analog gyro.
   *
   * @param handle the gyro handle
   * @see "HAL_SetupAnalogGyro"
   */
  public static native void setupAnalogGyro(int handle);

  /**
   * Frees an analog gyro.
   *
   * @param handle the gyro handle
   * @see "HAL_FreeAnalogGyro"
   */
  public static native void freeAnalogGyro(int handle);

  /**
   * Sets the analog gyro parameters to the specified values.
   *
   * <p>This is meant to be used if you want to reuse the values from a previous calibration.
   *
   * @param handle the gyro handle
   * @param voltsPerDegreePerSecond the gyro volts scaling
   * @param offset the gyro offset
   * @param center the gyro center
   * @see "HAL_SetAnalogGyroParameters"
   */
  public static native void setAnalogGyroParameters(
      int handle, double voltsPerDegreePerSecond, double offset, int center);

  /**
   * Sets the analog gyro volts per degrees per second scaling.
   *
   * @param handle the gyro handle
   * @param voltsPerDegreePerSecond the gyro volts scaling
   * @see "HAL_SetAnalogGyroVoltsPerDegreePerSecond"
   */
  public static native void setAnalogGyroVoltsPerDegreePerSecond(
      int handle, double voltsPerDegreePerSecond);

  /**
   * Resets the analog gyro value to 0.
   *
   * @param handle the gyro handle
   * @see "HAL_ResetAnalogGyro"
   */
  public static native void resetAnalogGyro(int handle);

  /**
   * Calibrates the analog gyro.
   *
   * <p>This happens by calculating the average value of the gyro over 5 seconds, and setting that
   * as the center. Note that this call blocks for 5 seconds to perform this.
   *
   * @param handle the gyro handle
   * @see "HAL_CalibrateAnalogGyro"
   */
  public static native void calibrateAnalogGyro(int handle);

  /**
   * Sets the deadband of the analog gyro.
   *
   * @param handle the gyro handle
   * @param volts the voltage deadband
   * @see "HAL_SetAnalogGyroDeadband"
   */
  public static native void setAnalogGyroDeadband(int handle, double volts);

  /**
   * Gets the gyro angle in degrees.
   *
   * @param handle the gyro handle
   * @return the gyro angle in degrees
   * @see "HAL_GetAnalogGyroAngle"
   */
  public static native double getAnalogGyroAngle(int handle);

  /**
   * Gets the gyro rate in degrees/second.
   *
   * @param handle the gyro handle
   * @return the gyro rate in degrees/second
   * @see "HAL_GetAnalogGyroRate"
   */
  public static native double getAnalogGyroRate(int handle);

  /**
   * Gets the calibrated gyro offset.
   *
   * <p>Can be used to not repeat a calibration but reconstruct the gyro object.
   *
   * @param handle the gyro handle
   * @return the gyro offset
   * @see "HAL_GetAnalogGyroOffset"
   */
  public static native double getAnalogGyroOffset(int handle);

  /**
   * Gets the calibrated gyro center.
   *
   * <p>Can be used to not repeat a calibration but reconstruct the gyro object.
   *
   * @param handle the gyro handle
   * @return the gyro center
   * @see "HAL_GetAnalogGyroCenter"
   */
  public static native int getAnalogGyroCenter(int handle);

  /** Utility class. */
  private AnalogGyroJNI() {}
}
