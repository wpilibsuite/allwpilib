// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <units/current.h>
#include <units/impedance.h>
#include <units/power.h>
#include <units/voltage.h>
#include <wpi/circular_buffer.h>

namespace frc {

/**
 * Finds the resistance of a channel or the entire robot using a running linear
 * regression over a window. Must be updated with current and voltage
 * periodically using the <code>Calculate</code> method. <p>To use this for
 * finding the resistance of a channel, use the calculate method with the
 * battery voltage minus the voltage at the motor controller or whatever is
 * plugged in to the PDP at that channel.</p>
 */
class ResistanceCalculator {
 public:
  static constexpr int kDefaultBufferSize = 250;
  static constexpr double kDefaultRSquaredThreshold = 0.75;

  /**
   * Create a ResistanceCalculator to find the resistance of a channel using a
   * running linear regression over a window. Must be updated with current and
   * voltage periodically using the <code>Calculate</code> method.
   *
   * @param bufferSize The maximum number of points to take the linear
   * regression over.
   * @param rSquaredThreshold The minimum R² value (0 to 1) considered
   * significant enough to return the regression slope instead of NaN. A lower
   * threshold allows resistance to be returned even with noisier data.
   */
  ResistanceCalculator(int bufferSize, double rSquaredThreshold);

  /**
   * Create a ResistanceCalculator to find the resistance of a channel using a
   * running linear regression over a window. Must be updated with current and
   * voltage periodically using the <code>Calculate</code> method. <p>Uses a
   * buffer size of 250 and an R² threshold of 0.5.</p>
   */
  ResistanceCalculator();

  ~ResistanceCalculator() = default;
  ResistanceCalculator(ResistanceCalculator&&) = default;
  ResistanceCalculator& operator=(ResistanceCalculator&&) = default;

  /**
   * Update the buffers with new (current, voltage) points, and remove old
   * points if necessary.
   *
   * @param current The current current
   * @param voltage The current voltage
   * @return The current resistance in ohms
   */
  units::ohm_t Calculate(units::ampere_t current, units::volt_t voltage);

 private:
  using ampere_squared_t = units::unit_t<units::squared<units::amperes>>;
  using volt_squared_t = units::unit_t<units::squared<units::volts>>;

  /**
   * Buffers holding the current values that will eventually need to be
   * subtracted from the sum when they leave the window.
   */
  wpi::circular_buffer<units::ampere_t> m_currentBuffer;

  /**
   * Buffer holding the voltage values that will eventually need to be
   * subtracted from the sum when they leave the window.
   */
  wpi::circular_buffer<units::volt_t> m_voltageBuffer;

  /**
   * The maximum number of points to take the linear regression over.
   */
  int m_bufferSize;

  /**
   * The minimum R² value considered significant enough to return the
   * regression slope instead of NaN.
   */
  double m_rSquaredThreshold;

  /**
   * Running sum of the past currents.
   */
  units::ampere_t m_currentSum;

  /**
   * Running sum of the past voltages.
   */
  units::volt_t m_voltageSum;

  /**
   * Running sum of the squares of the past currents.
   */
  ampere_squared_t m_currentSquaredSum;

  /**
   * Running sum of the squares of the past voltages.
   */
  volt_squared_t m_voltageSquaredSum;

  /**
   * Running sum of the past current*voltage's.
   */
  units::watt_t m_prodSum;

  /**
   * The number of points currently in the buffer.
   */
  int m_numPoints;
};

}  // namespace frc
