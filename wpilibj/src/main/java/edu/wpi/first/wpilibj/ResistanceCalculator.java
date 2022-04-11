// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import edu.wpi.first.util.CircularBuffer;

/**
 * Finds the resistance using a running linear regression over a window.
 * Must be updated with total current and voltage periodically using the
 * {@link ResistanceCalculator#update(double, double) update} method.
 * Resistance is voltage sag / total current.
 */
public class ResistanceCalculator {
  /**
   * Buffers holding the total current values that will eventually need to be
   * subtracted from the sum when they leave the window.
   */
  private final CircularBuffer m_currentBuffer;
  /**
   * Buffer holding the voltage values that will eventually need to be subtracted from the sum when
   * they leave the window.
   */
  private final CircularBuffer m_voltageBuffer;
  /**
   * The maximum number of points to take the linear regression over.
   */
  private final int m_bufferSize;
  /**
   * The minimum R^2 value considered significant enough to return the regression slope instead of
   * NaN.
   */
  private final double m_rSquaredThreshhold;
  /**
   * Running sum of the past total currents.
   */
  private double m_currentSum = 0;
  /**
   * Running sum of the past voltages.
   */
  private double m_voltageSum = 0;
  /**
   * Running sum of the squares of the past total currents.
   */
  private double m_currentSquaredSum = 0;
  /**
   * Running sum of the squares of the past voltages.
   */
  private double m_voltageSquaredSum = 0;
  /**
   * Running sum of the past current*voltage's.
   */
  private double m_prodSum = 0;
  /**
   * The number of points currently in the buffer.
   */
  private int m_numPoints = 0;

  /**
   * Default constructor.
   *
   * @param bufferSize          The maximum number of points to take the linear regression over.
   * @param krSquaredThreshhold The minimum R^2 value considered significant enough to return the
   *                           regression slope instead of NaN. Defaults to 0.
   */
  public ResistanceCalculator(int bufferSize, double krSquaredThreshhold) {
    m_currentBuffer = new CircularBuffer(bufferSize);
    m_voltageBuffer = new CircularBuffer(bufferSize);
    this.m_rSquaredThreshhold = krSquaredThreshhold;
    m_numPoints = 0;
    m_currentSum = 0;
    m_voltageSum = 0;
    this.m_bufferSize = bufferSize;
  }

  /**
   * @return The current resistance, equal to the slope (negated) of the linear regression line.
   */
  public Double getResistance() {
    if (m_numPoints < 2) {
      return Double.NaN;
    }

    double currentVariance =
        (m_currentSquaredSum / m_numPoints) - Math.pow(m_currentSum / m_numPoints, 2);
    double voltageVariance =
        (m_voltageSquaredSum / m_numPoints) - Math.pow(m_voltageSum / m_numPoints, 2);
    double covariance =
        (m_prodSum - m_currentSum * m_voltageSum / m_numPoints) / (m_numPoints - 1);
    double krSquared = covariance * covariance / (currentVariance * voltageVariance);

    if (krSquared > m_rSquaredThreshhold) {
      // Slope of total current vs voltage
      double slope = covariance / currentVariance;
      return -slope;
    } else {
      return Double.NaN;
    }
  }

  /**
   * Update the buffers with new (totalCurrent, voltage) points, and remove old points if necessary.
   *
   * @param totalCurrent The current total current
   * @param voltage      The current voltage
   */
  public void update(double totalCurrent, double voltage) {
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
    m_currentBuffer.addFirst(totalCurrent);
    m_voltageBuffer.addFirst(voltage);
    m_currentSum += totalCurrent;
    m_voltageSum += voltage;
    m_currentSquaredSum += totalCurrent * totalCurrent;
    m_voltageSquaredSum += voltage * voltage;
    m_prodSum += totalCurrent * voltage;
  }
}
