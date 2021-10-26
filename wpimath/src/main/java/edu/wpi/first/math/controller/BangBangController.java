// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.controller;

import edu.wpi.first.math.MathSharedStore;
import edu.wpi.first.math.MathUsageId;
import edu.wpi.first.math.MathUtil;
import edu.wpi.first.util.sendable.Sendable;
import edu.wpi.first.util.sendable.SendableBuilder;
import edu.wpi.first.util.sendable.SendableRegistry;

/**
 * Implements a bang-bang controller, which outputs 0, 1, or -1.
 *
 * <p>Bang-bang control is extremely simple, but also potentially hazardous.
 * Always ensure that your motor controllers are set to "coast" before
 * attempting to control them with a bang-bang controller.
 */
public class BangBangController implements Sendable {
    /**
     * The type of bang-bang control.  A symmetric controller will
     * control the motor in both directions, while the forward/reverse
     * controllers will output zero in other direction. Symmetric controllers
     * should *never* be used on high-inertia mechanisms.
     */
    public enum ControlType {
        kForward,
        kReverse,
        kSymmetric
    }

    private static int instances = 0;

    private ControlType m_type;

    private double m_tolerance;

    private boolean m_continuous;
    private double m_maximumInput;
    private double m_minimumInput;

    private double m_setpoint;
    private double m_measurement;

    /**
     * Creates a new bang-bang controller.
     *
     * @param tolerance The controller's error tolerance, within which the controller
     *                  will output zero.
     * @param type The type of bang-bang controller. *Never* use a symmetric controller
     *             on a high-inertia mechanism.
     */
    public BangBangController(
        double tolerance,
        ControlType type
    ) {
        instances++;

        m_tolerance = tolerance;
        m_type = type;

        SendableRegistry.addLW(this, "BangBangController", instances);

        MathSharedStore.reportUsage(MathUsageId.kController_PIDController2, instances);
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
     * Sets the error within which the controller will not output.
     *
     * @param tolerance Position error which is tolerable.
     */
    public void setTolerance(double tolerance) {
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
     * Returns true if the error is within the tolerance of the setpoint.
     *
     * <p>This will return false until at least one input value has been computed.
     *
     * @return Whether the error is within the acceptable bounds.
     */
    public boolean atSetpoint() {
        double positionError;
        if (m_continuous) {
            double errorBound = (m_maximumInput - m_minimumInput) / 2.0;
            positionError = MathUtil.inputModulus(m_setpoint - m_measurement, -errorBound, errorBound);
        } else {
            positionError = m_setpoint - m_measurement;
        }

        return Math.abs(positionError) < m_tolerance;
    }

    /**
     * Enables continuous input.
     *
     * <p>Rather then using the max and min input range as constraints, it considers them to be the
     * same point and automatically calculates the shortest route to the setpoint.
     *
     * @param minimumInput The minimum value expected from the input.
     * @param maximumInput The maximum value expected from the input.
     */
    public void enableContinuousInput(double minimumInput, double maximumInput) {
        m_continuous = true;
        m_minimumInput = minimumInput;
        m_maximumInput = maximumInput;
    }

    /** Disables continuous input. */
    public void disableContinuousInput() {
        m_continuous = false;
    }

    /**
     * Returns true if continuous input is enabled.
     *
     * @return True if continuous input is enabled.
     */
    public boolean isContinuousInputEnabled() {
        return m_continuous;
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
     * @param measurement The most recent measurement of the process variable.
     * @param setpoint The setpoint for the process variable.
     * @return The calculated motor output (-1, 0, or 1).
     */
    public double calculate(double measurement, double setpoint) {
        m_measurement = measurement;
        m_setpoint = setpoint;

        switch (m_type) {
            case kForward:
                return !atSetpoint() && measurement < setpoint ? 1 : 0;
            case kReverse:
                return !atSetpoint() && measurement > setpoint ? -1 : 0;
            case kSymmetric:
                return !atSetpoint() ? Math.signum(setpoint - measurement) : 0;
            default:
                return 0;
        }
    }

    /**
     * Returns the calculated control output.
     *
     * @param measurement The most recent measurement of the process variable.
     * @return The calculated motor output (-1, 0, or 1).
     */
    public double calculate(double measurement) {
        return calculate(measurement, m_setpoint);
    }

    @Override
    public void initSendable(SendableBuilder builder) {
        builder.setSmartDashboardType("BangBangController");
        builder.addDoubleProperty("tolerance", this::getTolerance, this::setTolerance);
        builder.addDoubleProperty("setpoint", this::getSetpoint, this::setSetpoint);
        builder.addDoubleProperty("measurement", this::getMeasurement, (double m) -> {});
    }
}