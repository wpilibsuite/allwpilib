// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import edu.wpi.first.util.DoubleCircularBuffer;
import java.util.OptionalDouble;

/**
 * Finds the resistance of a channel using a running linear regression over a window.
 *
 * <p>Must be updated with current and voltage periodically using the {@link
 * ResistanceCalculator#calculate(double, double) calculate} method.
 *
 * <p>To use this for finding the resistance of a channel, use the calculate method with the battery
 * voltage minus the voltage at the motor controller or whatever is plugged in to the PDP at that
 * channel.
 */
public final class ResistanceCalculator {
  /** Default buffer size. */
  public static final int kDefaultBufferSize = 250;

  /** Default R² threshold. */
  public static final double kDefaultRSquaredThreshold = 0.75;

  /** Buffer holding previous current values. */
  private final DoubleCircularBuffer m_currentBuffer;

  /** Buffer holding previous voltage values. */
  private final DoubleCircularBuffer m_voltageBuffer;

  /** The maximum number of points to take the linear regression over. */
  private final int m_bufferSize;

  /**
   * The minimum R² value considered significant enough to return the regression slope instead of
   * NaN.
   */
  private final double m_rSquaredThreshold;

  /** Used for approximating current variance. */
  private final OnlineCovariance m_currentVariance;

  /** Used for approximating voltage variance. */
  private final OnlineCovariance m_voltageVariance;

  /** Used for approximating covariance of current and voltage. */
  private final OnlineCovariance m_covariance;

  /** Creates a ResistanceCalculator with a default buffer size of 250 and R² threshold of 0.5. */
  public ResistanceCalculator() {
    this(kDefaultBufferSize, kDefaultRSquaredThreshold);
  }

  /**
   * Creates a {@code ResistanceCalculator}.
   *
   * @param bufferSize The maximum number of points to take the linear regression over.
   * @param rSquaredThreshold The minimum R² value considered significant enough to return the
   *     regression slope instead of NaN.
   */
  @SuppressWarnings("ParameterName")
  public ResistanceCalculator(int bufferSize, double rSquaredThreshold) {
    this.m_rSquaredThreshold = rSquaredThreshold;
    this.m_bufferSize = bufferSize;
    this.m_currentBuffer = new DoubleCircularBuffer(bufferSize);
    this.m_voltageBuffer = new DoubleCircularBuffer(bufferSize);
    this.m_currentVariance = new OnlineCovariance();
    this.m_voltageVariance = new OnlineCovariance();
    this.m_covariance = new OnlineCovariance();
  }

  /**
   * Recalculates resistance given a new current and voltage. The linear regression is only updated
   * if current is nonzero.
   *
   * @param current The current current, in amperes.
   * @param voltage The current voltage, in volts.
   * @return The current resistance, in ohms, or nothing if there wasn't enough data.
   */
  @SuppressWarnings("LocalVariableName")
  public OptionalDouble calculate(double current, double voltage) {
    if (current != 0) {
      if (m_currentBuffer.size() >= m_bufferSize) {
        double lastCurrent = m_currentBuffer.removeLast();
        double lastVoltage = m_voltageBuffer.removeLast();
        m_currentVariance.calculate(lastCurrent, lastCurrent, Operator.kRemove);
        m_voltageVariance.calculate(lastVoltage, lastVoltage, Operator.kRemove);
        m_covariance.calculate(lastCurrent, lastVoltage, Operator.kRemove);
      }

      m_currentBuffer.addFirst(current);
      m_voltageBuffer.addFirst(voltage);
      m_currentVariance.calculate(current, current, Operator.kAdd);
      m_voltageVariance.calculate(voltage, voltage, Operator.kAdd);
      m_covariance.calculate(current, voltage, Operator.kAdd);
    }

    if (m_currentBuffer.size() < 2) {
      return OptionalDouble.empty();
    }

    // Recalculate resistance
    double currentVariance = m_currentVariance.getCovariance();
    double voltageVariance = m_voltageVariance.getCovariance();
    double covariance = m_covariance.getCovariance();
    double rSquared = covariance * covariance / (currentVariance * voltageVariance);

    if (rSquared > m_rSquaredThreshold) {
      // Resistance is slope of current vs voltage
      return OptionalDouble.of(covariance / currentVariance);
    } else {
      return OptionalDouble.empty();
    }
  }

  /** Operator to apply with OnlineCovariance.Calculate() inputs. */
  public enum Operator {
    /** Add point. */
    kAdd,
    /** Remove point. */
    kRemove
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
     *
     * @param x The x value of the point.
     * @param y The y value of the point.
     * @param op Operator to apply with the point.
     * @return The new sample covariance.
     */
    public double calculate(double x, double y, Operator op) {
      // From https://en.wikipedia.org/wiki/Algorithms_for_calculating_variance#Covariance

      double dx = x - m_xMean;
      double dy = y - m_yMean;

      if (op == Operator.kAdd) {
        ++m_n;
        m_xMean += dx / m_n;
        m_yMean += dy / m_n;

        // This is supposed to be (y - yMean) and not dy
        m_cov += dx * (y - m_yMean);
      } else if (op == Operator.kRemove) {
        --m_n;
        m_xMean -= dx / m_n;
        m_yMean -= dy / m_n;

        // This is supposed to be (y - yMean) and not dy
        m_cov -= dx * (y - m_yMean);
      }

      // Correction for sample variance
      return m_cov / (m_n - 1);
    }
  }
}
