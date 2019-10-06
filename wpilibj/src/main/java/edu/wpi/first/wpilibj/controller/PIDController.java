/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.controller;

import edu.wpi.first.hal.FRCNetComm.tResourceType;
import edu.wpi.first.hal.HAL;
import edu.wpi.first.wpilibj.Sendable;
import edu.wpi.first.wpilibj.smartdashboard.SendableBuilder;
import edu.wpi.first.wpilibj.smartdashboard.SendableRegistry;
import edu.wpi.first.wpiutil.math.MathUtils;

/**
 * Implements a PID control loop.
 */
@SuppressWarnings("PMD.TooManyFields")
public class PIDController implements Sendable, AutoCloseable {
  private static int instances;

  // Factor for "proportional" control
  @SuppressWarnings("MemberName")
  private double m_Kp;

  // Factor for "integral" control
  @SuppressWarnings("MemberName")
  private double m_Ki;

  // Factor for "derivative" control
  @SuppressWarnings("MemberName")
  private double m_Kd;

  // The period (in seconds) of the loop that calls the controller
  private final double m_period;

  private double m_maximumIntegral = 1.0;

  private double m_minimumIntegral = -1.0;

  // Maximum input - limit setpoint to this
  private double m_maximumInput;

  // Minimum input - limit setpoint to this
  private double m_minimumInput;

  // Input range - difference between maximum and minimum
  private double m_inputRange;

  // Do the endpoints wrap around? eg. Absolute encoder
  private boolean m_continuous;

  // The error at the time of the most recent call to calculate()
  private double m_positionError;
  private double m_velocityError;

  // The error at the time of the second-most-recent call to calculate() (used to compute velocity)
  private double m_prevError;

  // The sum of the errors for use in the integral calc
  private double m_totalError;

  // The percentage or absolute error that is considered at setpoint.
  private double m_positionTolerance = 0.05;
  private double m_velocityTolerance = Double.POSITIVE_INFINITY;

  private double m_setpoint;

  /**
   * Allocates a PIDController with the given constants for Kp, Ki, and Kd and a default period of
   * 0.02 seconds.
   *
   * @param Kp The proportional coefficient.
   * @param Ki The integral coefficient.
   * @param Kd The derivative coefficient.
   */
  @SuppressWarnings("ParameterName")
  public PIDController(double Kp, double Ki, double Kd) {
    this(Kp, Ki, Kd, 0.02);
  }

  /**
   * Allocates a PIDController with the given constants for Kp, Ki, and Kd.
   *
   * @param Kp     The proportional coefficient.
   * @param Ki     The integral coefficient.
   * @param Kd     The derivative coefficient.
   * @param period The period between controller updates in seconds.
   */
  @SuppressWarnings("ParameterName")
  public PIDController(double Kp, double Ki, double Kd, double period) {
    m_Kp = Kp;
    m_Ki = Ki;
    m_Kd = Kd;

    m_period = period;

    instances++;
    SendableRegistry.addLW(this, "PIDController", instances);

    HAL.report(tResourceType.kResourceType_PIDController, instances);
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
   * @param Kp The proportional coefficient.
   * @param Ki The integral coefficient.
   * @param Kd The derivative coefficient.
   */
  @SuppressWarnings("ParameterName")
  public void setPID(double Kp, double Ki, double Kd) {
    m_Kp = Kp;
    m_Ki = Ki;
    m_Kd = Kd;
  }

  /**
   * Sets the Proportional coefficient of the PID controller gain.
   *
   * @param Kp proportional coefficient
   */
  @SuppressWarnings("ParameterName")
  public void setP(double Kp) {
    m_Kp = Kp;
  }

  /**
   * Sets the Integral coefficient of the PID controller gain.
   *
   * @param Ki integral coefficient
   */
  @SuppressWarnings("ParameterName")
  public void setI(double Ki) {
    m_Ki = Ki;
  }

  /**
   * Sets the Differential coefficient of the PID controller gain.
   *
   * @param Kd differential coefficient
   */
  @SuppressWarnings("ParameterName")
  public void setD(double Kd) {
    m_Kd = Kd;
  }

  /**
   * Get the Proportional coefficient.
   *
   * @return proportional coefficient
   */
  public double getP() {
    return m_Kp;
  }

  /**
   * Get the Integral coefficient.
   *
   * @return integral coefficient
   */
  public double getI() {
    return m_Ki;
  }

  /**
   * Get the Differential coefficient.
   *
   * @return differential coefficient
   */
  public double getD() {
    return m_Kd;
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
   * Sets the setpoint for the PIDController.
   *
   * @param setpoint The desired setpoint.
   */
  public void setSetpoint(double setpoint) {
    if (m_maximumInput > m_minimumInput) {
      m_setpoint = MathUtils.clamp(setpoint, m_minimumInput, m_maximumInput);
    } else {
      m_setpoint = setpoint;
    }
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
   * Returns true if the error is within the percentage of the total input range, determined by
   * SetTolerance. This asssumes that the maximum and minimum input were set using SetInput.
   *
   * <p>This will return false until at least one input value has been computed.
   *
   * @return Whether the error is within the acceptable bounds.
   */
  public boolean atSetpoint() {
    return Math.abs(m_positionError) < m_positionTolerance
        && Math.abs(m_velocityError) < m_velocityTolerance;
  }

  /**
   * Enables continuous input.
   *
   * <p>Rather then using the max and min input range as constraints, it considers
   * them to be the same point and automatically calculates the shortest route
   * to the setpoint.
   *
   * @param minimumInput The minimum value expected from the input.
   * @param maximumInput The maximum value expected from the input.
   */
  public void enableContinuousInput(double minimumInput, double maximumInput) {
    m_continuous = true;
    setInputRange(minimumInput, maximumInput);
  }

  /**
   * Disables continuous input.
   */
  public void disableContinuousInput() {
    m_continuous = false;
  }

  /**
   * Sets the minimum and maximum values for the integrator.
   *
   * <p>When the cap is reached, the integrator value is added to the controller
   * output rather than the integrator value times the integral gain.
   *
   * @param minimumIntegral The minimum value of the integrator.
   * @param maximumIntegral The maximum value of the integrator.
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
    return getContinuousError(m_positionError);
  }

  /**
   * Returns the velocity error.
   */
  public double getVelocityError() {
    return m_velocityError;
  }

  /**
   * Returns the next output of the PID controller.
   *
   * @param measurement The current measurement of the process variable.
   * @param setpoint    The new setpoint of the controller.
   */
  public double calculate(double measurement, double setpoint) {
    // Set setpoint to provided value
    setSetpoint(setpoint);
    return calculate(measurement);
  }

  /**
   * Returns the next output of the PID controller.
   *
   * @param measurement The current measurement of the process variable.
   */
  public double calculate(double measurement) {
    m_prevError = m_positionError;
    m_positionError = getContinuousError(m_setpoint - measurement);
    m_velocityError = (m_positionError - m_prevError) / m_period;

    if (m_Ki != 0) {
      m_totalError = MathUtils.clamp(m_totalError + m_positionError * m_period,
          m_minimumIntegral / m_Ki, m_maximumIntegral / m_Ki);
    }

    return m_Kp * m_positionError + m_Ki * m_totalError + m_Kd * m_velocityError;
  }

  /**
   * Resets the previous error and the integral term. Also disables the controller.
   */
  public void reset() {
    m_prevError = 0;
    m_totalError = 0;
  }

  @Override
  public void initSendable(SendableBuilder builder) {
    builder.setSmartDashboardType("PIDController");
    builder.addDoubleProperty("p", this::getP, this::setP);
    builder.addDoubleProperty("i", this::getI, this::setI);
    builder.addDoubleProperty("d", this::getD, this::setD);
    builder.addDoubleProperty("setpoint", this::getSetpoint, this::setSetpoint);
  }

  /**
   * Wraps error around for continuous inputs. The original error is returned if continuous mode is
   * disabled.
   *
   * @param error The current error of the PID controller.
   * @return Error for continuous inputs.
   */
  protected double getContinuousError(double error) {
    if (m_continuous && m_inputRange > 0) {
      error %= m_inputRange;
      if (Math.abs(error) > m_inputRange / 2) {
        if (error > 0) {
          return error - m_inputRange;
        } else {
          return error + m_inputRange;
        }
      }
    }
    return error;
  }

  /**
   * Sets the minimum and maximum values expected from the input.
   *
   * @param minimumInput The minimum value expected from the input.
   * @param maximumInput The maximum value expected from the input.
   */
  private void setInputRange(double minimumInput, double maximumInput) {
    m_minimumInput = minimumInput;
    m_maximumInput = maximumInput;
    m_inputRange = maximumInput - minimumInput;

    // Clamp setpoint to new input
    if (m_maximumInput > m_minimumInput) {
      m_setpoint = MathUtils.clamp(m_setpoint, m_minimumInput, m_maximumInput);
    }
  }
}
