// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

/**
 * Interface for counting the number of ticks on a digital input channel. Encoders, Gear tooth
 * sensors, and counters should all subclass this in order to build more advanced classes for
 * control and driving.
 *
 * <p>All counters will immediately start counting - reset() them if you need them to be zeroed
 * before use.
 */
public interface CounterBase {
  /** The number of edges for the CounterBase to increment or decrement on. */
  enum EncodingType {
    /** Count only the rising edge. */
    k1X(0),
    /** Count both the rising and falling edge. */
    k2X(1),
    /** Count rising and falling on both channels. */
    k4X(2);

    /** EncodingType value. */
    public final int value;

    EncodingType(int value) {
      this.value = value;
    }
  }

  /**
   * Get the count.
   *
   * @return the count
   */
  int get();

  /** Reset the count to zero. */
  void reset();

  /**
   * Get the time between the last two edges counted.
   *
   * @return the time between the last two ticks in seconds
   */
  double getPeriod();

  /**
   * Set the maximum time between edges to be considered stalled.
   *
   * @param maxPeriod the maximum period in seconds
   */
  void setMaxPeriod(double maxPeriod);

  /**
   * Determine if the counter is not moving.
   *
   * @return true if the counter has not changed for the max period
   */
  boolean getStopped();

  /**
   * Determine which direction the counter is going.
   *
   * @return true for one direction, false for the other
   */
  boolean getDirection();
}
