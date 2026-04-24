// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <array>
#include <initializer_list>
#include <span>
#include <stdexcept>
#include <string>
#include <vector>

#include "wpi/math/util/MathShared.hpp"

namespace wpi::math {

/**
 * This class implements a cascade of second-order IIR filter sections (biquads)
 * in Direct Form II Transposed. It is intended for running higher-order filters
 * (Butterworth, Chebyshev, etc.) produced by a filter designer without the
 * numerical instability that direct-form implementations of a single high-order
 * polynomial exhibit.
 *
 * Each section implements:<br>
 *   y[n]        = b0 x[n] + s1[n-1]<br>
 *   s1[n]       = b1 x[n] - a1 y[n] + s2[n-1]<br>
 *   s2[n]       = b2 x[n] - a2 y[n]
 *
 * Sections are normalized so that a0 = 1 and are applied in series.
 *
 * Note: Calculate() should be called by the user on a known, regular period.
 * Like any digital filter, the coefficients are a function of the sample rate
 * they were designed for.
 */
class BiquadFilter {
 public:
  /**
   * A single biquad (second-order) section. a0 is assumed normalized to 1.
   */
  struct Section {
    double b0;
    double b1;
    double b2;
    double a1;
    double a2;
  };

  /**
   * Creates a biquad filter cascade from the given sections.
   *
   * @param sections The biquad sections, applied in series.
   * @throws std::runtime_error if sections is empty.
   */
  explicit BiquadFilter(std::span<const Section> sections)
      : m_sections(sections.begin(), sections.end()),
        m_state(sections.size(), {0.0, 0.0}) {
    if (sections.empty()) {
      throw std::runtime_error("BiquadFilter requires at least one section.");
    }

    ++instances;
    wpi::math::MathSharedStore::ReportUsage("BiquadFilter",
                                            std::to_string(instances));
  }

  /**
   * Creates a biquad filter cascade from the given sections.
   *
   * @param sections The biquad sections, applied in series.
   * @throws std::runtime_error if sections is empty.
   */
  BiquadFilter(std::initializer_list<Section> sections)
      : BiquadFilter(
            std::span<const Section>{sections.begin(), sections.end()}) {}

  /**
   * Calculates the next value of the filter.
   *
   * @param input Current input value.
   * @return The filtered value at this step.
   */
  double Calculate(double input) {
    double x = input;
    for (size_t i = 0; i < m_sections.size(); ++i) {
      const auto& s = m_sections[i];
      auto& z = m_state[i];

      double y = s.b0 * x + z[0];
      z[0] = s.b1 * x - s.a1 * y + z[1];
      z[1] = s.b2 * x - s.a2 * y;

      x = y;
    }
    m_lastOutput = x;
    return x;
  }

  /**
   * Resets the filter state to zero.
   */
  void Reset() {
    for (auto& z : m_state) {
      z = {0.0, 0.0};
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
  void Reset(double value) {
    double x = value;
    for (size_t i = 0; i < m_sections.size(); ++i) {
      const auto& s = m_sections[i];
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
  double LastValue() const { return m_lastOutput; }

  /**
   * Returns the number of sections in the cascade.
   *
   * @return The number of sections.
   */
  size_t NumSections() const { return m_sections.size(); }

 private:
  std::vector<Section> m_sections;
  std::vector<std::array<double, 2>> m_state;
  double m_lastOutput = 0.0;

  inline static int instances = 0;
};

}  // namespace wpi::math
