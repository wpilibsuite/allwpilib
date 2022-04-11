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
   * Buffers holding the total current values that will eventually need to be subtracted from the sum when
   * they leave the window.
   */
  private final CircularBuffer currentBuffer;
  /**
   * Buffer holding the voltage values that will eventually need to be subtracted from the sum when
   * they leave the window.
   */
  private final CircularBuffer voltageBuffer;
  /**
   * The maximum number of points to take the linear regression over.
   */
  private final int bufferSize;
  /**
   * The minimum R^2 value considered significant enough to return the regression slope instead of
   * NaN.
   */
  private final double rSquaredThreshhold;
  /**
   * Running sum of the past total currents
   */
  private double currentSum;
  /**
   * Running sum of the past voltages
   */
  private double voltageSum;
  /**
   * Running sum of the squares of the past total currents
   */
  private double currentSquaredSum;
  /**
   * Running sum of the squares of the past voltages
   */
  private double voltageSquaredSum;
  /**
   * Running sum of the past current*voltage's
   */
  private double prodSum;
  /**
   * The number of points currently in the buffer
   */
  private int numPoints;

  /**
   * Default constructor.
   *
   * @param bufferSize         The maximum number of points to take the linear regression over.
   * @param rSquaredThreshhold The minimum R^2 value considered significant enough to return the
   *                           regression slope instead of NaN. Defaults to 0.
   */
  public ResistanceCalculator(int bufferSize, double rSquaredThreshhold) {
    currentBuffer = new CircularBuffer(bufferSize);
    voltageBuffer = new CircularBuffer(bufferSize);
    this.rSquaredThreshhold = rSquaredThreshhold;
    numPoints = 0;
    currentSum = 0;
    voltageSum = 0;
    this.bufferSize = bufferSize;
  }

  /**
   * The current resistance, equal to the slope (negated) of the linear regression line.
   */
  public Double getResistance() {
    if (numPoints < 2) {
      return Double.NaN;
    }

    double currentVariance = (currentSquaredSum / numPoints) - Math.pow(currentSum / numPoints, 2);
    double voltageVariance = (voltageSquaredSum / numPoints) - Math.pow(voltageSum / numPoints, 2);
    double covariance = (prodSum - currentSum * voltageSum / numPoints) / (numPoints - 1);
    double rSquared = covariance * covariance / (currentVariance * voltageVariance);

    if (rSquared > rSquaredThreshhold) {
      // Slope of total current vs voltage
      double slope = covariance / currentVariance;
      return -slope;
    } else {
      return Double.NaN;
    }
  }

  /**
   * Update the buffers with new (totalCurrent, voltage) points, and remove old points if necessary
   *
   * @param totalCurrent The current total current
   * @param voltage      The current voltage
   */
  public void update(double totalCurrent, double voltage) {
    if (numPoints >= bufferSize) {
      // Pop the last point and remove it from the sums
      double backCurrent = currentBuffer.removeLast();
      double backVoltage = voltageBuffer.removeLast();
      currentSum -= backCurrent;
      voltageSum -= backVoltage;
      currentSquaredSum -= backCurrent * backCurrent;
      voltageSquaredSum -= backVoltage * backVoltage;
      prodSum -= backCurrent * backVoltage;
    } else {
      numPoints++;
    }
    currentBuffer.addFirst(totalCurrent);
    voltageBuffer.addFirst(voltage);
    currentSum += totalCurrent;
    voltageSum += voltage;
    currentSquaredSum += totalCurrent * totalCurrent;
    voltageSquaredSum += voltage * voltage;
    prodSum += totalCurrent * voltage;
  }
}
