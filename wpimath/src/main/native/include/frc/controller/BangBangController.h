// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <limits>

#include <wpi/SymbolExports.h>
#include <wpi/sendable/Sendable.h>
#include <wpi/sendable/SendableHelper.h>

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
  explicit BangBangController(
      double tolerance = std::numeric_limits<double>::infinity());

  /**
   * Sets the setpoint for the bang-bang controller.
   *
   * @param setpoint The desired setpoint.
   */
  void SetSetpoint(double setpoint);

  /**
   * Returns the current setpoint of the bang-bang controller.
   *
   * @return The current setpoint.
   */
  double GetSetpoint() const;

  /**
   * Returns true if the error is within the tolerance of the setpoint.
   *
   * @return Whether the error is within the acceptable bounds.
   */
  bool AtSetpoint() const;

  /**
   * Sets the error within which AtSetpoint will return true.
   *
   * @param tolerance Position error which is tolerable.
   */
  void SetTolerance(double tolerance);

  /**
   * Returns the current tolerance of the controller.
   *
   * @return The current tolerance.
   */
  double GetTolerance() const;

  /**
   * Returns the current measurement of the process variable.
   *
   * @return The current measurement of the process variable.
   */
  double GetMeasurement() const;

  /**
   * Returns the current error.
   *
   * @return The current error.
   */
  double GetError() const;

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
  double Calculate(double measurement, double setpoint);

  /**
   * Returns the calculated control output.
   *
   * @param measurement The most recent measurement of the process variable.
   * @return The calculated motor output (0 or 1).
   */
  double Calculate(double measurement);

  void InitSendable(wpi::SendableBuilder& builder) override;

 private:
  double m_tolerance;

  double m_setpoint = 0;
  double m_measurement = 0;
};

}  // namespace frc
