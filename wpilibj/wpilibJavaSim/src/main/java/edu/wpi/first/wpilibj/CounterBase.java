/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2012. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

/**
 * Interface for counting the number of ticks on a digital input channel.
 * Encoders, Gear tooth sensors, and counters should all subclass this so it can be used to
 * build more advanced classes for control and driving.
 */
public interface CounterBase {

    /**
     * The number of edges for the counterbase to increment or decrement on
     */
    public static class EncodingType {

        /**
         * The integer value representing this enumeration
         */
        public final int value;
        static final int k1X_val = 0;
        static final int k2X_val = 1;
        static final int k4X_val = 2;
        /**
         * Count only the rising edge
         */
        public static final EncodingType k1X = new EncodingType(k1X_val);
        /**
         * Count both the rising and falling edge
         */
        public static final EncodingType k2X = new EncodingType(k2X_val);
        /**
         * Count rising and falling on both channels
         */
        public static final EncodingType k4X = new EncodingType(k4X_val);

        private EncodingType(int value) {
            this.value = value;
        }
    }

    /**
     * Start the counter
     */
    public void start();

    /**
     * Get the count
     * @return the count
     */
    int get();

    /**
     * Reset the count to zero
     */
    void reset();

    /**
     * Stop counting
     */
    void stop();

    /**
     * Get the time between the last two edges counted
     * @return the time beteween the last two ticks in seconds
     */
    double getPeriod();

    /**
     * Set the maximum time between edges to be considered stalled
     * @param maxPeriod the maximum period in seconds
     */
    void setMaxPeriod(double maxPeriod);

    /**
     * Determine if the counter is not moving
     * @return true if the counter has not changed for the max period
     */
    boolean getStopped();

    /**
     * Determine which direction the counter is going
     * @return true for one direction, false for the other
     */
    boolean getDirection();
}
