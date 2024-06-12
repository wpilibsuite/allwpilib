// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.controller;

import edu.wpi.first.math.MatBuilder;
import edu.wpi.first.math.Matrix;
import edu.wpi.first.math.Nat;
import edu.wpi.first.math.numbers.N1;
import edu.wpi.first.math.system.LinearSystem;
import edu.wpi.first.math.system.plant.LinearSystemId;
import edu.wpi.first.units.Measure;
import edu.wpi.first.units.Unit;
import edu.wpi.first.units.Velocity;

/** A helper class that computes feedforward outputs for a simple permanent-magnet DC motor. */
public class SimpleMotorFeedforward {
  /** The static gain. */
  public final double ks;

  /** The velocity gain. */
  public final double kv;

  /** The acceleration gain. */
  public final double ka;

  /** The plant. */
  private final LinearSystem<N1, N1, N1> plant;

  /** The feedforward. */
  private final LinearPlantInversionFeedforward<N1, N1, N1> feedforward;

  /** The current reference. */
  private final Matrix<N1, N1> r;

  /** The next reference. */
  private Matrix<N1, N1> nextR;

  /**
   * Creates a new SimpleMotorFeedforward with the specified gains and period. Units of the gain
   * values will dictate units of the computed feedforward.
   *
   * @param ks The static gain.
   * @param kv The velocity gain.
   * @param ka The acceleration gain.
   * @param periodSeconds The period in seconds.
   * @throws IllegalArgumentException for kv &lt; zero.
   * @throws IllegalArgumentException for ka &lt; zero.
   * @throws IllegalArgumentException for period &le; zero.
   */
  public SimpleMotorFeedforward(double ks, double kv, double ka, double periodSeconds) {
    this.ks = ks;
    this.kv = kv;
    this.ka = ka;
    if (kv < 0.0) {
      throw new IllegalArgumentException("kv must be a non-negative number, got " + kv + "!");
    }
    if (ka < 0.0) {
      throw new IllegalArgumentException("ka must be a non-negative number, got " + ka + "!");
    }
    if (periodSeconds <= 0.0) {
      throw new IllegalArgumentException(
          "period must be a positive number, got " + periodSeconds + "!");
    }
    if (ka != 0.0) {
      this.plant = LinearSystemId.identifyVelocitySystem(this.kv, this.ka);
      this.feedforward = new LinearPlantInversionFeedforward<>(plant, periodSeconds);

      this.r = MatBuilder.fill(Nat.N1(), Nat.N1(), 0.0);
      this.nextR = MatBuilder.fill(Nat.N1(), Nat.N1(), 0.0);
    } else {
      this.plant = null;
      this.feedforward = null;
      this.r = null;
      this.nextR = null;
    }
  }

  /**
   * Creates a new SimpleMotorFeedforward with the specified gains and period. The period is
   * defaulted to 20 ms. Units of the gain values will dictate units of the computed feedforward.
   *
   * @param ks The static gain.
   * @param kv The velocity gain.
   * @param ka The acceleration gain.
   * @throws IllegalArgumentException for kv &lt; zero.
   * @throws IllegalArgumentException for ka &lt; zero.
   */
  public SimpleMotorFeedforward(double ks, double kv, double ka) {
    this(ks, kv, ka, 0.020);
  }

  /**
   * Creates a new SimpleMotorFeedforward with the specified gains. Acceleration gain is defaulted
   * to zero. The period is defaulted to 20 ms. Units of the gain values will dictate units of the
   * computed feedforward.
   *
   * @param ks The static gain.
   * @param kv The velocity gain.
   */
  public SimpleMotorFeedforward(double ks, double kv) {
    this(ks, kv, 0, 0.020);
  }

  /**
   * Calculates the feedforward from the gains and setpoints.
   *
   * @param velocity The velocity setpoint.
   * @param acceleration The acceleration setpoint.
   * @return The computed feedforward.
   */
  @SuppressWarnings("removal")
  @Deprecated(forRemoval = true, since = "2025")
  public double calculate(double velocity, double acceleration) {
    return ks * Math.signum(velocity) + kv * velocity + ka * acceleration;
  }

  /**
   * Calculates the feedforward from the gains and velocity setpoint (acceleration is assumed to be
   * zero).
   *
   * @param velocity The velocity setpoint.
   * @return The computed feedforward.
   */
  @SuppressWarnings("removal")
  @Deprecated(forRemoval = true, since = "2025")
  public double calculate(double velocity) {
    return calculate(velocity, 0);
  }

  /**
   * Calculates the feedforward from the gains and setpoints assuming discrete control.
   *
   * @param <U> The velocity parameter either as distance or angle.
   * @param currentVelocity The current velocity setpoint.
   * @param nextVelocity The next velocity setpoint.
   * @return The computed feedforward.
   */
  public <U extends Unit<U>> double calculate(
      Measure<Velocity<U>> currentVelocity, Measure<Velocity<U>> nextVelocity) {
    if (ka == 0.0) {
      return ks * Math.signum(nextVelocity.magnitude()) + kv * nextVelocity.magnitude();
    // Given the following discrete feedforward model
    //   uₖ = B_d⁺(rₖ₊₁ − A_d rₖ)

    // where
    //   A_d = eᴬᵀ
    //   B_d = A⁻¹(eᴬᵀ - I)B
    //   A = −kᵥ/kₐ
    //   B = 1/kₐ

    // We want the feedforward model when kₐ = 0.    
    // Simplify A.
    //   A = −kᵥ/kₐ
    // As kₐ approaches zero, A approaches -∞.
    //   A = −∞

    // Simplify A_d.
    //   A_d = eᴬᵀ
    //   A_d = exp(−∞)
    //   A_d = 0

    // Simplify B_d.
    //   B_d = A⁻¹(eᴬᵀ - I)B
    //   B_d = A⁻¹((0) - I)B
    //   B_d = A⁻¹(-I)B
    //   B_d = -A⁻¹B
    //   B_d = -(−kᵥ/kₐ)⁻¹(1/kₐ)
    //   B_d = (kᵥ/kₐ)⁻¹(1/kₐ)
    //   B_d = kₐ/kᵥ(1/kₐ)
    //   B_d = 1/kᵥ

    // Substitute these into the feedforward equation.
    //   uₖ = B_d⁺(rₖ₊₁ − A_d rₖ)
    //   uₖ = (1/kᵥ)⁺(rₖ₊₁ − (0) rₖ)
    //   uₖ = kᵥrₖ₊₁
    } else {
      r.set(0, 0, currentVelocity.magnitude());
      nextR.set(0, 0, nextVelocity.magnitude());

      return ks * Math.signum(currentVelocity.magnitude())
          + feedforward.calculate(r, nextR).get(0, 0);
    }
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
    return (maxVoltage - ks - acceleration * ka) / kv;
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
    return (-maxVoltage + ks - acceleration * ka) / kv;
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
    return (maxVoltage - ks * Math.signum(velocity) - velocity * kv) / ka;
  }

  /**
   * Calculates the minimum achievable acceleration given a maximum voltage supply and a velocity.
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
}
