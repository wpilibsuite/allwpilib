// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal;

import java.nio.IntBuffer;

/**
 * Counter HAL JNI functions.
 *
 * @see "hal/Counter.h"
 */
public class CounterJNI extends JNIWrapper {
  /** Two pulse mode. */
  public static final int TWO_PULSE = 0;

  /** Semi-period mode. */
  public static final int SEMI_PERIOD = 1;

  /** Pulse length mode. */
  public static final int PULSE_LENGTH = 2;

  /** External direction mode. */
  public static final int EXTERNAL_DIRECTION = 3;

  /**
   * Initializes a counter.
   *
   * @param mode the counter mode
   * @param index the compressor index (output)
   * @return the created handle
   * @see "HAL_InitializeCounter"
   */
  public static native int initializeCounter(int mode, IntBuffer index);

  /**
   * Frees a counter.
   *
   * @param counterHandle the counter handle
   * @see "HAL_FreeCounter"
   */
  public static native void freeCounter(int counterHandle);

  /**
   * Sets the average sample size of a counter.
   *
   * @param counterHandle the counter handle
   * @param size the size of samples to average
   * @see "HAL_SetCounterAverageSize"
   */
  public static native void setCounterAverageSize(int counterHandle, int size);

  /**
   * Sets the source object that causes the counter to count up.
   *
   * @param counterHandle the counter handle
   * @param digitalSourceHandle the digital source handle (either a HAL_AnalogTriggerHandle or a
   *     HAL_DigitalHandle)
   * @param analogTriggerType the analog trigger type if the source is an analog trigger
   * @see "HAL_SetCounterUpSource"
   */
  public static native void setCounterUpSource(
      int counterHandle, int digitalSourceHandle, int analogTriggerType);

  /**
   * Sets the up source to either detect rising edges or falling edges.
   *
   * <p>Note that both are allowed to be set true at the same time without issues.
   *
   * @param counterHandle the counter handle
   * @param risingEdge true to trigger on rising
   * @param fallingEdge true to trigger on falling
   * @see "HAL_SetCounterUpSourceEdge"
   */
  public static native void setCounterUpSourceEdge(
      int counterHandle, boolean risingEdge, boolean fallingEdge);

  /**
   * Disables the up counting source to the counter.
   *
   * @param counterHandle the counter handle
   * @see "HAL_ClearCounterUpSource"
   */
  public static native void clearCounterUpSource(int counterHandle);

  /**
   * Sets the source object that causes the counter to count down.
   *
   * @param counterHandle the counter handle
   * @param digitalSourceHandle the digital source handle (either a HAL_AnalogTriggerHandle or a
   *     HAL_DigitalHandle)
   * @param analogTriggerType the analog trigger type if the source is an analog trigger
   * @see "HAL_SetCounterDownSource"
   */
  public static native void setCounterDownSource(
      int counterHandle, int digitalSourceHandle, int analogTriggerType);

  /**
   * Sets the down source to either detect rising edges or falling edges. Note that both are allowed
   * to be set true at the same time without issues.
   *
   * @param counterHandle the counter handle
   * @param risingEdge true to trigger on rising
   * @param fallingEdge true to trigger on falling
   * @see "HAL_SetCounterDownSourceEdge"
   */
  public static native void setCounterDownSourceEdge(
      int counterHandle, boolean risingEdge, boolean fallingEdge);

  /**
   * Disables the down counting source to the counter.
   *
   * @param counterHandle the counter handle
   * @see "HAL_ClearCounterDownSource"
   */
  public static native void clearCounterDownSource(int counterHandle);

  /**
   * Sets standard up / down counting mode on this counter.
   *
   * <p>Up and down counts are sourced independently from two inputs.
   *
   * @param counterHandle the counter handle
   * @see "HAL_SetCounterUpDownMode"
   */
  public static native void setCounterUpDownMode(int counterHandle);

  /**
   * Sets directional counting mode on this counter.
   *
   * <p>The direction is determined by the B input, with counting happening with the A input.
   *
   * @param counterHandle the counter handle
   * @see "HAL_SetCounterExternalDirectionMode"
   */
  public static native void setCounterExternalDirectionMode(int counterHandle);

  /**
   * Sets Semi-period mode on this counter.
   *
   * <p>The counter counts up based on the time the input is triggered. High or Low depends on the
   * highSemiPeriod parameter.
   *
   * @param counterHandle the counter handle
   * @param highSemiPeriod true for counting when the input is high, false for low
   * @see "HAL_SetCounterSemiPeriodMode"
   */
  public static native void setCounterSemiPeriodMode(int counterHandle, boolean highSemiPeriod);

  /**
   * Configures the counter to count in up or down based on the length of the input pulse.
   *
   * <p>This mode is most useful for direction sensitive gear tooth sensors.
   *
   * @param counterHandle the counter handle
   * @param threshold The pulse length beyond which the counter counts the opposite direction
   *     (seconds)
   * @see "HAL_SetCounterPulseLengthMode"
   */
  public static native void setCounterPulseLengthMode(int counterHandle, double threshold);

  /**
   * Gets the Samples to Average which specifies the number of samples of the timer to average when
   * calculating the period. Perform averaging to account for mechanical imperfections or as
   * oversampling to increase resolution.
   *
   * @param counterHandle the counter handle
   * @return SamplesToAverage The number of samples being averaged (from 1 to 127)
   * @see "HAL_GetCounterSamplesToAverage"
   */
  public static native int getCounterSamplesToAverage(int counterHandle);

  /**
   * Sets the Samples to Average which specifies the number of samples of the timer to average when
   * calculating the period. Perform averaging to account for mechanical imperfections or as
   * oversampling to increase resolution.
   *
   * @param counterHandle the counter handle
   * @param samplesToAverage The number of samples to average from 1 to 127
   * @see "HAL_SetCounterSamplesToAverage"
   */
  public static native void setCounterSamplesToAverage(int counterHandle, int samplesToAverage);

  /**
   * Resets the Counter to zero.
   *
   * <p>Sets the counter value to zero. This does not effect the running state of the counter, just
   * sets the current value to zero.
   *
   * @param counterHandle the counter handle
   * @see "HAL_ResetCounter"
   */
  public static native void resetCounter(int counterHandle);

  /**
   * Reads the current counter value.
   *
   * <p>Reads the value at this instant. It may still be running, so it reflects the current value.
   * Next time it is read, it might have a different value.
   *
   * @param counterHandle the counter handle
   * @return the current counter value
   * @see "HAL_GetCounter"
   */
  public static native int getCounter(int counterHandle);

  /**
   * Gets the Period of the most recent count.
   *
   * <p>Returns the time interval of the most recent count. This can be used for velocity
   * calculations to determine shaft speed.
   *
   * @param counterHandle the counter handle
   * @return the period of the last two pulses in units of seconds
   * @see "HAL_GetCounterPeriod"
   */
  public static native double getCounterPeriod(int counterHandle);

  /**
   * Sets the maximum period where the device is still considered "moving".
   *
   * <p>Sets the maximum period where the device is considered moving. This value is used to
   * determine the "stopped" state of the counter using the HAL_GetCounterStopped method.
   *
   * @param counterHandle the counter handle
   * @param maxPeriod the maximum period where the counted device is considered moving in seconds
   * @see "HAL_SetCounterMaxPeriod"
   */
  public static native void setCounterMaxPeriod(int counterHandle, double maxPeriod);

  /**
   * Selects whether you want to continue updating the event timer output when there are no samples
   * captured.
   *
   * <p>The output of the event timer has a buffer of periods that are averaged and posted to a
   * register on the FPGA. When the timer detects that the event source has stopped (based on the
   * MaxPeriod) the buffer of samples to be averaged is emptied.
   *
   * <p>If you enable the update when empty, you will be notified of the stopped source and the
   * event time will report 0 samples.
   *
   * <p>If you disable update when empty, the most recent average will remain on the output until a
   * new sample is acquired.
   *
   * <p>You will never see 0 samples output (except when there have been no events since an FPGA
   * reset) and you will likely not see the stopped bit become true (since it is updated at the end
   * of an average and there are no samples to average).
   *
   * @param counterHandle the counter handle
   * @param enabled true to enable counter updating with no samples
   * @see "HAL_SetCounterUpdateWhenEmpty"
   */
  public static native void setCounterUpdateWhenEmpty(int counterHandle, boolean enabled);

  /**
   * Determines if the clock is stopped.
   *
   * <p>Determine if the clocked input is stopped based on the MaxPeriod value set using the
   * SetMaxPeriod method. If the clock exceeds the MaxPeriod, then the device (and counter) are
   * assumed to be stopped and it returns true.
   *
   * @param counterHandle the counter handle
   * @return true if the most recent counter period exceeds the MaxPeriod value set by SetMaxPeriod
   * @see "HAL_GetCounterStopped"
   */
  public static native boolean getCounterStopped(int counterHandle);

  /**
   * Gets the last direction the counter value changed.
   *
   * @param counterHandle the counter handle
   * @return the last direction the counter value changed
   * @see "HAL_GetCounterDirection"
   */
  public static native boolean getCounterDirection(int counterHandle);

  /**
   * Sets the Counter to return reversed sensing on the direction.
   *
   * <p>This allows counters to change the direction they are counting in the case of 1X and 2X
   * quadrature encoding only. Any other counter mode isn't supported.
   *
   * @param counterHandle the counter handle
   * @param reverseDirection true if the value counted should be negated.
   * @see "HAL_SetCounterReverseDirection"
   */
  public static native void setCounterReverseDirection(int counterHandle, boolean reverseDirection);

  /** Utility class. */
  private CounterJNI() {}
}
