// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.hardware.rotation;

import org.wpilib.hardware.hal.DutyCycleJNI;
import org.wpilib.hardware.hal.HAL;
import org.wpilib.telemetry.TelemetryLoggable;
import org.wpilib.telemetry.TelemetryTable;

/**
 * Class to read a duty cycle PWM input.
 *
 * <p>PWM input signals are specified with a frequency and a ratio of high to low in that frequency.
 * There are 8 of these in the roboRIO, and they can be attached to any SmartIO Channel.
 *
 * <p>These can be combined as the input of an AnalogTrigger to a Counter in order to implement
 * rollover checking.
 */
public class DutyCycle implements TelemetryLoggable, AutoCloseable {
  // Explicitly package private
  final int m_handle;
  private final int m_channel;

  /**
   * Constructs a DutyCycle input from a smartio channel.
   *
   * @param channel The channel to use.
   */
  public DutyCycle(int channel) {
    m_handle = DutyCycleJNI.initialize(channel);

    m_channel = channel;
    HAL.reportUsage("IO", channel, "DutyCycle");
  }

  /** Close the DutyCycle and free all resources. */
  @Override
  public void close() {
    DutyCycleJNI.free(m_handle);
  }

  /**
   * Get the frequency of the duty cycle signal.
   *
   * @return frequency in Hertz
   */
  public double getFrequency() {
    return DutyCycleJNI.getFrequency(m_handle);
  }

  /**
   * Get the output ratio of the duty cycle signal.
   *
   * <p>0 means always low, 1 means always high.
   *
   * @return output ratio between 0 and 1
   */
  public double getOutput() {
    return DutyCycleJNI.getOutput(m_handle);
  }

  /**
   * Get the raw high time of the duty cycle signal.
   *
   * @return high time of last pulse in nanoseconds
   */
  public int getHighTimeNanoseconds() {
    return DutyCycleJNI.getHighTime(m_handle);
  }

  /**
   * Get the channel of the source.
   *
   * @return the source channel
   */
  public int getSourceChannel() {
    return m_channel;
  }

  @Override
  public void logTo(TelemetryTable table) {
    table.log("Frequency", getFrequency());
    table.log("Output", getOutput());
  }

  @Override
  public String getTelemetryType() {
    return "Duty Cycle";
  }
}
