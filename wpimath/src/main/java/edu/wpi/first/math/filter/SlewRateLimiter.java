// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.filter;

import edu.wpi.first.math.MathUtil;
import edu.wpi.first.util.WPIUtilJNI;

/**
 * A class that limits the rate of change of an input value. Useful for implementing voltage,
 * setpoint, and/or output ramps. A slew-rate limit is most appropriate when the quantity being
 * controlled is a velocity or a voltage; when controlling a position, consider using a {@link
 * edu.wpi.first.math.trajectory.TrapezoidProfile} instead.
 */
public class SlewRateLimiter {
    private final double m_rateLimit;
    private double m_prevVal;
    private double m_prevTime;
    private double m_forwardRateLimit;
    private double m_backwardRateLimit;

    /**
     * Creates a new SlewRateLimiter with the given rate limit and initial value.
     *
     * @param rateLimit    The rate-of-change limit, in units per second.
     * @param initialValue The initial value of the input.
     */
    public SlewRateLimiter(double rateLimit, double initialValue) {
        m_rateLimit = rateLimit;
        m_prevVal = initialValue;
        m_prevTime = WPIUtilJNI.now() * 1e-6;
    }

    /**
     * Creates a new SlewRateLimiter with the given rate limit and an initial value of zero.
     *
     * @param rateLimit The rate-of-change limit, in units per second.
     */
    public SlewRateLimiter(double rateLimit) {
        this(rateLimit, 0);
    }

    /**
     * Creates a new SlewRateLimiter with the given forward rate and backward rate.
     *
     * @param forward_Rate
     * @param backwardRateLimit
     */
    public SlewRateLimiter(double forwardRateLimit, double backwardRateLimit) {
        m_forwardRateLimit = forwardRateLimit;
        m_backwardRateLimit = backwardRateLimit;
        m_prevTime = WPIUtilJNI.now() * 1e-6;
    }

}

    /**
     * Filters the input to limit its slew rate.
     *
     * @param input The input value whose slew rate is to be limited.
     * @return The filtered value, which will not change faster than the slew rate.
     */
    public double calculate(double input) {
        double currentTime = WPIUtilJNI.now() * 1e-6;
        double elapsedTime = currentTime - m_prevTime;
        m_prevVal +=
                MathUtil.clamp(input - m_prevVal, -m_rateLimit * elapsedTime, m_rateLimit * elapsedTime);
        m_prevTime = currentTime;
        return m_prevVal;
    }

    /**
     * Filters the input to limit the slew rate for backward and forward.
     *
     * @param input The input value whose slew rate is to be limited.
     */
    public double calculate(double input) {
        double currentTime = WPIUtilJNI.now() * 1e-6;
        double elapsedTime = currentTime - m_prevTime;
        m_prevVal +=
                MathUtil.clamp(input - m_prevVal, -m_backwardRateLimit * elapsedTime, m_forwardRateLimit * elapsedTime);
        m_prevTime = currentTime;
        return m_prevVal;
    }

    /**
     * Resets the slew rate limiter to the specified value; ignores the rate limit when doing so.
     *
     * @param value The value to reset to.
     */
    public void reset(double value) {
        m_prevVal = value;
        m_prevTime = WPIUtilJNI.now() * 1e-6;
    }
    /**
     * Gets the maximum rate of change.
     */
    public double getForwardRateLimit(){
        return m_forwardRateLimit;
    }
    /**
     * Gets the minimum rate of change.
     */
    public double getBackwardRateLimit(){
        return m_backwardRateLimit;
    }
    /**
     * Sets the maximum rate of change.
     */
    public void setForwardRateLimit(double forwardVal){
        m_forwardRateLimit = forwardVal;
    }
    /**
     * Sets the minimum rate of change.
     */
    public void setBackwardRatelimit(double backwardVal){
        m_backwardRateLimit = backwardVal;
    }

