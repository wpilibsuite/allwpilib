/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2012. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import edu.wpi.first.wpilibj.parsing.IUtility;

/**
 * Timer objects measure accumulated time in milliseconds.
 * The timer object functions like a stopwatch. It can be started, stopped, and cleared. When the
 * timer is running its value counts up in milliseconds. When stopped, the timer holds the current
 * value. The implementation simply records the time when started and subtracts the current time
 * whenever the value is requested.
 */
public class Timer implements IUtility {

    private long m_startTime;
    private double m_accumulatedTime;
    private boolean m_running;

    /**
     * Pause the thread for a specified time. Pause the execution of the
     * thread for a specified period of time given in seconds. Motors will
     * continue to run at their last assigned values, and sensors will continue
     * to update. Only the task containing the wait will pause until the wait
     * time is expired.
     *
     * @param seconds Length of time to pause
     */
    public static void delay(final double seconds) {
        try {
            Thread.sleep((long) (seconds * 1e3));
        } catch (final InterruptedException e) {
        }
    }

    /**
     * Return the system clock time in microseconds. Return the time from the
     * FPGA hardware clock in microseconds since the FPGA started.
     *
     * @deprecated Use getFPGATimestamp instead.
     * @return Robot running time in microseconds.
     */
    public static long getUsClock() {
        return Utility.getFPGATime();
    }

    /**
     * Return the system clock time in milliseconds. Return the time from the
     * FPGA hardware clock in milliseconds since the FPGA started.
     *
     * @deprecated Use getFPGATimestamp instead.
     * @return Robot running time in milliseconds.
     */
    static long getMsClock() {
        return getUsClock() / 1000;
    }

    /**
     * Return the system clock time in seconds. Return the time from the
     * FPGA hardware clock in seconds since the FPGA started.
     *
     * @return Robot running time in seconds.
     */
    public static double getFPGATimestamp() {
        return Utility.getFPGATime() / 1000000.0;
    }

    /**
     * Create a new timer object.
     * Create a new timer object and reset the time to zero. The timer is initially not running and
     * must be started.
     */
    public Timer() {
        reset();
    }

    /**
     * Get the current time from the timer. If the clock is running it is derived from
     * the current system clock the start time stored in the timer class. If the clock
     * is not running, then return the time when it was last stopped.
     *
     * @return Current time value for this timer in seconds
     */
    public synchronized double get() {
        if (m_running) {
            return ((double) ((getMsClock() - m_startTime) + m_accumulatedTime)) / 1000.0;
        } else {
            return m_accumulatedTime;
        }
    }

    /**
     * Reset the timer by setting the time to 0.
     * Make the timer startTime the current time so new requests will be relative now
     */
    public synchronized void reset() {
        m_accumulatedTime = 0;
        m_startTime = getMsClock();
    }

    /**
     * Start the timer running.
     * Just set the running flag to true indicating that all time requests should be
     * relative to the system clock.
     */
    public synchronized void start() {
        m_startTime = getMsClock();
        m_running = true;
    }

    /**
     * Stop the timer.
     * This computes the time as of now and clears the running flag, causing all
     * subsequent time requests to be read from the accumulated time rather than
     * looking at the system clock.
     */
    public synchronized void stop() {
        final double temp = get();
        m_accumulatedTime = temp;
        m_running = false;
    }
}
