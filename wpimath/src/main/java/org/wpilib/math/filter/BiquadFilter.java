// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.filter;

import org.wpilib.math.filter.internal.BiquadFilterDesigner;
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
 *   y[n]  = b₀ x[n] + s₁[n-1]
 *   s₁[n] = b₁ x[n] - a₁ y[n] + s₂[n-1]
 *   s₂[n] = b₂ x[n] - a₂ y[n]
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

  /**
   * Frequency response shape for the classical IIR design factories. For BandPass/BandStop, two
   * cutoff frequencies (f1, f2) are required.
   */
  public enum Kind {
    /** Low-pass filter (passes frequencies below the cutoff). */
    LowPass,
    /** High-pass filter (passes frequencies above the cutoff). */
    HighPass,
    /** Band-pass filter (passes a frequency band). */
    BandPass,
    /** Band-stop filter (rejects a frequency band). */
    BandStop
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
    // Direct Form II Transposed biquad. Per section, with state (s₁, s₂):
    //
    //   y[n]  = b₀·x[n] + s₁[n-1]
    //   s₁[n] = b₁·x[n] - a₁·y[n] + s₂[n-1]
    //   s₂[n] = b₂·x[n] - a₂·y[n]
    //
    // Reference: https://ccrma.stanford.edu/~jos/fp/Transposed_Direct_Forms.html
    double x = input;
    for (int i = 0; i < m_sections.length; i++) {
      final Section sec = m_sections[i];
      final double[] s = m_state[i];

      double y = sec.b0 * x + s[0];
      s[0] = sec.b1 * x - sec.a1 * y + s[1];
      s[1] = sec.b2 * x - sec.a2 * y;

      x = y;
    }
    m_lastOutput = x;
    return x;
  }

  /** Resets the filter state to zero. */
  public void reset() {
    for (double[] s : m_state) {
      s[0] = 0.0;
      s[1] = 0.0;
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
    //
    //   y  = b₀·x + s₁
    //   s₁ = b₁·x - a₁·y + s₂
    //   s₂ = b₂·x - a₂·y
    //
    // Adding the s₁ and s₂ rows eliminates s₂:
    //
    //   s₁ = (b₁ + b₂)·x - (a₁ + a₂)·y
    //
    // Substituting into the y row yields y = H(1)·x, where
    //
    //   H(1) = (b₀ + b₁ + b₂) / (1 + a₁ + a₂)
    //
    // is the section's DC gain (the transfer function evaluated at z = 1). s₂ then
    // falls out of its row directly. For cascades, each section's steady-state y
    // is fed as the next section's x.
    //
    // Reference: https://ccrma.stanford.edu/~jos/fp/Transposed_Direct_Forms.html
    double x = value;
    for (int i = 0; i < m_sections.length; i++) {
      final Section sec = m_sections[i];
      double sumB = sec.b0 + sec.b1 + sec.b2;
      double sumA = sec.a1 + sec.a2;
      double y = sumB * x / (1.0 + sumA);

      m_state[i][0] = (sec.b1 + sec.b2) * x - sumA * y;
      m_state[i][1] = sec.b2 * x - sec.a2 * y;

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

  /**
   * Returns a copy of the cascade's sections, in application order. Useful for inspection, logging,
   * or serialization of designed filters.
   *
   * @return A copy of the section array.
   */
  public Section[] sections() {
    return m_sections.clone();
  }

  // ---------- Design factories -------------------------------------------------

  /**
   * Designs a Butterworth IIR band-pass or band-stop filter as a cascade of biquad sections.
   *
   * <p>Coefficients match {@code scipy.signal.butter(order, Wn, btype, fs, output='sos')}.
   * BandPass/BandStop outputs are numerically equivalent to scipy but may differ in section
   * ordering / zero pairing; the product response matches.
   *
   * @param kind Must be BandPass or BandStop.
   * @param order Prototype order (&gt;= 1). The cascade has 2*order poles.
   * @param sampleRate Sample rate (Hz). Must be positive.
   * @param lowCutoff Low edge of the band (Hz). Must satisfy 0 &lt; lowCutoff &lt; highCutoff &lt;
   *     sampleRate/2.
   * @param highCutoff High edge of the band (Hz).
   * @return The designed filter.
   * @throws IllegalArgumentException if any argument is out of range or kind is LowPass / HighPass.
   */
  public static BiquadFilter butterworth(
      Kind kind, int order, double sampleRate, double lowCutoff, double highCutoff) {
    return new BiquadFilter(
        BiquadFilterDesigner.butterworth(kind, order, sampleRate, lowCutoff, highCutoff));
  }

  /**
   * Designs a Butterworth IIR low-pass or high-pass filter (single cutoff).
   *
   * <p>Coefficients match {@code scipy.signal.butter(order, Wn, btype, fs, output='sos')} to within
   * ~1e-10.
   *
   * @param kind Must be LowPass or HighPass.
   * @param order Prototype order (&gt;= 1).
   * @param sampleRate Sample rate (Hz). Must be positive.
   * @param cutoff Cutoff frequency (Hz). Must satisfy 0 &lt; cutoff &lt; sampleRate/2.
   * @return The designed filter.
   * @throws IllegalArgumentException if any argument is out of range or kind is BandPass /
   *     BandStop.
   */
  public static BiquadFilter butterworth(Kind kind, int order, double sampleRate, double cutoff) {
    return new BiquadFilter(BiquadFilterDesigner.butterworth(kind, order, sampleRate, cutoff));
  }

  /**
   * Designs a Chebyshev type-I IIR band-pass or band-stop filter as a cascade of biquad sections.
   * Equiripple in the passband, monotonic in the stopband. Coefficients match {@code
   * scipy.signal.cheby1(order, rp, Wn, btype, fs, output='sos')}.
   *
   * @param kind Must be BandPass or BandStop.
   * @param order Prototype order (&gt;= 1). The cascade has 2*order poles.
   * @param sampleRate Sample rate (Hz). Must be positive.
   * @param lowCutoff Low edge of the band (Hz). Must satisfy 0 &lt; lowCutoff &lt; highCutoff &lt;
   *     sampleRate/2.
   * @param highCutoff High edge of the band (Hz).
   * @param rippleDb Peak-to-peak passband ripple in dB. Must be &gt; 0; values from ~0.1 to ~3 dB
   *     are typical.
   * @return The designed filter.
   * @throws IllegalArgumentException if any argument is out of range or kind is LowPass / HighPass.
   */
  public static BiquadFilter chebyshevI(
      Kind kind,
      int order,
      double sampleRate,
      double lowCutoff,
      double highCutoff,
      double rippleDb) {
    return new BiquadFilter(
        BiquadFilterDesigner.chebyshevI(kind, order, sampleRate, lowCutoff, highCutoff, rippleDb));
  }

  /**
   * Designs a Chebyshev type-I IIR low-pass or high-pass filter (single cutoff). The cutoff is the
   * frequency at which the response first drops to -rippleDb dB.
   *
   * @param kind Must be LowPass or HighPass.
   * @param order Prototype order (&gt;= 1).
   * @param sampleRate Sample rate (Hz). Must be positive.
   * @param cutoff Cutoff frequency (Hz). Must satisfy 0 &lt; cutoff &lt; sampleRate/2.
   * @param rippleDb Peak-to-peak passband ripple in dB. Must be &gt; 0.
   * @return The designed filter.
   * @throws IllegalArgumentException if any argument is out of range or kind is BandPass /
   *     BandStop.
   */
  public static BiquadFilter chebyshevI(
      Kind kind, int order, double sampleRate, double cutoff, double rippleDb) {
    return new BiquadFilter(
        BiquadFilterDesigner.chebyshevI(kind, order, sampleRate, cutoff, rippleDb));
  }

  /**
   * Designs a Chebyshev type-II (inverse Chebyshev) IIR band-pass or band-stop filter as a cascade
   * of biquad sections. Monotonic in the passband, equiripple in the stopband. Coefficients match
   * {@code scipy.signal.cheby2(order, rs, Wn, btype, fs, output='sos')}.
   *
   * @param kind Must be BandPass or BandStop.
   * @param order Prototype order (&gt;= 1). The cascade has 2*order poles.
   * @param sampleRate Sample rate (Hz). Must be positive.
   * @param lowCutoff Low edge of the stop band (Hz). Must satisfy 0 &lt; lowCutoff &lt; highCutoff
   *     &lt; sampleRate/2.
   * @param highCutoff High edge of the stop band (Hz).
   * @param stopAttenDb Stopband attenuation in dB. Must be &gt; 0; values from ~20 to ~80 dB are
   *     typical.
   * @return The designed filter.
   * @throws IllegalArgumentException if any argument is out of range or kind is LowPass / HighPass.
   */
  public static BiquadFilter chebyshevII(
      Kind kind,
      int order,
      double sampleRate,
      double lowCutoff,
      double highCutoff,
      double stopAttenDb) {
    return new BiquadFilter(
        BiquadFilterDesigner.chebyshevII(
            kind, order, sampleRate, lowCutoff, highCutoff, stopAttenDb));
  }

  /**
   * Designs a Chebyshev type-II IIR low-pass or high-pass filter (single cutoff). The cutoff is the
   * frequency at which the response first reaches {@code stopAttenDb} of attenuation.
   *
   * @param kind Must be LowPass or HighPass.
   * @param order Prototype order (&gt;= 1).
   * @param sampleRate Sample rate (Hz). Must be positive.
   * @param cutoff Stopband-edge frequency (Hz). Must satisfy 0 &lt; cutoff &lt; sampleRate/2.
   * @param stopAttenDb Stopband attenuation in dB. Must be &gt; 0.
   * @return The designed filter.
   * @throws IllegalArgumentException if any argument is out of range or kind is BandPass /
   *     BandStop.
   */
  public static BiquadFilter chebyshevII(
      Kind kind, int order, double sampleRate, double cutoff, double stopAttenDb) {
    return new BiquadFilter(
        BiquadFilterDesigner.chebyshevII(kind, order, sampleRate, cutoff, stopAttenDb));
  }

  /**
   * Designs an elliptic (Cauer) IIR band-pass or band-stop filter as a cascade of biquad sections.
   * Equiripple in both passband and stopband — the steepest transition for a given order, at the
   * cost of ripple everywhere. Coefficients match {@code scipy.signal.ellip(order, rp, rs, Wn,
   * btype, fs, output='sos')}.
   *
   * @param kind Must be BandPass or BandStop.
   * @param order Filter order (&gt;= 1).
   * @param sampleRate Sample rate (Hz). Must be positive.
   * @param lowCutoff Low edge of the band (Hz). Must satisfy 0 &lt; lowCutoff &lt; highCutoff &lt;
   *     sampleRate/2.
   * @param highCutoff High edge of the band (Hz).
   * @param rippleDb Passband ripple in dB (&gt; 0).
   * @param stopAttenDb Stopband attenuation in dB (must exceed {@code rippleDb}).
   * @return The designed filter.
   * @throws IllegalArgumentException if any argument is out of range or kind is LowPass / HighPass.
   */
  public static BiquadFilter elliptic(
      Kind kind,
      int order,
      double sampleRate,
      double lowCutoff,
      double highCutoff,
      double rippleDb,
      double stopAttenDb) {
    return new BiquadFilter(
        BiquadFilterDesigner.elliptic(
            kind, order, sampleRate, lowCutoff, highCutoff, rippleDb, stopAttenDb));
  }

  /**
   * Designs an elliptic (Cauer) IIR low-pass or high-pass filter (single cutoff). The cutoff is the
   * frequency at which the response first drops to -rippleDb dB.
   *
   * @param kind Must be LowPass or HighPass.
   * @param order Filter order (&gt;= 1).
   * @param sampleRate Sample rate (Hz). Must be positive.
   * @param cutoff Cutoff frequency (Hz). Must satisfy 0 &lt; cutoff &lt; sampleRate/2.
   * @param rippleDb Passband ripple in dB (&gt; 0).
   * @param stopAttenDb Stopband attenuation in dB (must exceed {@code rippleDb}).
   * @return The designed filter.
   * @throws IllegalArgumentException if any argument is out of range or kind is BandPass /
   *     BandStop.
   */
  public static BiquadFilter elliptic(
      Kind kind, int order, double sampleRate, double cutoff, double rippleDb, double stopAttenDb) {
    return new BiquadFilter(
        BiquadFilterDesigner.elliptic(kind, order, sampleRate, cutoff, rippleDb, stopAttenDb));
  }

  /**
   * Designs a second-order IIR notch at the given center frequency with the given quality factor.
   * Matches {@code scipy.signal.iirnotch}.
   *
   * @param sampleRate Sample rate (Hz). Must be positive.
   * @param centerFrequency Notch center frequency (Hz). Must satisfy 0 &lt; centerFrequency &lt;
   *     sampleRate/2.
   * @param qualityFactor Quality factor (Q). Higher values give a narrower notch. Must be positive.
   * @return The designed filter (single-section cascade).
   * @throws IllegalArgumentException if any argument is out of range.
   */
  public static BiquadFilter notch(
      double sampleRate, double centerFrequency, double qualityFactor) {
    return new BiquadFilter(BiquadFilterDesigner.notch(sampleRate, centerFrequency, qualityFactor));
  }

  /**
   * Designs an N-tap moving-average filter as a cascade of FIR biquads.
   *
   * <p>Each section has a1 = a2 = 0 (all poles at the origin). The total gain 1/taps is folded into
   * the first section's numerator so the DC gain of the cascade is 1.
   *
   * @param taps Length of the moving-average window. Must be &gt;= 1.
   * @return The designed filter.
   * @throws IllegalArgumentException if {@code taps} &lt; 1.
   */
  public static BiquadFilter movingAverage(int taps) {
    return new BiquadFilter(BiquadFilterDesigner.movingAverage(taps));
  }
}
