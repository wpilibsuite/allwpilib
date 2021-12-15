// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.controller;

import edu.wpi.first.math.Matrix;
import edu.wpi.first.math.Nat;
import edu.wpi.first.math.system.plant.LinearSystemId;
import edu.wpi.first.util.sendable.Sendable;
import edu.wpi.first.util.sendable.SendableBuilder;

/** A helper class that computes feedforward outputs for a simple permanent-magnet DC motor. */
public class SimpleMotorFeedforward implements Sendable {
  private double m_ks;
  private double m_kv;
  private double m_ka;

  private double m_velocity;
  private double m_acceleration;
  private double m_output;

  /**
   * Creates a new SimpleMotorFeedforward with the specified gains. Units of the gain values will
   * dictate units of the computed feedforward.
   *
   * @param ks The static gain.
   * @param kv The velocity gain.
   * @param ka The acceleration gain.
   */
  public SimpleMotorFeedforward(double ks, double kv, double ka) {
    this.m_ks = ks;
    this.m_kv = kv;
    this.m_ka = ka;
  }

  /**
   * Creates a new SimpleMotorFeedforward with the specified gains. Acceleration gain is defaulted
   * to zero. Units of the gain values will dictate units of the computed feedforward.
   *
   * @param ks The static gain.
   * @param kv The velocity gain.
   */
  public SimpleMotorFeedforward(double ks, double kv) {
    this(ks, kv, 0);
  }

  /**
   * Gets the static friction compensation term of the feedforward.
   *
   * @return The static gain.
   */
  public double getKs() {
    return m_ks;
  }

  /**
   * Sets the static friction compensation term of the feedforward.
   *
   * @param ks The static gain.
   */
  public void setKs(double ks) {
    this.m_ks = ks;
  }

  /**
   * Gets the velocity gain of the feedforward.
   *
   * @return The velocity gain.
   */
  public double getKv() {
    return m_kv;
  }

  /**
   * Sets the velocity gain of the feedforward.
   *
   * @param kv The velocity gain.
   */
  public void setKv(double kv) {
    this.m_kv = kv;
  }

  /**
   * Gets the acceleration gain of the feedforward.
   *
   * @return The acceleration gain.
   */
  public double getKa() {
    return m_ka;
  }

  /**
   * Sets the acceleration gain of the feedforward.
   *
   * @param ka The acceleration gain.
   */
  public void setKa(double ka) {
    this.m_ka = ka;
  }

  /**
   * Gets the velocity corresponding the currently-calculated feedforward output.
   *
   * @return Most recent velocity.
   */
  public double getVelocity() {
    return m_velocity;
  }

  /**
   * Gets the acceleration corresponding the currently-calculated feedforward output.
   *
   * @return Most recent acceleration.
   */
  public double getAcceleration() {
    return m_acceleration;
  }

  /**
   * Gets the most recent output.
   *
   * @return Most recent output.
   */
  public double getOutput() {
    return m_output;
  }

  /**
   * Calculates the feedforward from the gains and setpoints.
   *
   * @param velocity The velocity setpoint.
   * @param acceleration The acceleration setpoint.
   * @return The computed feedforward.
   */
  public double calculate(double velocity, double acceleration) {
    return calculate(velocity, velocity + 0.02 * acceleration, 0.02);
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
    m_velocity = currentVelocity;
    m_acceleration = (nextVelocity - currentVelocity) / dtSeconds;

    var plant = LinearSystemId.identifyVelocitySystem(this.m_kv, this.m_ka);
    var feedforward = new LinearPlantInversionFeedforward<>(plant, dtSeconds);

    var r = Matrix.mat(Nat.N1(), Nat.N1()).fill(currentVelocity);
    var nextR = Matrix.mat(Nat.N1(), Nat.N1()).fill(nextVelocity);

    m_output = m_ks * Math.signum(currentVelocity) + feedforward.calculate(r, nextR).get(0, 0);

    return m_output;
  }

  // Rearranging the main equation from the calculate() method yields the
  // formulas for the methods below:

  /**
   * Calculates the feedforward from the gains and velocity setpoint (acceleration is assumed to be
   * zero).
   *
   * @param velocity The velocity setpoint.
   * @return The computed feedforward.
   */
  public double calculate(double velocity) {
    return calculate(velocity, 0);
  }

  /**
   * Calculates the maximum achievable velocity given a maximum voltage supply and an acceleration.
   * Useful for ensuring that velocity and acceleration constraints for a trapezoidal profile are
   * simultaneously achievable - enter the acceleration constraint, and this will give you a
   * simultaneously-achievable velocity constraint.
   *
   * @param maxVoltage The maximum voltage that can be supplied to the motor.
   * @param acceleration The acceleration of the motor.
   * @return The maximum possible velocity at the given acceleration.
   */
  public double maxAchievableVelocity(double maxVoltage, double acceleration) {
    // Assume max velocity is positive
    return (maxVoltage - m_ks - acceleration * m_ka) / m_kv;
  }

  /**
   * Calculates the minimum achievable velocity given a maximum voltage supply and an acceleration.
   * Useful for ensuring that velocity and acceleration constraints for a trapezoidal profile are
   * simultaneously achievable - enter the acceleration constraint, and this will give you a
   * simultaneously-achievable velocity constraint.
   *
   * @param maxVoltage The maximum voltage that can be supplied to the motor.
   * @param acceleration The acceleration of the motor.
   * @return The minimum possible velocity at the given acceleration.
   */
  public double minAchievableVelocity(double maxVoltage, double acceleration) {
    // Assume min velocity is negative, ks flips sign
    return (-maxVoltage + m_ks - acceleration * m_ka) / m_kv;
  }

  /**
   * Calculates the maximum achievable acceleration given a maximum voltage supply and a velocity.
   * Useful for ensuring that velocity and acceleration constraints for a trapezoidal profile are
   * simultaneously achievable - enter the velocity constraint, and this will give you a
   * simultaneously-achievable acceleration constraint.
   *
   * @param maxVoltage The maximum voltage that can be supplied to the motor.
   * @param velocity The velocity of the motor.
   * @return The maximum possible acceleration at the given velocity.
   */
  public double maxAchievableAcceleration(double maxVoltage, double velocity) {
    return (maxVoltage - m_ks * Math.signum(velocity) - velocity * m_kv) / m_ka;
  }

  /**
   * Calculates the maximum achievable acceleration given a maximum voltage supply and a velocity.
   * Useful for ensuring that velocity and acceleration constraints for a trapezoidal profile are
   * simultaneously achievable - enter the velocity constraint, and this will give you a
   * simultaneously-achievable acceleration constraint.
   *
   * @param maxVoltage The maximum voltage that can be supplied to the motor.
   * @param velocity The velocity of the motor.
   * @return The minimum possible acceleration at the given velocity.
   */
  public double minAchievableAcceleration(double maxVoltage, double velocity) {
    return maxAchievableAcceleration(-maxVoltage, velocity);
  }

  @Override
  public void initSendable(SendableBuilder builder) {
    builder.setSmartDashboardType("SimpleMotorFeedforward");
    builder.addDoubleProperty("kS", this::getKs, this::setKs);
    builder.addDoubleProperty("kV", this::getKv, this::setKv);
    builder.addDoubleProperty("kA", this::getKa, this::setKa);
    builder.addDoubleProperty("velocity", this::getVelocity, null);
    builder.addDoubleProperty("velocityOutput", () -> getVelocity() * m_kv, null);
    builder.addDoubleProperty("acceleration", this::getAcceleration, null);
    builder.addDoubleProperty("accelerationOutput", () -> getAcceleration() * m_ka, null);
    builder.addDoubleProperty("output", this::getOutput, null);
  }
}
