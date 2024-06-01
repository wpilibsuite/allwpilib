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

/** Implements a PID control loop. */
public class PIDController implements Sendable, AutoCloseable {
  private static int instances;

  // Factor for "proportional" control
  private double m_kp;

  // Factor for "integral" control
  private double m_ki;

  // Factor for "derivative" control
  private double m_kd;

  // The error range where "integral" control applies
  private double m_iZone = Double.POSITIVE_INFINITY;

  // The period (in seconds) of the loop that calls the controller
  private final double m_period;

  private double m_maximumIntegral = 1.0;

  private double m_minimumIntegral = -1.0;

  private double m_maximumInput;

  private double m_minimumInput;

  // Do the endpoints wrap around? e.g. Absolute encoder
  private boolean m_continuous;

  // The error at the time of the most recent call to calculate()
  private double m_positionError;
  private double m_velocityError;

  // The error at the time of the second-most-recent call to calculate() (used to compute velocity)
  private double m_prevError;

  // The sum of the errors for use in the integral calc
  private double m_totalError;

  // The error that is considered at setpoint.
  private double m_positionTolerance = 0.05;
  private double m_velocityTolerance = Double.POSITIVE_INFINITY;

  private double m_setpoint;
  private double m_measurement;

  private boolean m_haveMeasurement;
  private boolean m_haveSetpoint;

  /**
   * Allocates a PIDController with the given constants for kp, ki, and kd and a default period of
   * 0.02 seconds.
   *
   * @param kp The proportional coefficient.
   * @param ki The integral coefficient.
   * @param kd The derivative coefficient.
   * @throws IllegalArgumentException if kp &lt; 0
   * @throws IllegalArgumentException if ki &lt; 0
   * @throws IllegalArgumentException if kd &lt; 0
   */
  public PIDController(double kp, double ki, double kd) {
    this(kp, ki, kd, 0.02);
  }

  /**
   * Allocates a PIDController with the given constants for kp, ki, and kd.
   *
   * @param kp The proportional coefficient.
   * @param ki The integral coefficient.
   * @param kd The derivative coefficient.
   * @param period The period between controller updates in seconds.
   * @throws IllegalArgumentException if kp &lt; 0
   * @throws IllegalArgumentException if ki &lt; 0
   * @throws IllegalArgumentException if kd &lt; 0
   * @throws IllegalArgumentException if period &lt;= 0
   */
  @SuppressWarnings("this-escape")
  public PIDController(double kp, double ki, double kd, double period) {
    m_kp = kp;
    m_ki = ki;
    m_kd = kd;

    if (kp < 0.0) {
      throw new IllegalArgumentException("Kp must be a non-negative number!");
    }
    if (ki < 0.0) {
      throw new IllegalArgumentException("Ki must be a non-negative number!");
    }
    if (kd < 0.0) {
      throw new IllegalArgumentException("Kd must be a non-negative number!");
    }
    if (period <= 0.0) {
      throw new IllegalArgumentException("Controller period must be a positive number!");
    }
    m_period = period;

    instances++;
    SendableRegistry.addLW(this, "PIDController", instances);

    MathSharedStore.reportUsage(MathUsageId.kController_PIDController2, instances);
  }

  @Override
  public void close() {
    SendableRegistry.remove(this);
  }

  /**
   * Sets the PID Controller gain parameters.
   *
   * <p>Set the proportional, integral, and differential coefficients.
   *
   * @param kp The proportional coefficient.
   * @param ki The integral coefficient.
   * @param kd The derivative coefficient.
   */
  public void setPID(double kp, double ki, double kd) {
    m_kp = kp;
    m_ki = ki;
    m_kd = kd;
  }

  /**
   * Sets the Proportional coefficient of the PID controller gain.
   *
   * @param kp The proportional coefficient. Must be &gt;= 0.
   */
  public void setP(double kp) {
    m_kp = kp;
  }

  /**
   * Sets the Integral coefficient of the PID controller gain.
   *
   * @param ki The integral coefficient. Must be &gt;= 0.
   */
  public void setI(double ki) {
    m_ki = ki;
  }

  /**
   * Sets the Differential coefficient of the PID controller gain.
   *
   * @param kd The differential coefficient. Must be &gt;= 0.
   */
  public void setD(double kd) {
    m_kd = kd;
  }

  /**
   * Sets the IZone range. When the absolute value of the position error is greater than IZone, the
   * total accumulated error will reset to zero, disabling integral gain until the absolute value of
   * the position error is less than IZone. This is used to prevent integral windup. Must be
   * non-negative. Passing a value of zero will effectively disable integral gain. Passing a value
   * of {@link Double#POSITIVE_INFINITY} disables IZone functionality.
   *
   * @param iZone Maximum magnitude of error to allow integral control.
   * @throws IllegalArgumentException if iZone &lt; 0
   */
  public void setIZone(double iZone) {
    if (iZone < 0) {
      throw new IllegalArgumentException("IZone must be a non-negative number!");
    }
    m_iZone = iZone;
  }

  /**
   * Get the Proportional coefficient.
   *
   * @return proportional coefficient
   */
  public double getP() {
    return m_kp;
  }

  /**
   * Get the Integral coefficient.
   *
   * @return integral coefficient
   */
  public double getI() {
    return m_ki;
  }

  /**
   * Get the Differential coefficient.
   *
   * @return differential coefficient
   */
  public double getD() {
    return m_kd;
  }

  /**
   * Get the IZone range.
   *
   * @return Maximum magnitude of error to allow integral control.
   */
  public double getIZone() {
    return m_iZone;
  }

  /**
   * Returns the period of this controller.
   *
   * @return the period of the controller.
   */
  public double getPeriod() {
    return m_period;
  }

  /**
   * Returns the position tolerance of this controller.
   *
   * @return the position tolerance of the controller.
   */
  public double getPositionTolerance() {
    return m_positionTolerance;
  }

  /**
   * Returns the velocity tolerance of this controller.
   *
   * @return the velocity tolerance of the controller.
   */
  public double getVelocityTolerance() {
    return m_velocityTolerance;
  }

  /**
   * Sets the setpoint for the PIDController.
   *
   * @param setpoint The desired setpoint.
   */
  public void setSetpoint(double setpoint) {
    m_setpoint = setpoint;
    m_haveSetpoint = true;

    if (m_continuous) {
      double errorBound = (m_maximumInput - m_minimumInput) / 2.0;
      m_positionError = MathUtil.inputModulus(m_setpoint - m_measurement, -errorBound, errorBound);
    } else {
      m_positionError = m_setpoint - m_measurement;
    }

    m_velocityError = (m_positionError - m_prevError) / m_period;
  }

  /**
   * Returns the current setpoint of the PIDController.
   *
   * @return The current setpoint.
   */
  public double getSetpoint() {
    return m_setpoint;
  }

  /**
   * Returns true if the error is within the tolerance of the setpoint.
   *
   * <p>This will return false until at least one input value has been computed.
   *
   * @return Whether the error is within the acceptable bounds.
   */
  public boolean atSetpoint() {
    return m_haveMeasurement
        && m_haveSetpoint
        && Math.abs(m_positionError) < m_positionTolerance
        && Math.abs(m_velocityError) < m_velocityTolerance;
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
   * Sets the minimum and maximum contributions of the integral term.
   *
   * <p>The internal integrator is clamped so that the integral term's contribution to the output
   * stays between minimumIntegral and maximumIntegral. This prevents integral windup.
   *
   * @param minimumIntegral The minimum contribution of the integral term.
   * @param maximumIntegral The maximum contribution of the integral term.
   */
  public void setIntegratorRange(double minimumIntegral, double maximumIntegral) {
    m_minimumIntegral = minimumIntegral;
    m_maximumIntegral = maximumIntegral;
  }

  /**
   * Sets the error which is considered tolerable for use with atSetpoint().
   *
   * @param positionTolerance Position error which is tolerable.
   */
  public void setTolerance(double positionTolerance) {
    setTolerance(positionTolerance, Double.POSITIVE_INFINITY);
  }

  /**
   * Sets the error which is considered tolerable for use with atSetpoint().
   *
   * @param positionTolerance Position error which is tolerable.
   * @param velocityTolerance Velocity error which is tolerable.
   */
  public void setTolerance(double positionTolerance, double velocityTolerance) {
    m_positionTolerance = positionTolerance;
    m_velocityTolerance = velocityTolerance;
  }

  /**
   * Returns the difference between the setpoint and the measurement.
   *
   * @return The error.
   */
  public double getPositionError() {
    return m_positionError;
  }

  /**
   * Returns the velocity error.
   *
   * @return The velocity error.
   */
  public double getVelocityError() {
    return m_velocityError;
  }

  /**
   * Returns the next output of the PID controller.
   *
   * @param measurement The current measurement of the process variable.
   * @param setpoint The new setpoint of the controller.
   * @return The next controller output.
   */
  public double calculate(double measurement, double setpoint) {
    m_setpoint = setpoint;
    m_haveSetpoint = true;
    return calculate(measurement);
  }

  /**
   * Returns the next output of the PID controller.
   *
   * @param measurement The current measurement of the process variable.
   * @return The next controller output.
   */
  public double calculate(double measurement) {
    m_measurement = measurement;
    m_prevError = m_positionError;
    m_haveMeasurement = true;

    if (m_continuous) {
      double errorBound = (m_maximumInput - m_minimumInput) / 2.0;
      m_positionError = MathUtil.inputModulus(m_setpoint - m_measurement, -errorBound, errorBound);
    } else {
      m_positionError = m_setpoint - m_measurement;
    }

    m_velocityError = (m_positionError - m_prevError) / m_period;

    // If the absolute value of the position error is greater than IZone, reset the total error
    if (Math.abs(m_positionError) > m_iZone) {
      m_totalError = 0;
    } else if (m_ki != 0) {
      m_totalError =
          MathUtil.clamp(
              m_totalError + m_positionError * m_period,
              m_minimumIntegral / m_ki,
              m_maximumIntegral / m_ki);
    }

    return m_kp * m_positionError + m_ki * m_totalError + m_kd * m_velocityError;
  }

  /** Resets the previous error and the integral term. */
  public void reset() {
    m_positionError = 0;
    m_prevError = 0;
    m_totalError = 0;
    m_velocityError = 0;
    m_haveMeasurement = false;
  }

  @Override
  public void initSendable(SendableBuilder builder) {
    builder.setSmartDashboardType("PIDController");
    builder.addDoubleProperty("p", this::getP, this::setP);
    builder.addDoubleProperty("i", this::getI, this::setI);
    builder.addDoubleProperty("d", this::getD, this::setD);
    builder.addDoubleProperty(
        "izone",
        this::getIZone,
        (double toSet) -> {
          try {
            setIZone(toSet);
          } catch (IllegalArgumentException e) {
            MathSharedStore.reportError("IZone must be a non-negative number!", e.getStackTrace());
          }
        });
    builder.addDoubleProperty("setpoint", this::getSetpoint, this::setSetpoint);
  }
}
