// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.controller;

import edu.wpi.first.util.sendable.Sendable;
import edu.wpi.first.util.sendable.SendableBuilder;

/**
 * A helper class that computes feedforward outputs for a simple elevator (modeled as a motor acting
 * against the force of gravity directly).
 */
public class ElevatorFeedforward implements Sendable {
  private final SimpleMotorFeedforward m_simpleFeedforward;
  private double m_kg;

  /**
   * Creates a new ElevatorFeedforward with the specified gains. Units of the gain values will
   * dictate units of the computed feedforward.
   *
   * @param ks The static gain.
   * @param kg The gravity gain.
   * @param kv The velocity gain.
   * @param ka The acceleration gain.
   */
  public ElevatorFeedforward(double ks, double kg, double kv, double ka) {
    m_simpleFeedforward = new SimpleMotorFeedforward(ks, kv, ka);
    this.m_kg = kg;
  }

  /**
   * Creates a new ElevatorFeedforward with the specified gains. Acceleration gain is defaulted to
   * zero. Units of the gain values will dictate units of the computed feedforward.
   *
   * @param ks The static gain.
   * @param kg The gravity gain.
   * @param kv The velocity gain.
   */
  public ElevatorFeedforward(double ks, double kg, double kv) {
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
   * Gets the most recent output.
   *
   * @return Most recent output.
   */
  public double getOutput() {
    return m_simpleFeedforward.getOutput() + m_kg;
  }

  /**
   * Calculates the feedforward from the gains and setpoints.
   *
   * @param currentVelocity The current velocity setpoint.
   * @param nextVelocity The next velocity setpoint.
   * @param dtSeconds The time until the next velocity setpoint.
   * @return The computed feedforward.
   */
  public double calculate(double currentVelocity, double nextVelocity, double dtSeconds) {
    return m_simpleFeedforward.calculate(currentVelocity, nextVelocity, dtSeconds) + m_kg;
  }

  /**
   * Calculates the feedforward from the gains and setpoints.
   *
   * @param velocity The velocity setpoint.
   * @param acceleration The acceleration setpoint.
   * @return The computed feedforward.
   */
  public double calculate(double velocity, double acceleration) {
    return m_simpleFeedforward.calculate(velocity, acceleration) + m_kg;
  }
  // Rearranging the main equation from the calculate() method yields the
  // formulas for the methods below:

  /**
   * Calculates the maximum achievable velocity given a maximum voltage supply and an acceleration.
   * Useful for ensuring that velocity and acceleration constraints for a trapezoidal profile are
   * simultaneously achievable - enter the acceleration constraint, and this will give you a
   * simultaneously-achievable velocity constraint.
   *
   * @param maxVoltage The maximum voltage that can be supplied to the elevator.
   * @param acceleration The acceleration of the elevator.
   * @return The maximum possible velocity at the given acceleration.
   */
  public double maxAchievableVelocity(double maxVoltage, double acceleration) {
    // Assume max velocity is positive
    return m_simpleFeedforward.maxAchievableVelocity(maxVoltage, acceleration)
        - m_kg / m_simpleFeedforward.getKv();
  }

  /**
   * Calculates the minimum achievable velocity given a maximum voltage supply and an acceleration.
   * Useful for ensuring that velocity and acceleration constraints for a trapezoidal profile are
   * simultaneously achievable - enter the acceleration constraint, and this will give you a
   * simultaneously-achievable velocity constraint.
   *
   * @param maxVoltage The maximum voltage that can be supplied to the elevator.
   * @param acceleration The acceleration of the elevator.
   * @return The minimum possible velocity at the given acceleration.
   */
  public double minAchievableVelocity(double maxVoltage, double acceleration) {
    // Assume min velocity is negative, ks flips sign
    return m_simpleFeedforward.minAchievableVelocity(maxVoltage, acceleration)
        - m_kg / m_simpleFeedforward.getKv();
  }

  /**
   * Calculates the maximum achievable acceleration given a maximum voltage supply and a velocity.
   * Useful for ensuring that velocity and acceleration constraints for a trapezoidal profile are
   * simultaneously achievable - enter the velocity constraint, and this will give you a
   * simultaneously-achievable acceleration constraint.
   *
   * @param maxVoltage The maximum voltage that can be supplied to the elevator.
   * @param velocity The velocity of the elevator.
   * @return The maximum possible acceleration at the given velocity.
   */
  public double maxAchievableAcceleration(double maxVoltage, double velocity) {
    return m_simpleFeedforward.maxAchievableAcceleration(maxVoltage, velocity)
        - m_kg / m_simpleFeedforward.getKa();
  }

  /**
   * Calculates the minimum achievable acceleration given a maximum voltage supply and a velocity.
   * Useful for ensuring that velocity and acceleration constraints for a trapezoidal profile are
   * simultaneously achievable - enter the velocity constraint, and this will give you a
   * simultaneously-achievable acceleration constraint.
   *
   * @param maxVoltage The maximum voltage that can be supplied to the elevator.
   * @param velocity The velocity of the elevator.
   * @return The minimum possible acceleration at the given velocity.
   */
  public double minAchievableAcceleration(double maxVoltage, double velocity) {
    return maxAchievableAcceleration(-maxVoltage, velocity);
  }

  @Override
  public void initSendable(SendableBuilder builder) {
    m_simpleFeedforward.initSendable(builder);
    builder.addDoubleProperty("kG", this::getKg, this::setKg);
    builder.addDoubleProperty("output", () -> getOutput() + m_kg, null);
  }
}
