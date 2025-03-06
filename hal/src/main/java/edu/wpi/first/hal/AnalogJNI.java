// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal;

/**
 * Analog Input / Output / Trigger JNI Functions.
 *
 * @see "hal/AnalogInput.h"
 */
public class AnalogJNI extends JNIWrapper {
  /**
   * Initializes the analog input port using the given port object.
   *
   * @param channel The smartio channel.
   * @return the created analog input handle
   * @see "HAL_InitializeAnalogInputPort"
   */
  public static native int initializeAnalogInputPort(int channel);

  /**
   * Frees an analog input port.
   *
   * @param analogPortHandle Handle to the analog port.
   * @see "HAL_FreeAnalogInputPort"
   */
  public static native void freeAnalogInputPort(int analogPortHandle);

  /**
   * Checks that the analog module number is valid.
   *
   * @param module The analog module number.
   * @return Analog module is valid and present
   * @see "HAL_CheckAnalogModule"
   */
  public static native boolean checkAnalogModule(byte module);

  /**
   * Checks that the analog output channel number is valid. Verifies that the analog channel number
   * is one of the legal channel numbers. Channel numbers are 0-based.
   *
   * @param channel The analog output channel number.
   * @return Analog channel is valid
   * @see "HAL_CheckAnalogInputChannel"
   */
  public static native boolean checkAnalogInputChannel(int channel);

  /**
   * Indicates the analog input is used by a simulated device.
   *
   * @param handle the analog input handle
   * @param device simulated device handle
   * @see "HAL_SetAnalogInputSimDevice"
   */
  public static native void setAnalogInputSimDevice(int handle, int device);

  /**
   * Gets a sample straight from the channel on this module.
   *
   * <p>The sample is a 12-bit value representing the 0V to 5V range of the A/D converter in the
   * module. The units are in A/D converter codes. Use GetVoltage() to get the analog value in
   * calibrated units.
   *
   * @param analogPortHandle Handle to the analog port to use.
   * @return A sample straight from the channel on this module.
   * @see "HAL_GetAnalogValue"
   */
  public static native short getAnalogValue(int analogPortHandle);

  /**
   * Converts a voltage to a raw value for a specified channel.
   *
   * <p>This process depends on the calibration of each channel, so the channel must be specified.
   *
   * <p>todo This assumes raw values. Oversampling not supported as is.
   *
   * @param analogPortHandle Handle to the analog port to use.
   * @param voltage The voltage to convert.
   * @return The raw value for the channel.
   * @see "HAL_GetAnalogVoltsToValue"
   */
  public static native int getAnalogVoltsToValue(int analogPortHandle, double voltage);

  /**
   * Get the analog voltage from a raw value.
   *
   * @param analogPortHandle Handle to the analog port the values were read from.
   * @param value The raw analog value
   * @return The voltage relating to the value
   * @see "HAL_GetAnalogValueToVolts"
   */
  public static native double getAnalogValueToVolts(int analogPortHandle, int value);

  /**
   * Gets a scaled sample straight from the channel on this module.
   *
   * <p>The value is scaled to units of Volts using the calibrated scaling data from GetLSBWeight()
   * and GetOffset().
   *
   * @param analogPortHandle Handle to the analog port to use.
   * @return A scaled sample straight from the channel on this module.
   * @see "HAL_GetAnalogVoltage"
   */
  public static native double getAnalogVoltage(int analogPortHandle);

  /** Utility class. */
  private AnalogJNI() {}
}
