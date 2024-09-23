// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.controller;

import edu.wpi.first.math.MathSharedStore;
import edu.wpi.first.math.MathUsageId;
import edu.wpi.first.math.MathUtil;
import edu.wpi.first.math.trajectory.TrapezoidProfile;
import edu.wpi.first.util.sendable.Sendable;
import edu.wpi.first.util.sendable.SendableBuilder;
import edu.wpi.first.util.sendable.SendableRegistry;

/**
 * Implements a PID control loop whose setpoint is constrained by a trapezoid profile. Users should
 * call reset() when they first start running the controller to avoid unwanted behavior.
 */
public class ProfiledPIDController implements Sendable {
  private static int instances;

  private PIDController m_controller;
  private double m_minimumInput;
  private double m_maximumInput;

  private TrapezoidProfile.Constraints m_constraints;
  private TrapezoidProfile m_profile;
  private TrapezoidProfile.State m_goal = new TrapezoidProfile.State();
  private TrapezoidProfile.State m_setpoint = new TrapezoidProfile.State();

  /**
   * Allocates a ProfiledPIDController with the given constants for Kp, Ki, and Kd.
   *
   * @param Kp The proportional coefficient.
   * @param Ki The integral coefficient.
   * @param Kd The derivative coefficient.
   * @param constraints Velocity and acceleration constraints for goal.
   * @throws IllegalArgumentException if kp &lt; 0
   * @throws IllegalArgumentException if ki &lt; 0
   * @throws IllegalArgumentException if kd &lt; 0
   */
  public ProfiledPIDController(
      double Kp, double Ki, double Kd, TrapezoidProfile.Constraints constraints) {
    this(Kp, Ki, Kd, constraints, 0.02);
  }

  /**
   * Allocates a ProfiledPIDController with the given constants for Kp, Ki, and Kd.
   *
   * @param Kp The proportional coefficient.
   * @param Ki The integral coefficient.
   * @param Kd The derivative coefficient.
   * @param constraints Velocity and acceleration constraints for goal.
   * @param period The period between controller updates in seconds. The default is 0.02 seconds.
   * @throws IllegalArgumentException if kp &lt; 0
   * @throws IllegalArgumentException if ki &lt; 0
   * @throws IllegalArgumentException if kd &lt; 0
   * @throws IllegalArgumentException if period &lt;= 0
   */
  @SuppressWarnings("this-escape")
  public ProfiledPIDController(
      double Kp, double Ki, double Kd, TrapezoidProfile.Constraints constraints, double period) {
    m_controller = new PIDController(Kp, Ki, Kd, period);
    m_constraints = constraints;
    m_profile = new TrapezoidProfile(m_constraints);
    instances++;

    SendableRegistry.add(this, "ProfiledPIDController", instances);
    MathSharedStore.reportUsage(MathUsageId.kController_ProfiledPIDController, instances);
  }

  /**
   * Sets the PID Controller gain parameters.
   *
   * <p>Sets the proportional, integral, and differential coefficients.
   *
   * @param Kp The proportional coefficient. Must be &gt;= 0.
   * @param Ki The integral coefficient. Must be &gt;= 0.
   * @param Kd The differential coefficient. Must be &gt;= 0.
   */
  public void setPID(double Kp, double Ki, double Kd) {
    m_controller.setPID(Kp, Ki, Kd);
  }

  /**
   * Sets the proportional coefficient of the PID controller gain.
   *
   * @param Kp The proportional coefficient. Must be &gt;= 0.
   */
  public void setP(double Kp) {
    m_controller.setP(Kp);
  }

  /**
   * Sets the integral coefficient of the PID controller gain.
   *
   * @param Ki The integral coefficient. Must be &gt;= 0.
   */
  public void setI(double Ki) {
    m_controller.setI(Ki);
  }

  /**
   * Sets the differential coefficient of the PID controller gain.
   *
   * @param Kd The differential coefficient. Must be &gt;= 0.
   */
  public void setD(double Kd) {
    m_controller.setD(Kd);
  }

  /**
   * Sets the IZone range. When the absolute value of the position error is greater than IZone, the
   * total accumulated error will reset to zero, disabling integral gain until the absolute value of
   * the position error is less than IZone. This is used to prevent integral windup. Must be
   * non-negative. Passing a value of zero will effectively disable integral gain. Passing a value
   * of {@link Double#POSITIVE_INFINITY} disables IZone functionality.
   *
   * @param iZone Maximum magnitude of error to allow integral control.
   * @throws IllegalArgumentException if iZone &lt;= 0
   */
  public void setIZone(double iZone) {
    m_controller.setIZone(iZone);
  }

  /**
   * Gets the proportional coefficient.
   *
   * @return proportional coefficient
   */
  public double getP() {
    return m_controller.getP();
  }

  /**
   * Gets the integral coefficient.
   *
   * @return integral coefficient
   */
  public double getI() {
    return m_controller.getI();
  }

  /**
   * Gets the differential coefficient.
   *
   * @return differential coefficient
   */
  public double getD() {
    return m_controller.getD();
  }

  /**
   * Get the IZone range.
   *
   * @return Maximum magnitude of error to allow integral control.
   */
  public double getIZone() {
    return m_controller.getIZone();
  }

  /**
   * Gets the period of this controller.
   *
   * @return The period of the controller.
   */
  public double getPeriod() {
    return m_controller.getPeriod();
  }

  /**
   * Returns the position tolerance of this controller.
   *
   * @return the position tolerance of the controller.
   */
  public double getPositionTolerance() {
    return m_controller.getErrorTolerance();
  }

  /**
   * Returns the velocity tolerance of this controller.
   *
   * @return the velocity tolerance of the controller.
   */
  public double getVelocityTolerance() {
    return m_controller.getErrorDerivativeTolerance();
  }

  /**
   * Returns the accumulated error used in the integral calculation of this controller.
   *
   * @return The accumulated error of this controller.
   */
  public double getAccumulatedError() {
    return m_controller.getAccumulatedError();
  }

  /**
   * Sets the goal for the ProfiledPIDController.
   *
   * @param goal The desired goal state.
   */
  public void setGoal(TrapezoidProfile.State goal) {
    m_goal = goal;
  }

  /**
   * Sets the goal for the ProfiledPIDController.
   *
   * @param goal The desired goal position.
   */
  public void setGoal(double goal) {
    m_goal = new TrapezoidProfile.State(goal, 0);
  }

  /**
   * Gets the goal for the ProfiledPIDController.
   *
   * @return The goal.
   */
  public TrapezoidProfile.State getGoal() {
    return m_goal;
  }

  /**
   * Returns true if the error is within the tolerance of the error.
   *
   * <p>This will return false until at least one input value has been computed.
   *
   * @return True if the error is within the tolerance of the error.
   */
  public boolean atGoal() {
    return atSetpoint() && m_goal.equals(m_setpoint);
  }

  /**
   * Set velocity and acceleration constraints for goal.
   *
   * @param constraints Velocity and acceleration constraints for goal.
   */
  public void setConstraints(TrapezoidProfile.Constraints constraints) {
    m_constraints = constraints;
    m_profile = new TrapezoidProfile(m_constraints);
  }

  /**
   * Get the velocity and acceleration constraints for this controller.
   *
   * @return Velocity and acceleration constraints.
   */
  public TrapezoidProfile.Constraints getConstraints() {
    return m_constraints;
  }

  /**
   * Returns the current setpoint of the ProfiledPIDController.
   *
   * @return The current setpoint.
   */
  public TrapezoidProfile.State getSetpoint() {
    return m_setpoint;
  }

  /**
   * Returns true if the error is within the tolerance of the error.
   *
   * <p>This will return false until at least one input value has been computed.
   *
   * @return True if the error is within the tolerance of the error.
   */
  public boolean atSetpoint() {
    return m_controller.atSetpoint();
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
    m_controller.enableContinuousInput(minimumInput, maximumInput);
    m_minimumInput = minimumInput;
    m_maximumInput = maximumInput;
  }

  /** Disables continuous input. */
  public void disableContinuousInput() {
    m_controller.disableContinuousInput();
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
    m_controller.setIntegratorRange(minimumIntegral, maximumIntegral);
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
    m_controller.setTolerance(positionTolerance, velocityTolerance);
  }

  /**
   * Returns the difference between the setpoint and the measurement.
   *
   * @return The error.
   */
  public double getPositionError() {
    return m_controller.getError();
  }

  /**
   * Returns the change in error per second.
   *
   * @return The change in error per second.
   */
  public double getVelocityError() {
    return m_controller.getErrorDerivative();
  }

  /**
   * Returns the next output of the PID controller.
   *
   * @param measurement The current measurement of the process variable.
   * @return The controller's next output.
   */
  public double calculate(double measurement) {
    if (m_controller.isContinuousInputEnabled()) {
      // Get error which is the smallest distance between goal and measurement
      double errorBound = (m_maximumInput - m_minimumInput) / 2.0;
      double goalMinDistance =
          MathUtil.inputModulus(m_goal.position - measurement, -errorBound, errorBound);
      double setpointMinDistance =
          MathUtil.inputModulus(m_setpoint.position - measurement, -errorBound, errorBound);

      // Recompute the profile goal with the smallest error, thus giving the shortest path. The goal
      // may be outside the input range after this operation, but that's OK because the controller
      // will still go there and report an error of zero. In other words, the setpoint only needs to
      // be offset from the measurement by the input range modulus; they don't need to be equal.
      m_goal.position = goalMinDistance + measurement;
      m_setpoint.position = setpointMinDistance + measurement;
    }

    m_setpoint = m_profile.calculate(getPeriod(), m_setpoint, m_goal);
    return m_controller.calculate(measurement, m_setpoint.position);
  }

  /**
   * Returns the next output of the PID controller.
   *
   * @param measurement The current measurement of the process variable.
   * @param goal The new goal of the controller.
   * @return The controller's next output.
   */
  public double calculate(double measurement, TrapezoidProfile.State goal) {
    setGoal(goal);
    return calculate(measurement);
  }

  /**
   * Returns the next output of the PIDController.
   *
   * @param measurement The current measurement of the process variable.
   * @param goal The new goal of the controller.
   * @return The controller's next output.
   */
  public double calculate(double measurement, double goal) {
    setGoal(goal);
    return calculate(measurement);
  }

  /**
   * Returns the next output of the PID controller.
   *
   * @param measurement The current measurement of the process variable.
   * @param goal The new goal of the controller.
   * @param constraints Velocity and acceleration constraints for goal.
   * @return The controller's next output.
   */
  public double calculate(
      double measurement, TrapezoidProfile.State goal, TrapezoidProfile.Constraints constraints) {
    setConstraints(constraints);
    return calculate(measurement, goal);
  }

  /**
   * Reset the previous error and the integral term.
   *
   * @param measurement The current measured State of the system.
   */
  public void reset(TrapezoidProfile.State measurement) {
    m_controller.reset();
    m_setpoint = measurement;
  }

  /**
   * Reset the previous error and the integral term.
   *
   * @param measuredPosition The current measured position of the system.
   * @param measuredVelocity The current measured velocity of the system.
   */
  public void reset(double measuredPosition, double measuredVelocity) {
    reset(new TrapezoidProfile.State(measuredPosition, measuredVelocity));
  }

  /**
   * Reset the previous error and the integral term.
   *
   * @param measuredPosition The current measured position of the system. The velocity is assumed to
   *     be zero.
   */
  public void reset(double measuredPosition) {
    reset(measuredPosition, 0.0);
  }

  @Override
  public void initSendable(SendableBuilder builder) {
    builder.setSmartDashboardType("ProfiledPIDController");
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
    builder.addDoubleProperty(
        "maxVelocity",
        () -> getConstraints().maxVelocity,
        maxVelocity ->
            setConstraints(
                new TrapezoidProfile.Constraints(maxVelocity, getConstraints().maxAcceleration)));
    builder.addDoubleProperty(
        "maxAcceleration",
        () -> getConstraints().maxAcceleration,
        maxAcceleration ->
            setConstraints(
                new TrapezoidProfile.Constraints(getConstraints().maxVelocity, maxAcceleration)));
    builder.addDoubleProperty("goal", () -> getGoal().position, this::setGoal);
  }
}
