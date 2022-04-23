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

  /**
   * Buffers holding the current values that will eventually need to be subtracted from the sum when
   * they leave the window.
   */
  private final CircularBuffer m_currentBuffer;

  /**
   * Buffer holding the voltage values that will eventually need to be subtracted from the sum when
   * they leave the window.
   */
  private final CircularBuffer m_voltageBuffer;

  /** The maximum number of points to take the linear regression over. */
  private final int m_bufferSize;

  /**
   * The minimum R^2 value considered significant enough to return the regression slope instead of
   * NaN.
   */
  private final double m_rSquaredThreshold;

  /** Running sum of the past currents. */
  private double m_currentSum;

  /** Running sum of the past voltages. */
  private double m_voltageSum;

  /** Running sum of the squares of the past currents. */
  private double m_currentSquaredSum;

  /** Running sum of the squares of the past voltages. */
  private double m_voltageSquaredSum;

  /** Running sum of the past current*voltage's. */
  private double m_prodSum;

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
    m_currentBuffer = new CircularBuffer(bufferSize);
    m_voltageBuffer = new CircularBuffer(bufferSize);
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
    m_currentBuffer = new CircularBuffer(kDefaultBufferSize);
    m_voltageBuffer = new CircularBuffer(kDefaultBufferSize);
    this.m_rSquaredThreshold = kDefaultRSquaredThreshold;
    this.m_bufferSize = kDefaultBufferSize;
  }

  /**
   * Update the buffers with new (current, voltage) points, and remove old points if necessary.
   *
   * @param current The current current
   * @param voltage The current voltage
   * @return The current resistance, in ohms
   */
  @SuppressWarnings("LocalVariableName")
  public double calculate(double current, double voltage) {
    // Update buffers only if drawing current
    if (current != 0) {
      if (m_numPoints >= m_bufferSize) {
        // Pop the last point and remove it from the sums
        double backCurrent = m_currentBuffer.removeLast();
        double backVoltage = m_voltageBuffer.removeLast();
        m_currentSum -= backCurrent;
        m_voltageSum -= backVoltage;
        m_currentSquaredSum -= backCurrent * backCurrent;
        m_voltageSquaredSum -= backVoltage * backVoltage;
        m_prodSum -= backCurrent * backVoltage;
      } else {
        m_numPoints++;
      }
      m_currentBuffer.addFirst(current);
      m_voltageBuffer.addFirst(voltage);
      m_currentSum += current;
      m_voltageSum += voltage;
      m_currentSquaredSum += current * current;
      m_voltageSquaredSum += voltage * voltage;
      m_prodSum += current * voltage;
    }

    // Recalculate resistance
    if (m_numPoints < 2) {
      return Double.NaN;
    }

    double currentVariance =
        (m_currentSquaredSum / m_numPoints) - Math.pow(m_currentSum / m_numPoints, 2);
    double voltageVariance =
        (m_voltageSquaredSum / m_numPoints) - Math.pow(m_voltageSum / m_numPoints, 2);
    double covariance = (m_prodSum - m_currentSum * m_voltageSum / m_numPoints) / (m_numPoints - 1);
    double rSquared = covariance * covariance / (currentVariance * voltageVariance);

    if (rSquared > m_rSquaredThreshold) {
      // Slope of current vs voltage
      double slope = covariance / currentVariance;
      return -slope;
    } else {
      return Double.NaN;
    }
  }
}
