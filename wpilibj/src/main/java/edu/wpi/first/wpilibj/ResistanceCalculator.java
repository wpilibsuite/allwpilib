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
public class ResistanceCalculator {
  public static final int kDefaultBufferSize = 250;
  public static final double kDefaultRSquaredThreshold = 0.75;

  /** Running sum and squared sum of the current. */
  private final RunningSums m_currentSums;

  /** Running sum and squared sum of the voltage. */
  private final RunningSums m_voltageSums;

  /** Running sum and squared sum of the products of current and voltage. */
  private final RunningSums m_prodSums;

  /** The maximum number of points to take the linear regression over. */
  private final int m_bufferSize;

  /**
   * The minimum R^2 value considered significant enough to return the regression slope instead of
   * NaN.
   */
  private final double m_rSquaredThreshold;

  /** The number of points currently in the buffer. */
  private int m_numPoints;

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
    this.m_currentSums = new RunningSums(bufferSize);
	  this.m_voltageSums = new RunningSums(bufferSize);
	  this.m_prodSums	= new RunningSums(bufferSize);
    this.m_rSquaredThreshold = rSquaredThreshold;
    this.m_bufferSize = bufferSize;
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
   * Recalculates resistance given a new current and voltage.
   * The linear regression is only updated if current is nonzero.
   *
   * @param current The current current, in amperes.
   * @param voltage The current voltage, in volts.
   * @return The current resistance, in ohms. NaN if fewer than 2 points have been added.
   */
  @SuppressWarnings("LocalVariableName")
  public double calculate(double current, double voltage) {
    if (current != 0) {
	  m_currentSums.update(current);
	  m_voltageSums.update(voltage);
	  m_prodSums.update(current * voltage);
      if (m_numPoints < m_bufferSize) {
        m_numPoints++;
      }
    }

    if (m_numPoints < 2) {
      return Double.NaN;
    }

    // Recalculate resistance
	  double currentSum = m_currentSums.getSum();
	  double voltageSum = m_voltageSums.getSum();
	  double prodSum = m_prodSums.getSum();
    double currentVariance = m_currentSums.calculateVariance();
    double voltageVariance = m_voltageSums.calculateVariance();

    double covariance = (prodSum - currentSum * voltageSum / m_numPoints) / (m_numPoints - 1);
    double rSquared = covariance * covariance / (currentVariance * voltageVariance);

    if (rSquared > m_rSquaredThreshold) {
      // Resistance is slope of current vs voltage negated
      double slope = covariance / currentVariance;
      return -slope;
    } else {
      return Double.NaN;
    }
  }

  /** A helper class for calculating running sum and variance. */
  private static final class RunningSums {
    /** Buffer holding values whose variance, sum, and squared sum is to be calculated. */
    private final CircularBuffer m_buffer;

    private final double m_bufferSize;

    /** Running sum of the past values. */
    private double m_sum;

    /** Running sum of the squares of the past values. */
    private double m_squaredSum;

    /** Number of values in the buffer. */
    private int m_numValues;

    public RunningSums(int bufferSize) {
      this.m_buffer = new CircularBuffer(bufferSize);
      this.m_bufferSize = bufferSize;
    }

    public double getSum() {
      return m_sum;
    }

    public double calculateVariance() {
      return m_squaredSum / m_numValues - m_sum * m_sum;
    }

    public void update(double value) {
      if (m_numValues >= m_bufferSize) {
        // Pop the last point and remove it from the sums
        double last = m_buffer.removeLast();
        m_sum -= last;
        m_squaredSum -= last * last;
      } else {
        m_numValues++;
      }

      m_buffer.addFirst(value);
      m_sum += value;
      m_squaredSum += value;
    }
  }
}
