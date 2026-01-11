// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.controller;

import static org.wpilib.math.autodiff.Variable.cos;
import static org.wpilib.math.autodiff.Variable.signum;

import org.wpilib.math.autodiff.Gradient;
import org.wpilib.math.autodiff.Hessian;
import org.wpilib.math.autodiff.NumericalIntegration;
import org.wpilib.math.autodiff.Variable;
import org.wpilib.math.autodiff.VariableMatrix;
import org.wpilib.math.autodiff.VariablePool;
import org.wpilib.math.controller.proto.ArmFeedforwardProto;
import org.wpilib.math.controller.struct.ArmFeedforwardStruct;
import org.wpilib.util.protobuf.ProtobufSerializable;
import org.wpilib.util.struct.StructSerializable;

/**
 * A helper class that computes feedforward outputs for a simple arm (modeled as a motor acting
 * against the force of gravity on a beam suspended at an angle).
 */
public class ArmFeedforward implements ProtobufSerializable, StructSerializable {
  /** The static gain, in volts. */
  private double ks;

  /** The gravity gain, in volts. */
  private double kg;

  /** The velocity gain, in V/(rad/s). */
  private double kv;

  /** The acceleration gain, in V/(rad/s²). */
  private double ka;

  /** The period, in seconds. */
  private final double m_dt;

  /**
   * Creates a new ArmFeedforward with the specified gains and period.
   *
   * @param ks The static gain in volts.
   * @param kg The gravity gain in volts.
   * @param kv The velocity gain in V/(rad/s).
   * @param ka The acceleration gain in V/(rad/s²).
   * @param dt The period in seconds.
   * @throws IllegalArgumentException for kv &lt; zero.
   * @throws IllegalArgumentException for ka &lt; zero.
   * @throws IllegalArgumentException for period &le; zero.
   */
  public ArmFeedforward(double ks, double kg, double kv, double ka, double dt) {
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
    if (dt <= 0.0) {
      throw new IllegalArgumentException("period must be a positive number, got " + dt + "!");
    }
    m_dt = dt;
  }

  /**
   * Creates a new ArmFeedforward with the specified gains. The period is defaulted to 20 ms.
   *
   * @param ks The static gain in volts.
   * @param kg The gravity gain in volts.
   * @param kv The velocity gain in V/(rad/s).
   * @param ka The acceleration gain in V/(rad/s²).
   * @throws IllegalArgumentException for kv &lt; zero.
   * @throws IllegalArgumentException for ka &lt; zero.
   */
  public ArmFeedforward(double ks, double kg, double kv, double ka) {
    this(ks, kg, kv, ka, 0.020);
  }

  /**
   * Creates a new ArmFeedforward with the specified gains. The period is defaulted to 20 ms.
   *
   * @param ks The static gain in volts.
   * @param kg The gravity gain in volts.
   * @param kv The velocity gain in V/(rad/s).
   * @throws IllegalArgumentException for kv &lt; zero.
   */
  public ArmFeedforward(double ks, double kg, double kv) {
    this(ks, kg, kv, 0);
  }

  /**
   * Sets the static gain.
   *
   * @param ks The static gain in volts.
   */
  public void setKs(double ks) {
    this.ks = ks;
  }

  /**
   * Sets the gravity gain.
   *
   * @param kg The gravity gain in volts.
   */
  public void setKg(double kg) {
    this.kg = kg;
  }

  /**
   * Sets the velocity gain.
   *
   * @param kv The velocity gain in V/(rad/s).
   */
  public void setKv(double kv) {
    this.kv = kv;
  }

  /**
   * Sets the acceleration gain.
   *
   * @param ka The acceleration gain in V/(rad/s²).
   */
  public void setKa(double ka) {
    this.ka = ka;
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
   * Returns the velocity gain in V/(rad/s).
   *
   * @return The velocity gain.
   */
  public double getKv() {
    return kv;
  }

  /**
   * Returns the acceleration gain in V/(rad/s²).
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
   * Calculates the feedforward from the gains and velocity setpoint assuming continuous control
   * (acceleration is assumed to be zero).
   *
   * @param position The position setpoint in radians. This angle should be measured from the
   *     horizontal (i.e. if the provided angle is 0, the arm should be parallel with the floor). If
   *     your encoder does not follow this convention, an offset should be added.
   * @param velocity The velocity setpoint in radians per second.
   * @return The computed feedforward.
   */
  public double calculate(double position, double velocity) {
    return ks * Math.signum(velocity) + kg * Math.cos(position) + kv * velocity;
  }

  /**
   * Calculates the feedforward from the gains and setpoints assuming continuous control.
   *
   * @param currentAngle The current angle in radians. This angle should be measured from the
   *     horizontal (i.e. if the provided angle is 0, the arm should be parallel to the floor). If
   *     your encoder does not follow this convention, an offset should be added.
   * @param currentVelocity The current velocity setpoint in radians per second.
   * @param nextVelocity The next velocity setpoint in radians per second.
   * @return The computed feedforward in volts.
   */
  public double calculate(double currentAngle, double currentVelocity, double nextVelocity) {
    // Small kₐ values make the solver ill-conditioned
    if (ka < 1e-1) {
      double acceleration = (nextVelocity - currentVelocity) / m_dt;
      return ks * Math.signum(currentVelocity)
          + kv * currentVelocity
          + ka * acceleration
          + kg * Math.cos(currentAngle);
    }

    try (var pool = new VariablePool()) {
      // Arm dynamics
      var A = new VariableMatrix(new double[][] {{0.0, 1.0}, {0.0, -kv / ka}});
      var B = new VariableMatrix(new double[][] {{0.0}, {1.0 / ka}});

      var r_k = new VariableMatrix(new double[][] {{currentAngle}, {currentVelocity}});

      var u_k = new Variable();

      // Initial guess
      double acceleration = (nextVelocity - currentVelocity) / m_dt;
      u_k.setValue(
          ks * Math.signum(currentVelocity)
              + kv * currentVelocity
              + ka * acceleration
              + kg * Math.cos(currentAngle));

      var r_k1 =
          NumericalIntegration.rk4(
              (VariableMatrix x, VariableMatrix u) -> {
                var c =
                    new VariableMatrix(
                        new Variable[][] {
                          {new Variable(0.0)},
                          {signum(x.get(1)).times(-ks / ka).plus(cos(x.get(0)).times(-kg / ka))}
                        });
                return A.times(x).plus(B.times(u)).plus(c);
              },
              r_k,
              new VariableMatrix(u_k),
              m_dt);

      // Minimize difference between desired and actual next velocity
      var cost =
          new Variable(nextVelocity)
              .minus(r_k1.get(1))
              .times(new Variable(nextVelocity).minus(r_k1.get(1)));

      // Refine solution via Newton's method
      {
        var xAD = u_k;
        double x = xAD.value();

        var gradientF = new Gradient(cost, xAD);
        var g = gradientF.value();

        var hessianF = new Hessian(cost, xAD);
        var H = hessianF.value();

        double error_k = Double.POSITIVE_INFINITY;
        double error_k1 = Math.abs(g.get(0, 0));

        // Loop until error stops decreasing or max iterations is reached
        for (int iteration = 0; iteration < 50 && error_k1 < (1.0 - 1e-10) * error_k; ++iteration) {
          error_k = error_k1;

          // Iterate via Newton's method.
          //
          //   xₖ₊₁ = xₖ − H⁻¹g
          //
          // The Hessian is regularized to at least 1e-4.
          double p_x = -g.get(0, 0) / Math.max(H.get(0, 0), 1e-4);

          // Shrink step until cost goes down
          {
            double oldCost = cost.value();

            double α = 1.0;
            double trial_x = x + α * p_x;

            xAD.setValue(trial_x);

            while (cost.value() > oldCost) {
              α *= 0.5;
              trial_x = x + α * p_x;

              xAD.setValue(trial_x);
            }

            x = trial_x;
          }

          xAD.setValue(x);

          g = gradientF.value();
          H = hessianF.value();

          error_k1 = Math.abs(g.get(0, 0));
        }

        hessianF.close();
        gradientF.close();
      }

      return u_k.value();
    }
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
   * @param angle The angle of the arm, in radians. This angle should be measured from the
   *     horizontal (i.e. if the provided angle is 0, the arm should be parallel with the floor). If
   *     your encoder does not follow this convention, an offset should be added.
   * @param acceleration The acceleration of the arm, in (rad/s²).
   * @return The maximum possible velocity in (rad/s) at the given acceleration and angle.
   */
  public double maxAchievableVelocity(double maxVoltage, double angle, double acceleration) {
    // Assume max velocity is positive
    return (maxVoltage - ks - Math.cos(angle) * kg - acceleration * ka) / kv;
  }

  /**
   * Calculates the minimum achievable velocity given a maximum voltage supply, a position, and an
   * acceleration. Useful for ensuring that velocity and acceleration constraints for a trapezoidal
   * profile are simultaneously achievable - enter the acceleration constraint, and this will give
   * you a simultaneously-achievable velocity constraint.
   *
   * @param maxVoltage The maximum voltage that can be supplied to the arm, in volts.
   * @param angle The angle of the arm, in radians. This angle should be measured from the
   *     horizontal (i.e. if the provided angle is 0, the arm should be parallel with the floor). If
   *     your encoder does not follow this convention, an offset should be added.
   * @param acceleration The acceleration of the arm, in (rad/s²).
   * @return The minimum possible velocity in (rad/s) at the given acceleration and angle.
   */
  public double minAchievableVelocity(double maxVoltage, double angle, double acceleration) {
    // Assume min velocity is negative, ks flips sign
    return (-maxVoltage + ks - Math.cos(angle) * kg - acceleration * ka) / kv;
  }

  /**
   * Calculates the maximum achievable acceleration given a maximum voltage supply, a position, and
   * a velocity. Useful for ensuring that velocity and acceleration constraints for a trapezoidal
   * profile are simultaneously achievable - enter the velocity constraint, and this will give you a
   * simultaneously-achievable acceleration constraint.
   *
   * @param maxVoltage The maximum voltage that can be supplied to the arm, in volts.
   * @param angle The angle of the arm, in radians. This angle should be measured from the
   *     horizontal (i.e. if the provided angle is 0, the arm should be parallel with the floor). If
   *     your encoder does not follow this convention, an offset should be added.
   * @param velocity The velocity of the elevator, in (rad/s)
   * @return The maximum possible acceleration in (rad/s²) at the given velocity.
   */
  public double maxAchievableAcceleration(double maxVoltage, double angle, double velocity) {
    return (maxVoltage - ks * Math.signum(velocity) - Math.cos(angle) * kg - velocity * kv) / ka;
  }

  /**
   * Calculates the minimum achievable acceleration given a maximum voltage supply, a position, and
   * a velocity. Useful for ensuring that velocity and acceleration constraints for a trapezoidal
   * profile are simultaneously achievable - enter the velocity constraint, and this will give you a
   * simultaneously-achievable acceleration constraint.
   *
   * @param maxVoltage The maximum voltage that can be supplied to the arm, in volts.
   * @param angle The angle of the arm, in radians. This angle should be measured from the
   *     horizontal (i.e. if the provided angle is 0, the arm should be parallel with the floor). If
   *     your encoder does not follow this convention, an offset should be added.
   * @param velocity The velocity of the elevator, in (rad/s)
   * @return The maximum possible acceleration in (rad/s²) at the given velocity.
   */
  public double minAchievableAcceleration(double maxVoltage, double angle, double velocity) {
    return maxAchievableAcceleration(-maxVoltage, angle, velocity);
  }

  /** Arm feedforward struct for serialization. */
  public static final ArmFeedforwardStruct struct = new ArmFeedforwardStruct();

  /** Arm feedforward protobuf for serialization. */
  public static final ArmFeedforwardProto proto = new ArmFeedforwardProto();
}
