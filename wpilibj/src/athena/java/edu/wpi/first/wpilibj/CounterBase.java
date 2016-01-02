/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

/**
 * Interface for counting the number of ticks on a digital input channel.
 * Encoders, Gear tooth sensors, and counters should all subclass this so it can
 * be used to build more advanced classes for control and driving.
 *
 * All counters will immediately start counting - reset() them if you need them
 * to be zeroed before use.
 */
public interface CounterBase {

  /**
   * The number of edges for the counterbase to increment or decrement on
   */
  public enum EncodingType {
    /**
     * Count only the rising edge
     */
    k1X(0),
    /**
     * Count both the rising and falling edge
     */
    k2X(1),
    /**
     * Count rising and falling on both channels
     */
    k4X(2);

    /**
     * The integer value representing this enumeration
     */
    public final int value;

    private EncodingType(int value) {
      this.value = value;
    }
  }

  /**
   * Get the count
   *$
   * @return the count
   */
  int get();

  /**
   * Reset the count to zero
   */
  void reset();

  /**
   * Get the time between the last two edges counted
   *$
   * @return the time beteween the last two ticks in seconds
   */
  double getPeriod();

  /**
   * Set the maximum time between edges to be considered stalled
   *$
   * @param maxPeriod the maximum period in seconds
   */
  void setMaxPeriod(double maxPeriod);

  /**
   * Determine if the counter is not moving
   *$
   * @return true if the counter has not changed for the max period
   */
  boolean getStopped();

  /**
   * Determine which direction the counter is going
   *$
   * @return true for one direction, false for the other
   */
  boolean getDirection();
}
