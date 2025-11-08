// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.hardware.hal;

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
   * Sets the sample rate.
   *
   * <p>This is a global setting for the Athena and effects all channels.
   *
   * @param samplesPerSecond The number of samples per channel per second.
   * @see "HAL_SetAnalogSampleRate"
   */
  public static native void setAnalogSampleRate(double samplesPerSecond);

  /**
   * Gets the current sample rate.
   *
   * <p>This assumes one entry in the scan list. This is a global setting for the Athena and effects
   * all channels.
   *
   * @return Sample rate.
   * @see "HAL_GetAnalogSampleRate"
   */
  public static native double getAnalogSampleRate();

  /**
   * Sets the number of averaging bits.
   *
   * <p>This sets the number of averaging bits. The actual number of averaged samples is 2**bits.
   * Use averaging to improve the stability of your measurement at the expense of sampling rate. The
   * averaging is done automatically in the FPGA.
   *
   * @param analogPortHandle Handle to the analog port to configure.
   * @param bits Number of bits to average.
   * @see "HAL_SetAnalogAverageBits"
   */
  public static native void setAnalogAverageBits(int analogPortHandle, int bits);

  /**
   * Gets the number of averaging bits.
   *
   * <p>This gets the number of averaging bits from the FPGA. The actual number of averaged samples
   * is 2**bits. The averaging is done automatically in the FPGA.
   *
   * @param analogPortHandle Handle to the analog port to use.
   * @return Bits to average.
   * @see "HAL_GetAnalogAverageBits"
   */
  public static native int getAnalogAverageBits(int analogPortHandle);

  /**
   * Sets the number of oversample bits.
   *
   * <p>This sets the number of oversample bits. The actual number of oversampled values is 2**bits.
   * Use oversampling to improve the resolution of your measurements at the expense of sampling
   * rate. The oversampling is done automatically in the FPGA.
   *
   * @param analogPortHandle Handle to the analog port to use.
   * @param bits Number of bits to oversample.
   * @see "HAL_SetAnalogOversampleBits"
   */
  public static native void setAnalogOversampleBits(int analogPortHandle, int bits);

  /**
   * Gets the number of oversample bits.
   *
   * <p>This gets the number of oversample bits from the FPGA. The actual number of oversampled
   * values is 2**bits. The oversampling is done automatically in the FPGA.
   *
   * @param analogPortHandle Handle to the analog port to use.
   * @return Bits to oversample.
   * @see "HAL_GetAnalogOversampleBits"
   */
  public static native int getAnalogOversampleBits(int analogPortHandle);

  /**
   * Gets a sample straight from the channel on this module.
   *
   * <p>The sample is a 12-bit value representing the 0V to 3.3V range of the A/D converter in the
   * module. The units are in A/D converter codes. Use GetVoltage() to get the analog value in
   * calibrated units.
   *
   * @param analogPortHandle Handle to the analog port to use.
   * @return A sample straight from the channel on this module.
   * @see "HAL_GetAnalogValue"
   */
  public static native short getAnalogValue(int analogPortHandle);

  /**
   * Gets a sample from the output of the oversample and average engine for the channel.
   *
   * <p>The sample is 12-bit + the value configured in SetOversampleBits(). The value configured in
   * SetAverageBits() will cause this value to be averaged 2**bits number of samples. This is not a
   * sliding window. The sample will not change until 2**(OversampleBits + AverageBits) samples have
   * been acquired from the module on this channel. Use GetAverageVoltage() to get the analog value
   * in calibrated units.
   *
   * @param analogPortHandle Handle to the analog port to use.
   * @return A sample from the oversample and average engine for the channel.
   * @see "HAL_GetAnalogAverageValue"
   */
  public static native int getAnalogAverageValue(int analogPortHandle);

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

  /**
   * Gets a scaled sample from the output of the oversample and average engine for the channel.
   *
   * <p>The value is scaled to units of Volts using the calibrated scaling data from GetLSBWeight()
   * and GetOffset(). Using oversampling will cause this value to be higher resolution, but it will
   * update more slowly. Using averaging will cause this value to be more stable, but it will update
   * more slowly.
   *
   * @param analogPortHandle Handle to the analog port to use.
   * @return A scaled sample from the output of the oversample and average engine for the channel.
   * @see "HAL_GetAnalogAverageVoltage"
   */
  public static native double getAnalogAverageVoltage(int analogPortHandle);

  /**
   * Gets the factory scaling least significant bit weight constant. The least significant bit
   * weight constant for the channel that was calibrated in manufacturing and stored in an eeprom in
   * the module.
   *
   * <p>Volts = ((LSB_Weight * 1e-9) * raw) - (Offset * 1e-9)
   *
   * @param analogPortHandle Handle to the analog port to use.
   * @return Least significant bit weight.
   * @see "HAL_GetAnalogLSBWeight"
   */
  public static native int getAnalogLSBWeight(int analogPortHandle);

  /**
   * Gets the factory scaling offset constant. The offset constant for the channel that was
   * calibrated in manufacturing and stored in an eeprom in the module.
   *
   * <p>Volts = ((LSB_Weight * 1e-9) * raw) - (Offset * 1e-9)
   *
   * @param analogPortHandle Handle to the analog port to use.
   * @return Offset constant.
   * @see "HAL_GetAnalogOffset"
   */
  public static native int getAnalogOffset(int analogPortHandle);

  /** Utility class. */
  private AnalogJNI() {}
}
