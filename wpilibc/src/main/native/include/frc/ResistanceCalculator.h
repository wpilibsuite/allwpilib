// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <optional>
#include <utility>

#include <units/current.h>
#include <units/impedance.h>
#include <units/voltage.h>
#include <wpi/circular_buffer.h>

namespace frc {

/**
 * Finds the resistance of a channel or the entire robot using a running linear
 * regression over a window.
 *
 * Must be updated with current and voltage periodically using the Calculate()
 * method.
 *
 * To use this for finding the resistance of a channel, use the calculate method
 * with the battery voltage minus the voltage at the motor controller or
 * whatever is plugged in to the PDP at that channel.
 */
class ResistanceCalculator {
 public:
  /// Default buffer size.
  static constexpr int kDefaultBufferSize = 250;

  /// Default R² threshold.
  static constexpr double kDefaultRSquaredThreshold = 0.75;

  /**
   * Creates a ResistanceCalculator with a default buffer size of 250 and R²
   * threshold of 0.5.
   */
  ResistanceCalculator() = default;

  /**
   * Creates a ResistanceCalculator.
   *
   * @param bufferSize The maximum number of points to take the linear
   *     regression over.
   * @param rSquaredThreshold The minimum R² value (0 to 1) considered
   *     significant enough to return the regression slope instead of NaN. A
   *     lower threshold allows resistance to be returned even with noisier
   *     data.
   */
  ResistanceCalculator(int bufferSize, double rSquaredThreshold);

  ResistanceCalculator(ResistanceCalculator&&) = default;
  ResistanceCalculator& operator=(ResistanceCalculator&&) = default;

  /**
   * Update the buffers with new (current, voltage) points, and remove old
   * points if necessary.
   *
   * @param current The current current
   * @param voltage The current voltage
   * @return The current resistance in ohms, or nothing if there wasn't enough
   *   data.
   */
  std::optional<units::ohm_t> Calculate(units::ampere_t current,
                                        units::volt_t voltage);

 private:
  /**
   * Operator to apply to OnlineCovariance<>::Calculate() inputs.
   */
  enum class Operator {
    /// Add point.
    kAdd,
    /// Remove point.
    kRemove
  };

  template <typename X, typename Y>
  class OnlineCovariance {
   public:
    /**
     * The previously calculated covariance.
     */
    decltype(auto) GetCovariance() const { return m_cov / (m_n - 1); }

    /**
     * Calculate the covariance based on a new point that may be removed or
     * added.
     *
     * @param x The x value of the point.
     * @param y The y value of the point.
     * @param op Operator to apply with the point.
     * @return The new sample covariance.
     */
    decltype(auto) Calculate(X x, Y y, Operator op) {
      // From
      // https://en.wikipedia.org/wiki/Algorithms_for_calculating_variance#Covariance

      X dx = x - m_xMean;
      Y dy = y - m_yMean;

      if (op == Operator::kAdd) {
        ++m_n;
        m_xMean += dx / m_n;
        m_yMean += dy / m_n;

        // This is supposed to be (y - yMean) and not dy
        m_cov += dx * (y - m_yMean);
      } else if (op == Operator::kRemove) {
        --m_n;
        m_xMean -= dx / m_n;
        m_yMean -= dy / m_n;

        // This is supposed to be (y - yMean) and not dy
        m_cov -= dx * (y - m_yMean);
      }

      // Correction for sample variance
      return m_cov / (m_n - 1);
    }

   private:
    /// Number of points covariance is calculated over.
    int m_n = 0;

    /// Current mean of x values.
    X m_xMean{0.0};

    /// Current mean of y values.
    Y m_yMean{0.0};

    /// Current approximated population covariance.
    decltype(std::declval<X>() * std::declval<Y>()) m_cov{0.0};
  };

  /// Buffers holding the current values that will eventually need to be
  /// subtracted from the sum when they leave the window.
  wpi::circular_buffer<units::ampere_t> m_currentBuffer{kDefaultBufferSize};

  /// Buffer holding the voltage values that will eventually need to be
  /// subtracted from the sum when they leave the window.
  wpi::circular_buffer<units::volt_t> m_voltageBuffer{kDefaultBufferSize};

  /// The maximum number of points to take the linear regression over.
  int m_bufferSize = kDefaultBufferSize;

  /// The minimum R² value considered significant enough to return the
  /// regression slope instead of NaN.
  double m_rSquaredThreshold = kDefaultRSquaredThreshold;

  /// Used for approximating current variance.
  OnlineCovariance<units::ampere_t, units::ampere_t> m_currentVariance;

  /// Used for approximating voltage variance.
  OnlineCovariance<units::volt_t, units::volt_t> m_voltageVariance;

  /// Used for approximating covariance of current and voltage.
  OnlineCovariance<units::ampere_t, units::volt_t> m_covariance;
};

}  // namespace frc
