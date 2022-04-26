// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import edu.wpi.first.util.CircularBuffer;

/**
 * Finds the resistance of a channel using a running linear regression over a window. Must be
 * updated with current and voltage periodically using the {@link
 * ResistanceCalculator#calculate(double, double) calculate} method.
 *
 * <p>To use this for finding the resistance of a channel, use the calculate method with the battery
 * voltage minus the voltage at the motor controller or whatever is plugged in to the PDP at that
 * channel.
 */
public final class ResistanceCalculator {
  public static final int kDefaultBufferSize = 250;
  public static final double kDefaultRSquaredThreshold = 0.75;

  /** Buffer holding previous current values. */
  private final CircularBuffer m_currentBuffer;

  /** Buffer holding previous voltage values. */
  private final CircularBuffer m_voltageBuffer;

  /** The maximum number of points to take the linear regression over. */
  private final int m_bufferSize;

  /**
   * The minimum R^2 value considered significant enough to return the regression slope instead of
   * NaN.
   */
  private final double m_rSquaredThreshold;

  /** Used for approximating current variance. */
  private final OnlineCovariance m_currentVariance;

  /** Used for approximating voltage variance. */
  private final OnlineCovariance m_voltageVariance;

  /** Used for approximating covariance of current and voltage. */
  private final OnlineCovariance m_covariance;

  /**
   * Create a {@code ResistanceCalculator} to find the resistance of a channel using a running
   * linear regression over a window. Must be updated with current and voltage periodically using
   * the {@link ResistanceCalculator#calculate(double, double) calculate} method.
   *
   * @param bufferSize The maximum number of points to take the linear regression over.
   * @param rSquaredThreshold The minimum R^2 value considered significant enough to return the
   *     regression slope instead of NaN.
   */
  @SuppressWarnings("ParameterName")
  public ResistanceCalculator(int bufferSize, double rSquaredThreshold) {
    this.m_rSquaredThreshold = rSquaredThreshold;
    this.m_bufferSize = bufferSize;
    this.m_currentBuffer = new CircularBuffer(bufferSize);
    this.m_voltageBuffer = new CircularBuffer(bufferSize);
    this.m_currentVariance = new OnlineCovariance();
    this.m_voltageVariance = new OnlineCovariance();
    this.m_covariance = new OnlineCovariance();
  }

  /**
   * Create a {@code ResistanceCalculator} to find the resistance of a channel using a running
   * linear regression over a window. Must be updated with current and voltage periodically using
   * the {@link ResistanceCalculator#calculate(double, double) calculate} method.
   *
   * <p>Uses a buffer size of {@link ResistanceCalculator#kDefaultBufferSize} and an r^2 threshold
   * of {@link ResistanceCalculator#kDefaultRSquaredThreshold}.
   */
  public ResistanceCalculator() {
    this(kDefaultBufferSize, kDefaultRSquaredThreshold);
  }

  /**
   * Recalculates resistance given a new current and voltage. The linear regression is only updated
   * if current is nonzero.
   *
   * @param current The current current, in amperes.
   * @param voltage The current voltage, in volts.
   * @return The current resistance, in ohms. NaN if fewer than 2 points have been added.
   */
  @SuppressWarnings("LocalVariableName")
  public double calculate(double current, double voltage) {
    if (current != 0) {
      if (m_currentBuffer.size() >= m_bufferSize) {
        var lastCurrent = m_currentBuffer.removeLast();
        var lastVoltage = m_voltageBuffer.removeLast();
        m_currentVariance.calculate(lastCurrent, lastCurrent, true);
        m_voltageVariance.calculate(lastVoltage, lastVoltage, true);
        m_covariance.calculate(lastCurrent, lastVoltage, true);
      }

      m_currentBuffer.addFirst(current);
      m_voltageBuffer.addFirst(voltage);
      m_currentVariance.calculate(current, current, false);
      m_voltageVariance.calculate(voltage, voltage, false);
      m_covariance.calculate(current, voltage, false);
    }

    if (m_currentBuffer.size() < 2) {
      return Double.NaN;
    }

    // Recalculate resistance
    var currentVariance = m_currentVariance.getCovariance();
    var voltageVariance = m_voltageVariance.getCovariance();
    var covariance = m_covariance.getCovariance();
    var rSquared = covariance * covariance / (currentVariance * voltageVariance);

    if (rSquared > m_rSquaredThreshold) {
      // Resistance is slope of current vs voltage negated
      var slope = covariance / currentVariance;
      return -slope;
    } else {
      return Double.NaN;
    }
  }

  /** A helper that approximates covariance incrementally. */
  private static final class OnlineCovariance {
    /** Number of points covariance is calculated over. */
    private int m_n;

    /** Current mean of x values. */
    private double m_xMean;

    /** Current mean of y values. */
    private double m_yMean;

    /** Current approximated population covariance. */
    private double m_cov;

    /** The previously calculated covariance. */
    public double getCovariance() {
      return m_cov / (m_n - 1);
    }

    /**
     * Calculate the covariance based on a new point that may be removed or added.
     * @param x The x value of the point.
     * @param y The y value of the point.
     * @param remove Whether to remove the point or add it.
     * @return The new sample covariance.
     */
    public double calculate(double x, double y, boolean remove) {
      // From https://en.wikipedia.org/wiki/Algorithms_for_calculating_variance#Covariance
      var factor = remove ? -1 : 1;
      m_n += factor;

      var dx = x - m_xMean;
      var dy = y - m_yMean;

      m_xMean += factor * dx / m_n;
      m_yMean += factor * dy / m_n;

      // This is supposed to be (y - yMean) and not dy
      m_cov += factor * dx * (y - m_yMean);

      // Correction for sample variance
      return m_cov / (m_n - 1);
    }
  }
}
