// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <array>
#include <initializer_list>
#include <span>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>

#include "wpi/math/util/MathShared.hpp"
#include "wpi/units/frequency.hpp"
#include "wpi/util/SymbolExports.hpp"

namespace wpi::math {

/**
 * This class implements a cascade of second-order IIR filter sections (biquads)
 * in Direct Form II Transposed. It is intended for running higher-order filters
 * (Butterworth, Chebyshev, etc.) produced by a filter designer without the
 * numerical instability that direct-form implementations of a single high-order
 * polynomial exhibit.
 *
 * Each section implements:<br>
 *   y[n]   = b₀ x[n] + s₁[n-1]<br>
 *   s₁[n]  = b₁ x[n] - a₁ y[n] + s₂[n-1]<br>
 *   s₂[n]  = b₂ x[n] - a₂ y[n]
 *
 * Sections are normalized so that a₀ = 1 and are applied in series.
 *
 * For 1st-order IIR filters or simple FIR filters (moving averages, finite
 * differences), prefer LinearFilter and its factory methods — they cover those
 * cases more ergonomically. Use BiquadFilter for high-order IIR cascades.
 *
 * Note: Calculate() should be called by the user on a known, regular period.
 * Like any digital filter, the coefficients are a function of the sample rate
 * they were designed for.
 */
class WPILIB_DLLEXPORT BiquadFilter {
 public:
  /**
   * A single biquad (second-order) section. a₀ is assumed normalized to 1.
   */
  struct Section {
    double b0;
    double b1;
    double b2;
    double a1;
    double a2;
  };

  /**
   * Frequency response shape for the classical IIR design factories.
   * For BandPass/BandStop, two cutoff frequencies (f1, f2) are required.
   */
  enum class Kind { LowPass, HighPass, BandPass, BandStop };

  /**
   * Creates a biquad filter cascade from the given sections.
   *
   * @param sections The biquad sections, applied in series.
   * @throws std::runtime_error if sections is empty.
   */
  constexpr explicit BiquadFilter(std::span<const Section> sections)
      : m_sections(sections.begin(), sections.end()),
        m_state(sections.size(), {0.0, 0.0}) {
    if (sections.empty()) {
      throw std::runtime_error("BiquadFilter requires at least one section.");
    }

    if (!std::is_constant_evaluated()) {
      ++instances;
      wpi::math::MathSharedStore::ReportUsage("BiquadFilter",
                                              std::to_string(instances));
    }
  }

  /**
   * Creates a biquad filter cascade from the given sections.
   *
   * @param sections The biquad sections, applied in series.
   * @throws std::runtime_error if sections is empty.
   */
  constexpr BiquadFilter(std::initializer_list<Section> sections)
      : BiquadFilter(
            std::span<const Section>{sections.begin(), sections.end()}) {}

  /**
   * Calculates the next value of the filter.
   *
   * @param input Current input value.
   * @return The filtered value at this step.
   */
  constexpr double Calculate(double input) {
    // Direct Form II Transposed biquad. Per section, with state (s₁, s₂):
    //
    //   y[n]  = b₀·x[n] + s₁[n-1]
    //   s₁[n] = b₁·x[n] - a₁·y[n] + s₂[n-1]
    //   s₂[n] = b₂·x[n] - a₂·y[n]
    //
    // Reference:
    // https://ccrma.stanford.edu/~jos/fp/Transposed_Direct_Forms.html
    double x = input;
    for (size_t i = 0; i < m_sections.size(); ++i) {
      const auto& sec = m_sections[i];
      auto& s = m_state[i];

      double y = sec.b0 * x + s[0];
      s[0] = sec.b1 * x - sec.a1 * y + s[1];
      s[1] = sec.b2 * x - sec.a2 * y;

      x = y;
    }
    m_lastOutput = x;
    return x;
  }

  /**
   * Resets the filter state to zero.
   */
  constexpr void Reset() {
    for (auto& s : m_state) {
      s = {0.0, 0.0};
    }
    m_lastOutput = 0.0;
  }

  /**
   * Resets the filter state so that subsequent calls to Calculate() with a
   * constant input equal to {@code value} immediately return the filter's
   * steady-state response to that input.
   *
   * @param value The constant input value to seed with.
   */
  constexpr void Reset(double value) {
    // Steady-state seed: at constant input x, y[n] = y[n-1] = y, s₁[n] =
    // s₁[n-1], and s₂[n] = s₂[n-1]. Substituting into the DF-II Transposed
    // update equations gives the linear system:
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
    // is the section's DC gain (the transfer function evaluated at z = 1). s₂
    // then falls out of its row directly. For cascades, each section's
    // steady-state y is fed as the next section's x.
    //
    // Reference:
    // https://ccrma.stanford.edu/~jos/fp/Transposed_Direct_Forms.html
    double x = value;
    for (size_t i = 0; i < m_sections.size(); ++i) {
      const auto& sec = m_sections[i];
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
  constexpr double LastValue() const { return m_lastOutput; }

  /**
   * Returns the number of sections in the cascade.
   *
   * @return The number of sections.
   */
  constexpr size_t NumSections() const { return m_sections.size(); }

  /**
   * Returns a view over the cascade's sections, in application order.
   * Useful for inspection, logging, or serialization of designed filters.
   *
   * @return Span over the section list. Valid for the filter's lifetime.
   */
  std::span<const Section> Sections() const { return m_sections; }

  /**
   * Designs a Butterworth IIR low-pass or high-pass filter (single cutoff).
   *
   * Coefficients match @c scipy.signal.butter(order, Wn, btype, fs,
   * output='sos') to within ~1e-10.
   *
   * @param kind        Must be LowPass or HighPass.
   * @param order       Prototype order (>= 1).
   * @param sampleRate  Sample rate. Must be positive.
   * @param cutoff      Cutoff frequency. Must satisfy
   *                    0 < cutoff < sampleRate/2.
   * @throws std::invalid_argument if any argument is out of range or @a kind
   *         is BandPass / BandStop.
   */
  static BiquadFilter Butterworth(Kind kind, int order,
                                  wpi::units::hertz_t sampleRate,
                                  wpi::units::hertz_t cutoff);

  /**
   * Designs a Butterworth IIR band-pass or band-stop filter as a cascade of
   * biquad sections.
   *
   * BandPass/BandStop outputs are numerically equivalent to scipy but may
   * differ in section ordering / zero pairing; the product response matches.
   *
   * @param kind        Must be BandPass or BandStop.
   * @param order       Prototype order (>= 1). The resulting cascade has
   *                    2*order poles.
   * @param sampleRate  Sample rate. Must be positive.
   * @param lowCutoff   Low edge of the band. Must satisfy
   *                    0 < lowCutoff < highCutoff < sampleRate/2.
   * @param highCutoff  High edge of the band.
   * @throws std::invalid_argument if any argument is out of range or @a kind
   *         is LowPass / HighPass.
   */
  static BiquadFilter Butterworth(Kind kind, int order,
                                  wpi::units::hertz_t sampleRate,
                                  wpi::units::hertz_t lowCutoff,
                                  wpi::units::hertz_t highCutoff);

  /**
   * Designs a Chebyshev type-I IIR filter as a cascade of biquad sections.
   * Equiripple in the passband, monotonic in the stopband. Coefficients match
   * @c scipy.signal.cheby1(order, rp, Wn, btype, fs, output='sos').
   *
   * @param kind        Must be BandPass or BandStop.
   * @param order       Prototype order (>= 1). The cascade has 2*order poles.
   * @param sampleRate  Sample rate. Must be positive.
   * @param lowCutoff   Low edge of the band. Must satisfy
   *                    0 < lowCutoff < highCutoff < sampleRate/2.
   * @param highCutoff  High edge of the band.
   * @param rippleDb    Peak-to-peak passband ripple in dB. Must be > 0; values
   *                    from ~0.1 to ~3 dB are typical.
   * @throws std::invalid_argument if any argument is out of range or @a kind
   *         is LowPass / HighPass.
   */
  static BiquadFilter ChebyshevI(Kind kind, int order,
                                 wpi::units::hertz_t sampleRate,
                                 wpi::units::hertz_t lowCutoff,
                                 wpi::units::hertz_t highCutoff,
                                 double rippleDb);

  /**
   * Designs a Chebyshev type-I IIR low-pass or high-pass filter (single
   * cutoff). The cutoff is the frequency at which the response first drops
   * to -rippleDb dB.
   *
   * @param kind        Must be LowPass or HighPass.
   * @param order       Prototype order (>= 1).
   * @param sampleRate  Sample rate. Must be positive.
   * @param cutoff      Cutoff frequency. Must satisfy
   *                    0 < cutoff < sampleRate/2.
   * @param rippleDb    Peak-to-peak passband ripple in dB. Must be > 0.
   * @throws std::invalid_argument if any argument is out of range or @a kind
   *         is BandPass / BandStop.
   */
  static BiquadFilter ChebyshevI(Kind kind, int order,
                                 wpi::units::hertz_t sampleRate,
                                 wpi::units::hertz_t cutoff, double rippleDb);

  /**
   * Designs a Chebyshev type-II (inverse Chebyshev) IIR filter as a cascade of
   * biquad sections. Monotonic in the passband, equiripple in the stopband.
   * Coefficients match @c scipy.signal.cheby2(order, rs, Wn, btype, fs,
   * output='sos').
   *
   * @param kind          Must be BandPass or BandStop.
   * @param order         Prototype order (>= 1). The cascade has 2*order poles.
   * @param sampleRate    Sample rate. Must be positive.
   * @param lowCutoff     Low edge of the stop band. Must satisfy
   *                      0 < lowCutoff < highCutoff < sampleRate/2.
   * @param highCutoff    High edge of the stop band.
   * @param stopAttenDb   Stopband attenuation in dB. Must be > 0; values from
   *                      ~20 to ~80 dB are typical.
   * @throws std::invalid_argument if any argument is out of range or @a kind
   *         is LowPass / HighPass.
   */
  static BiquadFilter ChebyshevII(Kind kind, int order,
                                  wpi::units::hertz_t sampleRate,
                                  wpi::units::hertz_t lowCutoff,
                                  wpi::units::hertz_t highCutoff,
                                  double stopAttenDb);

  /**
   * Designs a Chebyshev type-II IIR low-pass or high-pass filter (single
   * cutoff). The cutoff is the frequency at which the response first reaches
   * @a stopAttenDb of attenuation.
   *
   * @param kind          Must be LowPass or HighPass.
   * @param order         Prototype order (>= 1).
   * @param sampleRate    Sample rate. Must be positive.
   * @param cutoff        Stopband-edge frequency. Must satisfy
   *                      0 < cutoff < sampleRate/2.
   * @param stopAttenDb   Stopband attenuation in dB. Must be > 0.
   * @throws std::invalid_argument if any argument is out of range or @a kind
   *         is BandPass / BandStop.
   */
  static BiquadFilter ChebyshevII(Kind kind, int order,
                                  wpi::units::hertz_t sampleRate,
                                  wpi::units::hertz_t cutoff,
                                  double stopAttenDb);

  /**
   * Designs an elliptic (Cauer) IIR filter as a cascade of biquad sections.
   * Equiripple in both passband and stopband — the steepest transition for a
   * given order, at the cost of ripple everywhere. Coefficients match
   * @c scipy.signal.ellip(order, rp, rs, Wn, btype, fs, output='sos').
   *
   * @param kind          Must be BandPass or BandStop.
   * @param order         Filter order (>= 1).
   * @param sampleRate    Sample rate. Must be positive.
   * @param lowCutoff     Low edge of the band. Must satisfy
   *                      0 < lowCutoff < highCutoff < sampleRate/2.
   * @param highCutoff    High edge of the band.
   * @param rippleDb      Passband ripple in dB (> 0).
   * @param stopAttenDb   Stopband attenuation in dB (must exceed @a rippleDb).
   * @throws std::invalid_argument if any argument is out of range or @a kind
   *         is LowPass / HighPass.
   */
  static BiquadFilter Elliptic(Kind kind, int order,
                               wpi::units::hertz_t sampleRate,
                               wpi::units::hertz_t lowCutoff,
                               wpi::units::hertz_t highCutoff, double rippleDb,
                               double stopAttenDb);

  /**
   * Designs an elliptic (Cauer) IIR low-pass or high-pass filter (single
   * cutoff). The cutoff is the frequency at which the response first drops
   * to -rippleDb dB.
   *
   * @param kind          Must be LowPass or HighPass.
   * @param order         Filter order (>= 1).
   * @param sampleRate    Sample rate. Must be positive.
   * @param cutoff        Cutoff frequency. Must satisfy
   *                      0 < cutoff < sampleRate/2.
   * @param rippleDb      Passband ripple in dB (> 0).
   * @param stopAttenDb   Stopband attenuation in dB (must exceed @a rippleDb).
   * @throws std::invalid_argument if any argument is out of range or @a kind
   *         is BandPass / BandStop.
   */
  static BiquadFilter Elliptic(Kind kind, int order,
                               wpi::units::hertz_t sampleRate,
                               wpi::units::hertz_t cutoff, double rippleDb,
                               double stopAttenDb);

  /**
   * Designs a second-order IIR notch at the given center frequency with the
   * given quality factor. Matches @c scipy.signal.iirnotch.
   *
   * @param sampleRate       Sample rate. Must be positive.
   * @param centerFrequency  Notch center frequency. Must satisfy
   *                         0 < centerFrequency < sampleRate/2.
   * @param qualityFactor    Quality factor (Q). Higher values give a narrower
   *                         notch. Must be positive.
   * @throws std::invalid_argument if any argument is out of range.
   */
  static BiquadFilter Notch(wpi::units::hertz_t sampleRate,
                            wpi::units::hertz_t centerFrequency,
                            double qualityFactor);

  /**
   * Designs an N-tap moving-average filter as a cascade of FIR biquads.
   *
   * Each section has a1 = a2 = 0 (all poles at the origin). The total gain
   * 1/taps is folded into the first section's numerator so the DC gain of
   * the cascade is 1.
   *
   * @param taps Length of the moving-average window. Must be >= 1.
   * @throws std::invalid_argument if taps < 1.
   */
  static BiquadFilter MovingAverage(int taps);

 private:
  std::vector<Section> m_sections;
  std::vector<std::array<double, 2>> m_state;
  double m_lastOutput = 0.0;

  inline static int instances = 0;
};

}  // namespace wpi::math
