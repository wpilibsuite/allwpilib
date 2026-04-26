// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.filter;

import org.wpilib.math.util.MathSharedStore;

/**
 * This class implements a cascade of second-order IIR filter sections (biquads) in Direct Form II
 * Transposed. It is intended for running higher-order filters (Butterworth, Chebyshev, etc.)
 * produced by a filter designer without the numerical instability that direct-form implementations
 * of a single high-order polynomial exhibit.
 *
 * <p>Each section implements:
 *
 * <pre>
 *   y[n]   = b₀ x[n] + s₁[n-1]
 *   s₁[n]  = b₁ x[n] - a₁ y[n] + s₂[n-1]
 *   s₂[n]  = b₂ x[n] - a₂ y[n]
 * </pre>
 *
 * <p>Sections are normalized so that a₀ = 1 and are applied in series.
 *
 * <p>For 1st-order IIR filters or simple FIR filters (moving averages, finite differences), prefer
 * LinearFilter and its factory methods — they cover those cases more ergonomically. Use
 * BiquadFilter for high-order IIR cascades.
 *
 * <p>Note: calculate() should be called by the user on a known, regular period. Like any digital
 * filter, the coefficients are a function of the sample rate they were designed for.
 */
public class BiquadFilter {
  /** A single biquad (second-order) section. a₀ is assumed normalized to 1. */
  public static final class Section {
    /** The b0 feedforward coefficient. */
    public final double b0;

    /** The b1 feedforward coefficient. */
    public final double b1;

    /** The b2 feedforward coefficient. */
    public final double b2;

    /** The a1 feedback coefficient. */
    public final double a1;

    /** The a2 feedback coefficient. */
    public final double a2;

    /**
     * Creates a biquad section.
     *
     * @param b0 The b0 feedforward coefficient.
     * @param b1 The b1 feedforward coefficient.
     * @param b2 The b2 feedforward coefficient.
     * @param a1 The a1 feedback coefficient.
     * @param a2 The a2 feedback coefficient.
     */
    public Section(double b0, double b1, double b2, double a1, double a2) {
      this.b0 = b0;
      this.b1 = b1;
      this.b2 = b2;
      this.a1 = a1;
      this.a2 = a2;
    }
  }

  private final Section[] m_sections;
  private final double[][] m_state;
  private double m_lastOutput;

  private static int instances;

  /**
   * Creates a biquad filter cascade from the given sections.
   *
   * @param sections The biquad sections, applied in series.
   * @throws IllegalArgumentException if sections is empty.
   */
  public BiquadFilter(Section... sections) {
    if (sections.length == 0) {
      throw new IllegalArgumentException("BiquadFilter requires at least one section.");
    }
    m_sections = sections.clone();
    m_state = new double[sections.length][2];

    instances++;
    MathSharedStore.reportUsage("BiquadFilter", String.valueOf(instances));
  }

  /**
   * Calculates the next value of the filter.
   *
   * @param input Current input value.
   * @return The filtered value at this step.
   */
  public double calculate(double input) {
    // Direct Form II Transposed biquad. Per section, with state z = (s₁, s₂):
    //   y[n]  = b₀·x[n] + s₁[n-1]
    //   s₁[n] = b₁·x[n] - a₁·y[n] + s₂[n-1]
    //   s₂[n] = b₂·x[n] - a₂·y[n]
    // Reference: https://ccrma.stanford.edu/~jos/fp/Transposed_Direct_Forms.html
    double x = input;
    for (int i = 0; i < m_sections.length; i++) {
      final Section s = m_sections[i];
      final double[] z = m_state[i];

      double y = s.b0 * x + z[0];
      z[0] = s.b1 * x - s.a1 * y + z[1];
      z[1] = s.b2 * x - s.a2 * y;

      x = y;
    }
    m_lastOutput = x;
    return x;
  }

  /** Resets the filter state to zero. */
  public void reset() {
    for (double[] z : m_state) {
      z[0] = 0.0;
      z[1] = 0.0;
    }
    m_lastOutput = 0.0;
  }

  /**
   * Resets the filter state so that subsequent calls to calculate() with a constant input equal to
   * {@code value} immediately return the filter's steady-state response to that input.
   *
   * @param value The constant input value to seed with.
   */
  public void reset(double value) {
    // Steady-state seed: at constant input x, y[n] = y[n-1] = y, s₁[n] = s₁[n-1],
    // and s₂[n] = s₂[n-1]. Substituting into the DF-II Transposed update equations
    // gives the linear system:
    //   y  = b₀·x + s₁
    //   s₁ = b₁·x - a₁·y + s₂
    //   s₂ = b₂·x - a₂·y
    // Adding the s₁ and s₂ rows eliminates s₂:
    //   s₁ = (b₁ + b₂)·x - (a₁ + a₂)·y
    // Substituting into the y row yields y = H(1)·x, where
    //   H(1) = (b₀ + b₁ + b₂) / (1 + a₁ + a₂)
    // is the section's DC gain (the transfer function evaluated at z = 1). s₂ then
    // falls out of its row directly. For cascades, each section's steady-state y
    // is fed as the next section's x.
    // Reference: https://ccrma.stanford.edu/~jos/fp/Transposed_Direct_Forms.html
    double x = value;
    for (int i = 0; i < m_sections.length; i++) {
      final Section s = m_sections[i];
      double sumB = s.b0 + s.b1 + s.b2;
      double sumA = s.a1 + s.a2;
      double y = sumB * x / (1.0 + sumA);

      m_state[i][0] = (s.b1 + s.b2) * x - sumA * y;
      m_state[i][1] = s.b2 * x - s.a2 * y;

      x = y;
    }
    m_lastOutput = x;
  }

  /**
   * Returns the last value calculated by the BiquadFilter.
   *
   * @return The last value.
   */
  public double lastValue() {
    return m_lastOutput;
  }

  /**
   * Returns the number of sections in the cascade.
   *
   * @return The number of sections.
   */
  public int numSections() {
    return m_sections.length;
  }
}
