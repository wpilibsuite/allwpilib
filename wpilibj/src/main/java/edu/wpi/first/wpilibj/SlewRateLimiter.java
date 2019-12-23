/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import edu.wpi.first.wpiutil.math.MathUtil;

/**
 * A class that limits the rate of change of an input value.  Useful for implementing voltage,
 * setpoint, and/or output ramps.
 */
public class SlewRateLimiter {
  private final Timer m_timer = new Timer();
  private final double m_rateLimit;
  private double m_prevVal;

  /**
   * Creates a new SlewRateLimiter with the given rate limit and initial value.
   *
   * @param rateLimit The rate-of-change limit, in units per second.
   * @param initialValue The initial value of the input.
   */
  public SlewRateLimiter(double rateLimit, double initialValue) {
    m_prevVal = initialValue;
    m_rateLimit = rateLimit;
    m_timer.start();
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
   * Filters the input to limit its slew rate.
   *
   * @param input The input value whose slew rate is to be limited.
   * @return The filtered value, which will not change faster than the slew rate.
   */
  public double calculate(double input) {
    m_prevVal += MathUtil.clamp(input - m_prevVal,
                                -m_rateLimit * m_timer.get(),
                                m_rateLimit * m_timer.get());
    m_timer.reset();
    return m_prevVal;
  }

  /**
   * Resets the slew rate limiter to the specified value; ignores the rate limit when doing so.
   *
   * @param value The value to reset to.
   */
  public void reset(double value) {
    m_timer.reset();
    m_prevVal = value;
  }
}
