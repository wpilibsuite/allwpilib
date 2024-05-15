// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal;

/**
 * Analog Input / Output / Accumulator / Trigger JNI Functions.
 *
 * @see "hal/AnalogInput.h"
 * @see "hal/AnalogOutput.h"
 * @see "hal/AnalogAccumulator.h"
 * @see "hal/AnalogTrigger.h"
 */
public class AnalogJNI extends JNIWrapper {
  /**
   * <i>native declaration : AthenaJava\target\native\include\HAL\Analog.h:58</i><br>
   * enum values
   */
  public interface AnalogTriggerType {
    /** <i>native declaration : AthenaJava\target\native\include\HAL\Analog.h:54</i> */
    int kInWindow = 0;

    /** <i>native declaration : AthenaJava\target\native\include\HAL\Analog.h:55</i> */
    int kState = 1;

    /** <i>native declaration : AthenaJava\target\native\include\HAL\Analog.h:56</i> */
    int kRisingPulse = 2;

    /** <i>native declaration : AthenaJava\target\native\include\HAL\Analog.h:57</i> */
    int kFallingPulse = 3;
  }

  /**
   * Initializes the analog input port using the given port object.
   *
   * @param halPortHandle Handle to the port to initialize.
   * @return the created analog input handle
   * @see "HAL_InitializeAnalogInputPort"
   */
  public static native int initializeAnalogInputPort(int halPortHandle);

  /**
   * Frees an analog input port.
   *
   * @param portHandle Handle to the analog port.
   * @see "HAL_FreeAnalogInputPort"
   */
  public static native void freeAnalogInputPort(int portHandle);

  /**
   * Initializes the analog output port using the given port object.
   *
   * @param halPortHandle handle to the port
   * @return the created analog output handle
   * @see "HAL_InitializeAnalogOutputPort"
   */
  public static native int initializeAnalogOutputPort(int halPortHandle);

  /**
   * Frees an analog output port.
   *
   * @param portHandle the analog output handle
   * @see "HAL_FreeAnalogOutputPort"
   */
  public static native void freeAnalogOutputPort(int portHandle);

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
   * Checks that the analog output channel number is valid.
   *
   * <p>Verifies that the analog channel number is one of the legal channel numbers. Channel numbers
   * are 0-based.
   *
   * @param channel The analog output channel number.
   * @return Analog channel is valid
   * @see "HAL_CheckAnalogOutputChannel"
   */
  public static native boolean checkAnalogOutputChannel(int channel);

  /**
   * Indicates the analog input is used by a simulated device.
   *
   * @param handle the analog input handle
   * @param device simulated device handle
   * @see "HAL_SetAnalogInputSimDevice"
   */
  public static native void setAnalogInputSimDevice(int handle, int device);

  /**
   * Sets an analog output value.
   *
   * @param portHandle the analog output handle
   * @param voltage the voltage (0-5v) to output
   * @see "HAL_SetAnalogOutput"
   */
  public static native void setAnalogOutput(int portHandle, double voltage);

  /**
   * Gets the current analog output value.
   *
   * @param portHandle the analog output handle
   * @return the current output voltage (0-5v)
   * @see "HAL_GetAnalogOutput"
   */
  public static native double getAnalogOutput(int portHandle);

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

  /**
   * Is the channel attached to an accumulator.
   *
   * @param analogPortHandle Handle to the analog port.
   * @return The analog channel is attached to an accumulator.
   * @see "HAL_IsAccumulatorChannel"
   */
  public static native boolean isAccumulatorChannel(int analogPortHandle);

  /**
   * Initialize the accumulator.
   *
   * @param analogPortHandle Handle to the analog port.
   * @see "HAL_InitAccumulator"
   */
  public static native void initAccumulator(int analogPortHandle);

  /**
   * Resets the accumulator to the initial value.
   *
   * @param analogPortHandle Handle to the analog port.
   * @see "HAL_ResetAccumulator"
   */
  public static native void resetAccumulator(int analogPortHandle);

  /**
   * Set the center value of the accumulator.
   *
   * <p>The center value is subtracted from each A/D value before it is added to the accumulator.
   * This is used for the center value of devices like gyros and accelerometers to make integration
   * work and to take the device offset into account when integrating.
   *
   * <p>This center value is based on the output of the oversampled and averaged source from channel
   * 1. Because of this, any non-zero oversample bits will affect the size of the value for this
   * field.
   *
   * @param analogPortHandle Handle to the analog port.
   * @param center The center value of the accumulator.
   * @see "HAL_SetAccumulatorCenter"
   */
  public static native void setAccumulatorCenter(int analogPortHandle, int center);

  /**
   * Set the accumulator's deadband.
   *
   * @param analogPortHandle Handle to the analog port.
   * @param deadband The deadband of the accumulator.
   * @see "HAL_SetAccumulatorDeadband"
   */
  public static native void setAccumulatorDeadband(int analogPortHandle, int deadband);

  /**
   * Read the accumulated value.
   *
   * <p>Read the value that has been accumulating on channel 1. The accumulator is attached after
   * the oversample and average engine.
   *
   * @param analogPortHandle Handle to the analog port.
   * @return The 64-bit value accumulated since the last Reset().
   * @see "HAL_GetAccumulatorValue"
   */
  public static native long getAccumulatorValue(int analogPortHandle);

  /**
   * Read the number of accumulated values.
   *
   * <p>Read the count of the accumulated values since the accumulator was last Reset().
   *
   * @param analogPortHandle Handle to the analog port.
   * @return The number of times samples from the channel were accumulated.
   * @see "HAL_GetAccumulatorCount"
   */
  public static native int getAccumulatorCount(int analogPortHandle);

  /**
   * Read the accumulated value and the number of accumulated values atomically.
   *
   * <p>This function reads the value and count from the FPGA atomically. This can be used for
   * averaging.
   *
   * @param analogPortHandle Handle to the analog port.
   * @param result Accumulator result.
   * @see "HAL_GetAccumulatorOutput"
   */
  public static native void getAccumulatorOutput(int analogPortHandle, AccumulatorResult result);

  /**
   * Initializes an analog trigger.
   *
   * @param analogInputHandle the analog input to use for triggering
   * @return the created analog trigger handle
   * @see "HAL_InitializeAnalogTrigger"
   */
  public static native int initializeAnalogTrigger(int analogInputHandle);

  /**
   * Initializes an analog trigger with a Duty Cycle input.
   *
   * @param dutyCycleHandle the analog input to use for duty cycle
   * @return tbe created analog trigger handle
   * @see "HAL_InitializeAnalogTriggerDutyCycle"
   */
  public static native int initializeAnalogTriggerDutyCycle(int dutyCycleHandle);

  /**
   * Frees an analog trigger.
   *
   * @param analogTriggerHandle the trigger handle
   * @see "HAL_CleanAnalogTrigger"
   */
  public static native void cleanAnalogTrigger(int analogTriggerHandle);

  /**
   * Sets the raw ADC upper and lower limits of the analog trigger.
   *
   * <p>HAL_SetAnalogTriggerLimitsVoltage or HAL_SetAnalogTriggerLimitsDutyCycle is likely better in
   * most cases.
   *
   * @param analogTriggerHandle the trigger handle
   * @param lower the lower ADC value
   * @param upper the upper ADC value
   * @see "HAL_SetAnalogTriggerLimitsRaw"
   */
  public static native void setAnalogTriggerLimitsRaw(
      int analogTriggerHandle, int lower, int upper);

  /**
   * Sets the upper and lower limits of the analog trigger.
   *
   * <p>The limits are given as floating point duty cycle values.
   *
   * @param analogTriggerHandle the trigger handle
   * @param lower the lower duty cycle value
   * @param higher the upper duty cycle value
   * @see "HAL_SetAnalogTriggerLimitsDutyCycle"
   */
  public static native void setAnalogTriggerLimitsDutyCycle(
      int analogTriggerHandle, double lower, double higher);

  /**
   * Sets the upper and lower limits of the analog trigger.
   *
   * <p>The limits are given as floating point voltage values.
   *
   * @param analogTriggerHandle the trigger handle
   * @param lower the lower voltage value
   * @param upper the upper voltage value
   * @see "HAL_SetAnalogTriggerLimitsVoltage"
   */
  public static native void setAnalogTriggerLimitsVoltage(
      int analogTriggerHandle, double lower, double upper);

  /**
   * Configures the analog trigger to use the averaged vs. raw values.
   *
   * <p>If the value is true, then the averaged value is selected for the analog trigger, otherwise
   * the immediate value is used.
   *
   * <p>This is not allowed to be used if filtered mode is set. This is not allowed to be used with
   * Duty Cycle based inputs.
   *
   * @param analogTriggerHandle the trigger handle
   * @param useAveragedValue true to use averaged values, false for raw
   * @see "HAL_SetAnalogTriggerAveraged"
   */
  public static native void setAnalogTriggerAveraged(
      int analogTriggerHandle, boolean useAveragedValue);

  /**
   * Configures the analog trigger to use a filtered value.
   *
   * <p>The analog trigger will operate with a 3 point average rejection filter. This is designed to
   * help with 360 degree pot applications for the period where the pot crosses through zero.
   *
   * <p>This is not allowed to be used if averaged mode is set.
   *
   * @param analogTriggerHandle the trigger handle
   * @param useFilteredValue true to use filtered values, false for average or raw
   * @see "HAL_SetAnalogTriggerFiltered"
   */
  public static native void setAnalogTriggerFiltered(
      int analogTriggerHandle, boolean useFilteredValue);

  /**
   * Returns the InWindow output of the analog trigger.
   *
   * <p>True if the analog input is between the upper and lower limits.
   *
   * @param analogTriggerHandle the trigger handle
   * @return the InWindow output of the analog trigger
   * @see "HAL_GetAnalogTriggerInWindow"
   */
  public static native boolean getAnalogTriggerInWindow(int analogTriggerHandle);

  /**
   * Returns the TriggerState output of the analog trigger.
   *
   * <p>True if above upper limit. False if below lower limit. If in Hysteresis, maintain previous
   * state.
   *
   * @param analogTriggerHandle the trigger handle
   * @return the TriggerState output of the analog trigger
   * @see "HAL_GetAnalogTriggerTriggerState"
   */
  public static native boolean getAnalogTriggerTriggerState(int analogTriggerHandle);

  /**
   * Gets the state of the analog trigger output.
   *
   * @param analogTriggerHandle the trigger handle
   * @param type the type of trigger to trigger on
   * @return the state of the analog trigger output
   * @see "HAL_GetAnalogTriggerOutput"
   */
  public static native boolean getAnalogTriggerOutput(int analogTriggerHandle, int type);

  /**
   * Get the FPGA index for the AnalogTrigger.
   *
   * @param analogTriggerHandle the trigger handle
   * @return the FPGA index
   * @see "HAL_GetAnalogTriggerFPGAIndex"
   */
  public static native int getAnalogTriggerFPGAIndex(int analogTriggerHandle);

  /** Utility class. */
  private AnalogJNI() {}
}
