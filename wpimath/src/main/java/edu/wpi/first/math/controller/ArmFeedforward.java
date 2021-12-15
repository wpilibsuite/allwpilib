// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.controller;

import edu.wpi.first.util.sendable.Sendable;
import edu.wpi.first.util.sendable.SendableBuilder;

/**
 * A helper class that computes feedforward outputs for a simple arm (modeled as a motor acting
 * against the force of gravity on a beam suspended at an angle).
 */
@SuppressWarnings("MemberName")
public class ArmFeedforward implements Sendable {
  private double m_kg;
  private double m_angleRadians;
  private final SimpleMotorFeedforward m_simpleFeedforward;

  /**
   * Creates a new ArmFeedforward with the specified gains. Units of the gain values will dictate
   * units of the computed feedforward.
   *
   * @param ks The static gain.
   * @param kg The gravity gain.
   * @param kv The velocity gain.
   * @param ka The acceleration gain.
   */
  public ArmFeedforward(double ks, double kg, double kv, double ka) {
    m_simpleFeedforward = new SimpleMotorFeedforward(ks, kv, ka);
    this.m_kg = kg;
  }

  /**
   * Creates a new ArmFeedforward with the specified gains. Acceleration gain is defaulted to zero.
   * Units of the gain values will dictate units of the computed feedforward.
   *
   * @param ks The static gain.
   * @param kg The gravity gain.
   * @param kv The velocity gain.
   */
  public ArmFeedforward(double ks, double kg, double kv) {
    this(ks, kg, kv, 0);
  }

  /**
   * Gets the gravity compensation term of the feedforward.
   *
   * @return The gravity compensation gain.
   */
  public double getKg() {
    return m_kg;
  }

  /**
   * Sets the gravity compensation term of the feedforward.
   *
   * @param kg The gravity compensation gain.
   */
  public void setKg(double kg) {
    this.m_kg = kg;
  }

  /**
   * Gets the SimpleMotorFeedforward that describes the motor without the effect of gravity.
   *
   * @return The internal SimpleMotorFeedforward.
   */
  public SimpleMotorFeedforward getSimpleFeedforward() {
    return m_simpleFeedforward;
  }

  /**
   * Returns the most recent angle of the arm, in radians.
   *
   * @return The position of the arm in radians.
   */
  public double getAngleRadians() {
    return m_angleRadians;
  }

  /**
   * Gets the most recent output.
   *
   * @return Most recent output.
   */
  public double getOutput() {
    return m_simpleFeedforward.getOutput() + m_kg * Math.cos(m_angleRadians);
  }

  /**
   * Calculates the feedforward from the gains and setpoints.
   *
   * @param positionRadians The position (angle) setpoint.
   * @param currentVelocityRadPerSec The current velocity setpoint.
   * @param nextVelocityRadPerSec The next velocity setpoint.
   * @param dtSeconds The time until the next velocity setpoint.
   * @return The computed feedforward.
   */
  public double calculate(
      double positionRadians,
      double currentVelocityRadPerSec,
      double nextVelocityRadPerSec,
      double dtSeconds) {
    m_angleRadians = positionRadians;
    return m_simpleFeedforward.calculate(currentVelocityRadPerSec, nextVelocityRadPerSec, dtSeconds)
        + m_kg * Math.cos(positionRadians);
  }

  /**
   * Calculates the feedforward from the gains and setpoints.
   *
   * @param positionRadians The position (angle) setpoint.
   * @param velocityRadPerSec The velocity setpoint.
   * @param accelRadPerSecSquared The acceleration setpoint.
   * @return The computed feedforward.
   */
  public double calculate(
      double positionRadians, double velocityRadPerSec, double accelRadPerSecSquared) {
    return calculate(
        positionRadians, velocityRadPerSec, velocityRadPerSec + 0.02 * accelRadPerSecSquared, 0.02);
  }

  /**
   * Calculates the feedforward from the gains and velocity setpoint (acceleration is assumed to be
   * zero).
   *
   * @param positionRadians The position (angle) setpoint.
   * @param velocity The velocity setpoint.
   * @return The computed feedforward.
   */
  public double calculate(double positionRadians, double velocity) {
    return calculate(positionRadians, velocity, 0);
  }

  // Rearranging the main equation from the calculate() method yields the
  // formulas for the methods below:

  /**
   * Calculates the maximum achievable velocity given a maximum voltage supply, a position, and an
   * acceleration. Useful for ensuring that velocity and acceleration constraints for a trapezoidal
   * profile are simultaneously achievable - enter the acceleration constraint, and this will give
   * you a simultaneously-achievable velocity constraint.
   *
   * @param maxVoltage The maximum voltage that can be supplied to the arm.
   * @param angle The angle of the arm.
   * @param acceleration The acceleration of the arm.
   * @return The maximum possible velocity at the given acceleration and angle.
   */
  public double maxAchievableVelocity(double maxVoltage, double angle, double acceleration) {
    // Assume max velocity is positive
    return m_simpleFeedforward.maxAchievableVelocity(maxVoltage, acceleration)
        - Math.cos(angle) * m_kg / m_simpleFeedforward.getKv();
  }

  /**
   * Calculates the minimum achievable velocity given a maximum voltage supply, a position, and an
   * acceleration. Useful for ensuring that velocity and acceleration constraints for a trapezoidal
   * profile are simultaneously achievable - enter the acceleration constraint, and this will give
   * you a simultaneously-achievable velocity constraint.
   *
   * @param maxVoltage The maximum voltage that can be supplied to the arm.
   * @param angle The angle of the arm.
   * @param acceleration The acceleration of the arm.
   * @return The minimum possible velocity at the given acceleration and angle.
   */
  public double minAchievableVelocity(double maxVoltage, double angle, double acceleration) {
    // Assume min velocity is negative, ks flips sign
    return m_simpleFeedforward.minAchievableVelocity(maxVoltage, acceleration)
        - Math.cos(angle) * m_kg / m_simpleFeedforward.getKv();
  }

  /**
   * Calculates the maximum achievable acceleration given a maximum voltage supply, a position, and
   * a velocity. Useful for ensuring that velocity and acceleration constraints for a trapezoidal
   * profile are simultaneously achievable - enter the velocity constraint, and this will give you a
   * simultaneously-achievable acceleration constraint.
   *
   * @param maxVoltage The maximum voltage that can be supplied to the arm.
   * @param angle The angle of the arm.
   * @param velocity The velocity of the arm.
   * @return The maximum possible acceleration at the given velocity.
   */
  public double maxAchievableAcceleration(double maxVoltage, double angle, double velocity) {
    return m_simpleFeedforward.maxAchievableAcceleration(maxVoltage, velocity)
        - Math.cos(angle) * m_kg / m_simpleFeedforward.getKa();
  }

  /**
   * Calculates the minimum achievable acceleration given a maximum voltage supply, a position, and
   * a velocity. Useful for ensuring that velocity and acceleration constraints for a trapezoidal
   * profile are simultaneously achievable - enter the velocity constraint, and this will give you a
   * simultaneously-achievable acceleration constraint.
   *
   * @param maxVoltage The maximum voltage that can be supplied to the arm.
   * @param angle The angle of the arm.
   * @param velocity The velocity of the arm.
   * @return The minimum possible acceleration at the given velocity.
   */
  public double minAchievableAcceleration(double maxVoltage, double angle, double velocity) {
    return maxAchievableAcceleration(-maxVoltage, angle, velocity);
  }

  @Override
  public void initSendable(SendableBuilder builder) {
    m_simpleFeedforward.initSendable(builder);
    builder.setSmartDashboardType("ArmFeedforward");
    builder.addDoubleProperty("kG", this::getKg, this::setKg);
    builder.addDoubleProperty("gravityOutput", () -> m_kg * Math.cos(m_angleRadians), null);
    builder.addDoubleProperty("output", this::getOutput, null);
  }
}
