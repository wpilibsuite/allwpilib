// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <limits>

#include <gcem.hpp>
#include <wpi/SymbolExports.h>
#include <wpi/sendable/Sendable.h>
#include <wpi/sendable/SendableHelper.h>

#include "wpimath/MathShared.h"

namespace frc {

/**
 * Implements a bang-bang controller, which outputs either 0 or 1 depending on
 * whether the measurement is less than the setpoint. This maximally-aggressive
 * control approach works very well for velocity control of high-inertia
 * mechanisms, and poorly on most other things.
 *
 * <p>Note that this is an *asymmetric* bang-bang controller - it will not exert
 * any control effort in the reverse direction (e.g. it won't try to slow down
 * an over-speeding shooter wheel). This asymmetry is *extremely important.*
 * Bang-bang control is extremely simple, but also potentially hazardous. Always
 * ensure that your motor controllers are set to "coast" before attempting to
 * control them with a bang-bang controller.
 */
class WPILIB_DLLEXPORT BangBangController
    : public wpi::Sendable,
      public wpi::SendableHelper<BangBangController> {
 public:
  /**
   * Creates a new bang-bang controller.
   *
   * <p>Always ensure that your motor controllers are set to "coast" before
   * attempting to control them with a bang-bang controller.
   *
   * @param tolerance Tolerance for atSetpoint.
   */
  constexpr explicit BangBangController(
      double tolerance = std::numeric_limits<double>::infinity())
      : m_tolerance(tolerance) {
    if (!std::is_constant_evaluated()) {
      ++instances;
      wpi::math::MathSharedStore::ReportUsage(
          wpi::math::MathUsageId::kController_BangBangController, instances);
    }
  }

  /**
   * Sets the setpoint for the bang-bang controller.
   *
   * @param setpoint The desired setpoint.
   */
  constexpr void SetSetpoint(double setpoint) { m_setpoint = setpoint; }

  /**
   * Returns the current setpoint of the bang-bang controller.
   *
   * @return The current setpoint.
   */
  constexpr double GetSetpoint() const { return m_setpoint; }

  /**
   * Returns true if the error is within the tolerance of the setpoint.
   *
   * @return Whether the error is within the acceptable bounds.
   */
  constexpr bool AtSetpoint() const {
    return gcem::abs(m_setpoint - m_measurement) < m_tolerance;
  }

  /**
   * Sets the error within which AtSetpoint will return true.
   *
   * @param tolerance Position error which is tolerable.
   */
  constexpr void SetTolerance(double tolerance) { m_tolerance = tolerance; }

  /**
   * Returns the current tolerance of the controller.
   *
   * @return The current tolerance.
   */
  constexpr double GetTolerance() const { return m_tolerance; }

  /**
   * Returns the current measurement of the process variable.
   *
   * @return The current measurement of the process variable.
   */
  constexpr double GetMeasurement() const { return m_measurement; }

  /**
   * Returns the current error.
   *
   * @return The current error.
   */
  constexpr double GetError() const { return m_setpoint - m_measurement; }

  /**
   * Returns the calculated control output.
   *
   * <p>Always ensure that your motor controllers are set to "coast" before
   * attempting to control them with a bang-bang controller.
   *
   * @param measurement The most recent measurement of the process variable.
   * @param setpoint The setpoint for the process variable.
   * @return The calculated motor output (0 or 1).
   */
  constexpr double Calculate(double measurement, double setpoint) {
    m_measurement = measurement;
    m_setpoint = setpoint;

    return measurement < setpoint ? 1 : 0;
  }

  /**
   * Returns the calculated control output.
   *
   * @param measurement The most recent measurement of the process variable.
   * @return The calculated motor output (0 or 1).
   */
  constexpr double Calculate(double measurement) {
    return Calculate(measurement, m_setpoint);
  }

  void InitSendable(wpi::SendableBuilder& builder) override;

 private:
  double m_tolerance;

  double m_setpoint = 0;
  double m_measurement = 0;

  // Usage reporting instances
  inline static int instances = 0;
};

}  // namespace frc
