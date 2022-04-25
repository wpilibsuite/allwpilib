// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import java.util.ArrayDeque;

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

  /** The number of points currently in the buffer. */
  private int m_numPoints;

  /** Running sum of the current. */
  private double m_currentSum;

  /** Running sum of the voltage. */
  private double m_voltageSum;

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
      if (m_numPoints >= m_bufferSize) {
        var lastCurrent = m_currentBuffer.removeLast();
        var lastVoltage = m_voltageBuffer.removeLast();
        m_currentSum -= lastCurrent;
        m_voltageSum -= lastVoltage;
      }

      m_currentBuffer.addFirst(current);
      m_voltageBuffer.addFirst(voltage);
      m_currentSum += current;
      m_voltageSum += voltage;
      
      if (m_numPoints < m_bufferSize) {
        m_numPoints++;
      }
    }

    if (m_numPoints < 2) {
      return Double.NaN;
    }

    // Recalculate resistance
    var currentMean = m_currentSum / m_numPoints;
    var voltageMean = m_voltageSum / m_numPoints;
    var currentVariance = covariance(currentMean, currentMean, m_currentBuffer, m_currentBuffer);
    var voltageVariance = covariance(voltageMean, voltageMean, m_voltageBuffer, m_voltageBuffer);
    double covariance = covariance(currentMean, voltageMean, m_currentBuffer, m_voltageBuffer);
    double rSquared = covariance * covariance / (currentVariance * voltageVariance);

    if (rSquared > m_rSquaredThreshold) {
      // Resistance is slope of current vs voltage negated
      double slope = covariance / currentVariance;
      return -slope;
    } else {
      return Double.NaN;
    }
  }

  /**
   * Calculate covariance of two variables, x and y.
   * @param xMean Mean of x
   * @param yMean Mean of y
   * @param xBuffer x values used for calculating covariance
   * @param yBuffer y values used for calculating covariance
   */
  @SuppressWarnings("ParameterName")
  private double covariance(
      double xMean, double yMean, CircularBuffer xBuffer, CircularBuffer yBuffer) {
    var prodSum = 0.0;

    for (int i = 0; i < m_numPoints; i ++) {
      prodSum += (xBuffer.get(i) - xMean) * (yBuffer.get(i) - yMean);
    }

    return prodSum / m_numPoints;
  }
}
