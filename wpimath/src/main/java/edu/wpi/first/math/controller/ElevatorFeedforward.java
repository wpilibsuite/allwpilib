// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.controller;

import edu.wpi.first.math.MatBuilder;
import edu.wpi.first.math.Nat;
import edu.wpi.first.math.controller.proto.ElevatorFeedforwardProto;
import edu.wpi.first.math.controller.struct.ElevatorFeedforwardStruct;
import edu.wpi.first.math.system.plant.LinearSystemId;
import edu.wpi.first.util.protobuf.ProtobufSerializable;
import edu.wpi.first.util.struct.StructSerializable;

/**
 * A helper class that computes feedforward outputs for a simple elevator (modeled as a motor acting
 * against the force of gravity).
 */
public class ElevatorFeedforward implements ProtobufSerializable, StructSerializable {
  /** The static gain. */
  public final double ks;

  /** The gravity gain. */
  public final double kg;

  /** The velocity gain. */
  public final double kv;

  /** The acceleration gain. */
  public final double ka;

  /** ElevatorFeedforward protobuf for serialization. */
  public static final ElevatorFeedforwardProto proto = new ElevatorFeedforwardProto();

  /** ElevatorFeedforward struct for serialization. */
  public static final ElevatorFeedforwardStruct struct = new ElevatorFeedforwardStruct();

  /**
   * Creates a new ElevatorFeedforward with the specified gains. Units of the gain values will
   * dictate units of the computed feedforward.
   *
   * @param ks The static gain.
   * @param kg The gravity gain.
   * @param kv The velocity gain.
   * @param ka The acceleration gain.
   * @throws IllegalArgumentException for kv &lt; zero.
   * @throws IllegalArgumentException for ka &lt; zero.
   */
  public ElevatorFeedforward(double ks, double kg, double kv, double ka) {
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
   * Calculates the feedforward from the gains and setpoints.
   *
   * @param velocity The velocity setpoint.
   * @param acceleration The acceleration setpoint.
   * @return The computed feedforward.
   */
  public double calculate(double velocity, double acceleration) {
    return ks * Math.signum(velocity) + kg + kv * velocity + ka * acceleration;
  }

  /**
   * Calculates the feedforward from the gains and setpoints.
   *
   * <p>Note this method is inaccurate when the velocity crosses 0.
   *
   * @param currentVelocity The current velocity setpoint.
   * @param nextVelocity The next velocity setpoint.
   * @param dtSeconds Time between velocity setpoints in seconds.
   * @return The computed feedforward.
   */
  public double calculate(double currentVelocity, double nextVelocity, double dtSeconds) {
    // Discretize the affine model.
    //
    //   dx/dt = Ax + Bu + c
    //   dx/dt = Ax + B(u + B⁺c)
    //   xₖ₊₁ = eᴬᵀxₖ + A⁻¹(eᴬᵀ - I)B(uₖ + B⁺cₖ)
    //   xₖ₊₁ = A_d xₖ + B_d (uₖ + B⁺cₖ)
    //   xₖ₊₁ = A_d xₖ + B_duₖ + B_d B⁺cₖ
    //
    // Solve for uₖ.
    //
    //   B_duₖ = xₖ₊₁ − A_d xₖ − B_d B⁺cₖ
    //   uₖ = B_d⁺(xₖ₊₁ − A_d xₖ − B_d B⁺cₖ)
    //   uₖ = B_d⁺(xₖ₊₁ − A_d xₖ) − B⁺cₖ
    //
    // For an elevator with the model
    // dx/dt = -Kv/Ka x + 1/Ka u - Kg/Ka - Ks/Ka sgn(x),
    // A = -Kv/Ka, B = 1/Ka, and c = -(Kg/Ka + Ks/Ka sgn(x)). Substitute in B
    // assuming sgn(x) is a constant for the duration of the step.
    //
    //   uₖ = B_d⁺(xₖ₊₁ − A_d xₖ) − Ka(-(Kg/Ka + Ks/Ka sgn(x)))
    //   uₖ = B_d⁺(xₖ₊₁ − A_d xₖ) + Ka(Kg/Ka + Ks/Ka sgn(x))
    //   uₖ = B_d⁺(xₖ₊₁ − A_d xₖ) + Kg + Ks sgn(x)
    var plant = LinearSystemId.identifyVelocitySystem(this.kv, this.ka);
    var feedforward = new LinearPlantInversionFeedforward<>(plant, dtSeconds);

    var r = MatBuilder.fill(Nat.N1(), Nat.N1(), currentVelocity);
    var nextR = MatBuilder.fill(Nat.N1(), Nat.N1(), nextVelocity);

    return kg + ks * Math.signum(currentVelocity) + feedforward.calculate(r, nextR).get(0, 0);
  }

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
    return (maxVoltage - ks - kg - acceleration * ka) / kv;
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
    return (-maxVoltage + ks - kg - acceleration * ka) / kv;
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
    return (maxVoltage - ks * Math.signum(velocity) - kg - velocity * kv) / ka;
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
}
