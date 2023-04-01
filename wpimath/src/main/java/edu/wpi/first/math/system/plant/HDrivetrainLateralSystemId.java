package edu.wpi.first.math.system.plant;

import edu.wpi.first.math.Matrix;
import edu.wpi.first.math.Nat;
import edu.wpi.first.math.numbers.N1;
import edu.wpi.first.math.system.LinearSystem;

public final class HDrivetrainLateralSystemId {
  public HDrivetrainLateralSystemId() {
    // Utility class
  }

  /**
   * Create a state-space model of a H-drive drivetrain. In this model,
   * the states are
   * [left velocity, right velocity, lateral velocity]ᵀ, inputs are [left voltage, right voltage, lateral voltage]ᵀ,
   * and outputs are
   * [left velocity, right velocity, lateral velocity]ᵀ.
   *
   * @param motor                           The motor (or gearbox) driving the
   *                                        drivetrain.
   * @param massKg                          The mass of the robot in kilograms.
   * @param rMeters                         The radius of the wheels in meters.
   * @param offsetFromCenterOfGravityMeters The distance of the lateral movement
   *                                        wheel from the center of gravity.
   * @param JKgMetersSquared                The moment of inertia of the robot.
   * @param G                               The gearing reduction as output over
   *                                        input.
   * @return A LinearSystem representing a differential drivetrain.
   * @throws IllegalArgumentException if m &lt;= 0, r &lt;= 0, ocfg &lt; 0, J
   *                                  &lt;= 0, or G &lt;= 0.
   */
  public static LinearSystem<N1, N1, N1> createHDrivetrainLateralVelocitySystem(
      DCMotor motor,
      double massKg,
      double rMeters,
      double offsetFromCenterOfGravityMeters,
      double JKgMetersSquared,
      double G) {
    if (massKg <= 0.0) {
      throw new IllegalArgumentException("massKg must be greater than zero.");
    }
    if (rMeters <= 0.0) {
      throw new IllegalArgumentException("rMeters must be greater than zero.");
    }
    if (offsetFromCenterOfGravityMeters < 0.0) {
      throw new IllegalArgumentException("offsetFromCenterOfGravityMeters must not be less than zero.");
    }
    if (JKgMetersSquared <= 0.0) {
      throw new IllegalArgumentException("JKgMetersSquared must be greater than zero.");
    }
    if (G <= 0.0) {
      throw new IllegalArgumentException("G must be greater than zero.");
    }

    var C1 = -(G * G) * motor.KtNMPerAmp / (motor.KvRadPerSecPerVolt * motor.rOhms * rMeters * rMeters);
    var C2 = G * motor.KtNMPerAmp / (motor.rOhms * rMeters);

    final double C3 = 1 / massKg + offsetFromCenterOfGravityMeters * offsetFromCenterOfGravityMeters / JKgMetersSquared;
    var A = Matrix.mat(Nat.N1(), Nat.N1()).fill(C3 * C1);
    var B = Matrix.mat(Nat.N1(), Nat.N1()).fill(C3 * C2);
    var C = Matrix.mat(Nat.N1(), Nat.N1()).fill(1.0);
    var D = Matrix.mat(Nat.N1(), Nat.N1()).fill(0.0);

    return new LinearSystem<>(A, B, C, D);
  }

  /**
   * Identify a H-drive lateral plant given the lateral plants's kV and kA in
   * both linear, This is used in combination with a regular Differential Drive
   * Velocity System to create an H-Drive
   * {(volts/(meter/sec), (volts/(meter/sec²))} and angular {(volts/(radian/sec)),
   * (volts/(radian/sec²))} cases. These constants can be found using SysId.
   *
   * <p>
   * States: [[lateral velocity]]<br>
   * Inputs: [[lateral voltage]]<br>
   * Outputs: [[lateral velocity]]
   *
   * @param kVLinear  The linear velocity gain in volts per (meters per second).
   * @param kALinear  The linear acceleration gain in volts per (meters per second
   *                  squared).
   * @param kVAngular The angular velocity gain in volts per (meters per second).
   * @param kAAngular The angular acceleration gain in volts per (meters per
   *                  second squared).
   * @return A LinearSystem representing the given characterized constants.
   * @throws IllegalArgumentException if kVLinear &lt;= 0, kALinear &lt;= 0,
   *                                  kVAngular &lt;= 0, or
   *                                  kAAngular &lt;= 0.
   * @see <a href=
   *      "https://github.com/wpilibsuite/sysid">https://github.com/wpilibsuite/sysid</a>
   */
  public static LinearSystem<N1, N1, N1> identifyLateralVelocitySystem(
      double kVLinear, double kALinear, double kVAngular, double kAAngular) {
    if (kVLinear <= 0.0) {
      throw new IllegalArgumentException("Kv,linear must be greater than zero.");
    }
    if (kALinear <= 0.0) {
      throw new IllegalArgumentException("Ka,linear must be greater than zero.");
    }
    if (kVAngular <= 0.0) {
      throw new IllegalArgumentException("Kv,angular must be greater than zero.");
    }
    if (kAAngular <= 0.0) {
      throw new IllegalArgumentException("Ka,angular must be greater than zero.");
    }

    final double A1 = 0.5 * -(kVLinear / kALinear + kVAngular / kAAngular);
    final double B1 = 0.5 * (1.0 / kALinear + 1.0 / kAAngular);

    var A = Matrix.mat(Nat.N1(), Nat.N1()).fill(A1);
    var B = Matrix.mat(Nat.N1(), Nat.N1()).fill(B1);
    var C = Matrix.mat(Nat.N1(), Nat.N1()).fill(1.0);
    var D = Matrix.mat(Nat.N1(), Nat.N1()).fill(0.0);

    return new LinearSystem<>(A, B, C, D);
  }
}
