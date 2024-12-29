// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.controller;

import edu.wpi.first.math.MathSharedStore;
import edu.wpi.first.math.MathUsageId;
import edu.wpi.first.util.sendable.Sendable;
import edu.wpi.first.util.sendable.SendableBuilder;
import edu.wpi.first.util.sendable.SendableRegistry;

/**
 * Implements a bang-bang controller, which outputs either 0 or 1 depending on whether the
 * measurement is less than the setpoint. This maximally-aggressive control approach works very well
 * for velocity control of high-inertia mechanisms, and poorly on most other things.
 *
 * <p>Note that this is an *asymmetric* bang-bang controller - it will not exert any control effort
 * in the reverse direction (e.g. it won't try to slow down an over-speeding shooter wheel). This
 * asymmetry is *extremely important.* Bang-bang control is extremely simple, but also potentially
 * hazardous. Always ensure that your motor controllers are set to "coast" before attempting to
 * control them with a bang-bang controller.
 */
public class BangBangController implements Sendable {
  private static int instances;

  private double m_tolerance;

  private double m_setpoint;
  private double m_measurement;

  /**
   * Creates a new bang-bang controller.
   *
   * <p>Always ensure that your motor controllers are set to "coast" before attempting to control
   * them with a bang-bang controller.
   *
   * @param tolerance Tolerance for {@link #atSetpoint() atSetpoint}.
   */
  @SuppressWarnings("this-escape")
  public BangBangController(double tolerance) {
    instances++;

    setTolerance(tolerance);

    SendableRegistry.addLW(this, "BangBangController", instances);

    MathSharedStore.reportUsage(MathUsageId.kController_BangBangController, instances);
  }

  /**
   * Creates a new bang-bang controller.
   *
   * <p>Always ensure that your motor controllers are set to "coast" before attempting to control
   * them with a bang-bang controller.
   */
  public BangBangController() {
    this(Double.POSITIVE_INFINITY);
  }

  /**
   * Sets the setpoint for the bang-bang controller.
   *
   * @param setpoint The desired setpoint.
   */
  public void setSetpoint(double setpoint) {
    m_setpoint = setpoint;
  }

  /**
   * Returns the current setpoint of the bang-bang controller.
   *
   * @return The current setpoint.
   */
  public double getSetpoint() {
    return m_setpoint;
  }

  /**
   * Returns true if the error is within the tolerance of the setpoint.
   *
   * @return Whether the error is within the acceptable bounds.
   */
  public boolean atSetpoint() {
    return Math.abs(m_setpoint - m_measurement) < m_tolerance;
  }

  /**
   * Sets the error within which atSetpoint will return true.
   *
   * @param tolerance Position error which is tolerable.
   */
  public final void setTolerance(double tolerance) {
    m_tolerance = tolerance;
  }

  /**
   * Returns the current tolerance of the controller.
   *
   * @return The current tolerance.
   */
  public double getTolerance() {
    return m_tolerance;
  }

  /**
   * Returns the current measurement of the process variable.
   *
   * @return The current measurement of the process variable.
   */
  public double getMeasurement() {
    return m_measurement;
  }

  /**
   * Returns the current error.
   *
   * @return The current error.
   */
  public double getError() {
    return m_setpoint - m_measurement;
  }

  /**
   * Returns the calculated control output.
   *
   * <p>Always ensure that your motor controllers are set to "coast" before attempting to control
   * them with a bang-bang controller.
   *
   * @param measurement The most recent measurement of the process variable.
   * @param setpoint The setpoint for the process variable.
   * @return The calculated motor output (0 or 1).
   */
  public double calculate(double measurement, double setpoint) {
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
  public double calculate(double measurement) {
    return calculate(measurement, m_setpoint);
  }

  @Override
  public void initSendable(SendableBuilder builder) {
    builder.setSmartDashboardType("BangBangController");
    builder.addDoubleProperty("tolerance", this::getTolerance, this::setTolerance);
    builder.addDoubleProperty("setpoint", this::getSetpoint, this::setSetpoint);
    builder.addDoubleProperty("measurement", this::getMeasurement, null);
    builder.addDoubleProperty("error", this::getError, null);
    builder.addBooleanProperty("atSetpoint", this::atSetpoint, null);
  }
}
