// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.controller;

import edu.wpi.first.math.controller.proto.ElevatorFeedforwardProto;
import edu.wpi.first.math.controller.struct.ElevatorFeedforwardStruct;
import edu.wpi.first.util.protobuf.ProtobufSerializable;
import edu.wpi.first.util.struct.StructSerializable;

/**
 * A helper class that computes feedforward outputs for a simple elevator (modeled as a motor acting
 * against the force of gravity).
 */
public class ElevatorFeedforward implements ProtobufSerializable, StructSerializable {
  /** The static gain, in volts. */
  private final double ks;

  /** The gravity gain, in volts. */
  private final double kg;

  /** The velocity gain, in V/(m/s). */
  private final double kv;

  /** The acceleration gain, in V/(m/s²). */
  private final double ka;

  /** The period, in seconds. */
  private final double m_dt;

  /**
   * Creates a new ElevatorFeedforward with the specified gains and period.
   *
   * @param ks The static gain in volts.
   * @param kg The gravity gain in volts.
   * @param kv The velocity gain in V/(m/s).
   * @param ka The acceleration gain in V/(m/s²).
   * @param dtSeconds The period in seconds.
   * @throws IllegalArgumentException for kv &lt; zero.
   * @throws IllegalArgumentException for ka &lt; zero.
   * @throws IllegalArgumentException for period &le; zero.
   */
  public ElevatorFeedforward(double ks, double kg, double kv, double ka, double dtSeconds) {
    this.ks = ks;
    this.kg = kg;
    this.kv = kv;
    this.ka = ka;
    if (kv < 0.0) {
      throw new IllegalArgumentException("kv must be a non-negative number, got " + kv + "!");
    }
    if (ka < 0.0) {
      throw new IllegalArgumentException("ka must be a non-negative number, got " + ka + "!");
    }
    if (dtSeconds <= 0.0) {
      throw new IllegalArgumentException(
          "period must be a positive number, got " + dtSeconds + "!");
    }
    m_dt = dtSeconds;
  }

  /**
   * Creates a new ElevatorFeedforward with the specified gains. The period is defaulted to 20 ms.
   *
   * @param ks The static gain in volts.
   * @param kg The gravity gain in volts.
   * @param kv The velocity gain in V/(m/s).
   * @param ka The acceleration gain in V/(m/s²).
   * @throws IllegalArgumentException for kv &lt; zero.
   * @throws IllegalArgumentException for ka &lt; zero.
   */
  public ElevatorFeedforward(double ks, double kg, double kv, double ka) {
    this(ks, kg, kv, ka, 0.020);
  }

  /**
   * Creates a new ElevatorFeedforward with the specified gains. Acceleration gain is defaulted to
   * zero. The period is defaulted to 20 ms.
   *
   * @param ks The static gain in volts.
   * @param kg The gravity gain in volts.
   * @param kv The velocity gain in V/(m/s).
   * @throws IllegalArgumentException for kv &lt; zero.
   */
  public ElevatorFeedforward(double ks, double kg, double kv) {
    this(ks, kg, kv, 0);
  }

  /**
   * Returns the static gain in volts.
   *
   * @return The static gain in volts.
   */
  public double getKs() {
    return ks;
  }

  /**
   * Returns the gravity gain in volts.
   *
   * @return The gravity gain in volts.
   */
  public double getKg() {
    return kg;
  }

  /**
   * Returns the velocity gain in V/(m/s).
   *
   * @return The velocity gain.
   */
  public double getKv() {
    return kv;
  }

  /**
   * Returns the acceleration gain in V/(m/s²).
   *
   * @return The acceleration gain.
   */
  public double getKa() {
    return ka;
  }

  /**
   * Returns the period in seconds.
   *
   * @return The period in seconds.
   */
  public double getDt() {
    return m_dt;
  }

  /**
   * Calculates the feedforward from the gains and setpoints assuming continuous control.
   *
   * @param velocity The velocity setpoint.
   * @param acceleration The acceleration setpoint.
   * @return The computed feedforward.
   */
  @SuppressWarnings("removal")
  @Deprecated(forRemoval = true, since = "2025")
  public double calculate(double velocity, double acceleration) {
    return ks * Math.signum(velocity) + kg + kv * velocity + ka * acceleration;
  }

  /**
   * Calculates the feedforward from the gains and velocity setpoint assuming continuous control
   * (acceleration is assumed to be zero).
   *
   * @param velocity The velocity setpoint.
   * @return The computed feedforward.
   */
  public double calculate(double velocity) {
    return calculate(velocity, 0);
  }

  /**
   * Calculates the feedforward from the gains and setpoints assuming discrete control.
   *
   * <p>Note this method is inaccurate when the velocity crosses 0.
   *
   * @param currentVelocity The current velocity setpoint in meters per second.
   * @param nextVelocity The next velocity setpoint in meters per second.
   * @return The computed feedforward in volts.
   */
  public double calculateWithVelocities(double currentVelocity, double nextVelocity) {
    // See wpimath/algorithms.md#Elevator_feedforward for derivation
    if (ka == 0.0) {
      return ks * Math.signum(nextVelocity) + kg + kv * nextVelocity;
    } else {
      double A = -kv / ka;
      double B = 1.0 / ka;
      double A_d = Math.exp(A * m_dt);
      double B_d = 1.0 / A * (A_d - 1.0) * B;
      return kg
          + ks * Math.signum(currentVelocity)
          + 1.0 / B_d * (nextVelocity - A_d * currentVelocity);
    }
  }

  // Rearranging the main equation from the calculate() method yields the
  // formulas for the methods below:

  /**
   * Calculates the maximum achievable velocity given a maximum voltage supply and an acceleration.
   * Useful for ensuring that velocity and acceleration constraints for a trapezoidal profile are
   * simultaneously achievable - enter the acceleration constraint, and this will give you a
   * simultaneously-achievable velocity constraint.
   *
   * @param maxVoltage The maximum voltage that can be supplied to the elevator, in volts.
   * @param acceleration The acceleration of the elevator, in (m/s²).
   * @return The maximum possible velocity in (m/s) at the given acceleration.
   */
  public double maxAchievableVelocity(double maxVoltage, double acceleration) {
    // Assume max velocity is positive
    return (maxVoltage - ks - kg - acceleration * ka) / kv;
  }

  /**
   * Calculates the minimum achievable velocity given a maximum voltage supply and an acceleration.
   * Useful for ensuring that velocity and acceleration constraints for a trapezoidal profile are
   * simultaneously achievable - enter the acceleration constraint, and this will give you a
   * simultaneously-achievable velocity constraint.
   *
   * @param maxVoltage The maximum voltage that can be supplied to the elevator, in volts.
   * @param acceleration The acceleration of the elevator, in (m/s²).
   * @return The maximum possible velocity in (m/s) at the given acceleration.
   */
  public double minAchievableVelocity(double maxVoltage, double acceleration) {
    // Assume min velocity is negative, ks flips sign
    return (-maxVoltage + ks - kg - acceleration * ka) / kv;
  }

  /**
   * Calculates the maximum achievable acceleration given a maximum voltage supply and a velocity.
   * Useful for ensuring that velocity and acceleration constraints for a trapezoidal profile are
   * simultaneously achievable - enter the velocity constraint, and this will give you a
   * simultaneously-achievable acceleration constraint.
   *
   * @param maxVoltage The maximum voltage that can be supplied to the elevator, in volts.
   * @param velocity The velocity of the elevator, in (m/s)
   * @return The maximum possible acceleration in (m/s²) at the given velocity.
   */
  public double maxAchievableAcceleration(double maxVoltage, double velocity) {
    return (maxVoltage - ks * Math.signum(velocity) - kg - velocity * kv) / ka;
  }

  /**
   * Calculates the minimum achievable acceleration given a maximum voltage supply and a velocity.
   * Useful for ensuring that velocity and acceleration constraints for a trapezoidal profile are
   * simultaneously achievable - enter the velocity constraint, and this will give you a
   * simultaneously-achievable acceleration constraint.
   *
   * @param maxVoltage The maximum voltage that can be supplied to the elevator, in volts.
   * @param velocity The velocity of the elevator, in (m/s)
   * @return The maximum possible acceleration in (m/s²) at the given velocity.
   */
  public double minAchievableAcceleration(double maxVoltage, double velocity) {
    return maxAchievableAcceleration(-maxVoltage, velocity);
  }

  /** ElevatorFeedforward struct for serialization. */
  public static final ElevatorFeedforwardStruct struct = new ElevatorFeedforwardStruct();

  /** ElevatorFeedforward protobuf for serialization. */
  public static final ElevatorFeedforwardProto proto = new ElevatorFeedforwardProto();
}
