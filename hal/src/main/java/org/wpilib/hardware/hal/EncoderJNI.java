// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.hardware.hal;

/**
 * Encoder JNI Functions.
 *
 * @see "hal/Encoder.h"
 */
public class EncoderJNI extends JNIWrapper {
  /**
   * Initializes an encoder.
   *
   * @param aChannel the A channel
   * @param bChannel the B channel
   * @param reverseDirection true to reverse the counting direction from standard, otherwise false
   * @param encodingType the encoding type
   * @return the created encoder handle
   * @see "HAL_InitializeEncoder"
   */
  public static native int initializeEncoder(
      int aChannel, int bChannel, boolean reverseDirection, int encodingType);

  /**
   * Frees an encoder.
   *
   * @param encoderHandle the encoder handle
   * @see "HAL_FreeEncoder"
   */
  public static native void freeEncoder(int encoderHandle);

  /**
   * Indicates the encoder is used by a simulated device.
   *
   * @param handle the encoder handle
   * @param device simulated device handle
   * @see "HAL_SetEncoderSimDevice"
   */
  public static native void setEncoderSimDevice(int handle, int device);

  /**
   * Gets the current counts of the encoder after encoding type scaling.
   *
   * <p>This is scaled by the value passed during initialization to encodingType.
   *
   * @param encoderHandle the encoder handle
   * @return the current scaled count
   * @see "HAL_GetEncoder"
   */
  public static native int getEncoder(int encoderHandle);

  /**
   * Gets the raw counts of the encoder.
   *
   * <p>This is not scaled by any values.
   *
   * @param encoderHandle the encoder handle
   * @return the raw encoder count
   * @see "HAL_GetEncoderRaw"
   */
  public static native int getEncoderRaw(int encoderHandle);

  /**
   * Gets the encoder scale value.
   *
   * <p>This is set by the value passed during initialization to encodingType.
   *
   * @param encoderHandle the encoder handle
   * @return the encoder scale value
   * @see "HAL_GetEncoderEncodingScale"
   */
  public static native int getEncodingScaleFactor(int encoderHandle);

  /**
   * Reads the current encoder value.
   *
   * <p>Read the value at this instant. It may still be running, so it reflects the current value.
   * Next time it is read, it might have a different value.
   *
   * @param encoderHandle the encoder handle
   * @see "HAL_ResetEncoder"
   */
  public static native void resetEncoder(int encoderHandle);

  /**
   * Gets the Period of the most recent count.
   *
   * <p>Returns the time interval of the most recent count. This can be used for velocity
   * calculations to determine shaft speed.
   *
   * @param encoderHandle the encoder handle
   * @return the period of the last two pulses in units of seconds
   * @see "HAL_GetEncoderPeriod"
   */
  public static native double getEncoderPeriod(int encoderHandle);

  /**
   * Sets the maximum period where the device is still considered "moving".
   *
   * <p>Sets the maximum period where the device is considered moving. This value is used to
   * determine the "stopped" state of the encoder using the getEncoderStopped method.
   *
   * @param encoderHandle the encoder handle
   * @param maxPeriod the maximum period where the counted device is considered moving in seconds
   * @see "HAL_SetEncoderMaxPeriod"
   */
  public static native void setEncoderMaxPeriod(int encoderHandle, double maxPeriod);

  /**
   * Determines if the clock is stopped.
   *
   * <p>Determines if the clocked input is stopped based on the MaxPeriod value set using the
   * SetMaxPeriod method. If the clock exceeds the MaxPeriod, then the device (and encoder) are
   * assumed to be stopped and it returns true.
   *
   * @param encoderHandle the encoder handle
   * @return true if the most recent encoder period exceeds the MaxPeriod value set by SetMaxPeriod
   * @see "HAL_GetEncoderStopped"
   */
  public static native boolean getEncoderStopped(int encoderHandle);

  /**
   * Gets the last direction the encoder value changed.
   *
   * @param encoderHandle the encoder handle
   * @return the last direction the encoder value changed
   * @see "HAL_GetEncoderDirection"
   */
  public static native boolean getEncoderDirection(int encoderHandle);

  /**
   * Gets the current distance traveled by the encoder.
   *
   * <p>This is the encoder count scaled by the distance per pulse set for the encoder.
   *
   * @param encoderHandle the encoder handle
   * @return the encoder distance (units are determined by the units passed to
   *     setEncoderDistancePerPulse)
   * @see "HAL_GetEncoderDistance"
   */
  public static native double getEncoderDistance(int encoderHandle);

  /**
   * Gets the current rate of the encoder.
   *
   * <p>This is the encoder period scaled by the distance per pulse set for the encoder.
   *
   * @param encoderHandle the encoder handle
   * @return the encoder rate (units are determined by the units passed to
   *     setEncoderDistancePerPulse, time value is seconds)
   * @see "HAL_GetEncoderRate"
   */
  public static native double getEncoderRate(int encoderHandle);

  /**
   * Sets the minimum rate to be considered moving by the encoder.
   *
   * <p>Units need to match what is set by setEncoderDistancePerPulse, with time as seconds.
   *
   * @param encoderHandle the encoder handle
   * @param minRate the minimum rate to be considered moving (units are determined by the units
   *     passed to setEncoderDistancePerPulse, time value is seconds)
   * @see "HAL_SetEncoderMinRate"
   */
  public static native void setEncoderMinRate(int encoderHandle, double minRate);

  /**
   * Sets the distance traveled per encoder pulse. This is used as a scaling factor for the rate and
   * distance calls.
   *
   * @param encoderHandle the encoder handle
   * @param distancePerPulse the distance traveled per encoder pulse (units user defined)
   * @see "HAL_SetEncoderDistancePerPulse"
   */
  public static native void setEncoderDistancePerPulse(int encoderHandle, double distancePerPulse);

  /**
   * Sets if to reverse the direction of the encoder.
   *
   * <p>Note that this is not a toggle. It is an absolute set.
   *
   * @param encoderHandle the encoder handle
   * @param reverseDirection true to reverse the direction, false to not.
   * @see "HAL_SetEncoderReverseDirection"
   */
  public static native void setEncoderReverseDirection(int encoderHandle, boolean reverseDirection);

  /**
   * Sets the number of encoder samples to average when calculating encoder rate.
   *
   * @param encoderHandle the encoder handle
   * @param samplesToAverage the number of samples to average
   * @see "HAL_SetEncoderSamplesToAverage"
   */
  public static native void setEncoderSamplesToAverage(int encoderHandle, int samplesToAverage);

  /**
   * Gets the current samples to average value.
   *
   * @param encoderHandle the encoder handle
   * @return the current samples to average value
   * @see "HAL_GetEncoderSamplesToAverage"
   */
  public static native int getEncoderSamplesToAverage(int encoderHandle);

  /**
   * Gets the FPGA index of the encoder.
   *
   * @param encoderHandle the encoder handle
   * @return the FPGA index of the encoder
   * @see "HAL_GetEncoderFPGAIndex"
   */
  public static native int getEncoderFPGAIndex(int encoderHandle);

  /**
   * Gets the encoder scale value.
   *
   * <p>This is set by the value passed during initialization to encodingType.
   *
   * @param encoderHandle the encoder handle
   * @return the encoder scale value
   * @see "HAL_GetEncoderEncodingScale"
   */
  public static native int getEncoderEncodingScale(int encoderHandle);

  /**
   * Gets the decoding scale factor of the encoder.
   *
   * <p>This is used to perform the scaling from raw to type scaled values.
   *
   * @param encoderHandle the encoder handle
   * @return the scale value for the encoder
   * @see "HAL_GetEncoderDecodingScaleFactor"
   */
  public static native double getEncoderDecodingScaleFactor(int encoderHandle);

  /**
   * Gets the user set distance per pulse of the encoder.
   *
   * @param encoderHandle the encoder handle
   * @return the set distance per pulse
   * @see "HAL_GetEncoderDistancePerPulse"
   */
  public static native double getEncoderDistancePerPulse(int encoderHandle);

  /**
   * Gets the encoding type of the encoder.
   *
   * @param encoderHandle the encoder handle
   * @return the encoding type
   * @see "HAL_GetEncoderEncodingType"
   */
  public static native int getEncoderEncodingType(int encoderHandle);

  /** Utility class. */
  private EncoderJNI() {}
}
