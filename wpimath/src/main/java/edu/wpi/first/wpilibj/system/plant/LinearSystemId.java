// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.system.plant;

import edu.wpi.first.wpilibj.system.LinearSystem;
import edu.wpi.first.wpiutil.math.Matrix;
import edu.wpi.first.wpiutil.math.Nat;
import edu.wpi.first.wpiutil.math.VecBuilder;
import edu.wpi.first.wpiutil.math.numbers.N1;
import edu.wpi.first.wpiutil.math.numbers.N2;

public final class LinearSystemId {
  private LinearSystemId() {
    // Utility class
  }

  /**
   * Create a state-space model of an elevator system. The states of the system are [position,
   * velocity]^T, inputs are [voltage], and outputs are [position].
   *
   * @param motor The motor (or gearbox) attached to the arm.
   * @param massKg The mass of the elevator carriage, in kilograms.
   * @param radiusMeters The radius of thd driving drum of the elevator, in meters.
   * @param G The reduction between motor and drum, as a ratio of output to input.
   * @return A LinearSystem representing the given characterized constants.
   */
  @SuppressWarnings("ParameterName")
  public static LinearSystem<N2, N1, N1> createElevatorSystem(
      DCMotor motor, double massKg, double radiusMeters, double G) {
    return new LinearSystem<>(
        Matrix.mat(Nat.N2(), Nat.N2())
            .fill(
                0,
                1,
                0,
                -Math.pow(G, 2)
                    * motor.KtNMPerAmp
                    / (motor.rOhms
                        * radiusMeters
                        * radiusMeters
                        * massKg
                        * motor.KvRadPerSecPerVolt)),
        VecBuilder.fill(0, G * motor.KtNMPerAmp / (motor.rOhms * radiusMeters * massKg)),
        Matrix.mat(Nat.N1(), Nat.N2()).fill(1, 0),
        new Matrix<>(Nat.N1(), Nat.N1()));
  }

  /**
   * Create a state-space model of a flywheel system. The states of the system are [angular
   * velocity], inputs are [voltage], and outputs are [angular velocity].
   *
   * @param motor The motor (or gearbox) attached to the arm.
   * @param jKgMetersSquared The moment of inertia J of the flywheel.
   * @param G The reduction between motor and drum, as a ratio of output to input.
   * @return A LinearSystem representing the given characterized constants.
   */
  @SuppressWarnings("ParameterName")
  public static LinearSystem<N1, N1, N1> createFlywheelSystem(
      DCMotor motor, double jKgMetersSquared, double G) {
    return new LinearSystem<>(
        VecBuilder.fill(
            -G
                * G
                * motor.KtNMPerAmp
                / (motor.KvRadPerSecPerVolt * motor.rOhms * jKgMetersSquared)),
        VecBuilder.fill(G * motor.KtNMPerAmp / (motor.rOhms * jKgMetersSquared)),
        Matrix.eye(Nat.N1()),
        new Matrix<>(Nat.N1(), Nat.N1()));
  }

  /**
   * Create a state-space model of a differential drive drivetrain. In this model, the states are
   * [v_left, v_right]^T, inputs are [V_left, V_right]^T and outputs are [v_left, v_right]^T.
   *
   * @param motor the gearbox representing the motors driving the drivetrain.
   * @param massKg the mass of the robot.
   * @param rMeters the radius of the wheels in meters.
   * @param rbMeters the radius of the base (half the track width) in meters.
   * @param JKgMetersSquared the moment of inertia of the robot.
   * @param G the gearing reduction as output over input.
   * @return A LinearSystem representing a differential drivetrain.
   */
  @SuppressWarnings({"LocalVariableName", "ParameterName"})
  public static LinearSystem<N2, N2, N2> createDrivetrainVelocitySystem(
      DCMotor motor,
      double massKg,
      double rMeters,
      double rbMeters,
      double JKgMetersSquared,
      double G) {
    var C1 =
        -(G * G) * motor.KtNMPerAmp / (motor.KvRadPerSecPerVolt * motor.rOhms * rMeters * rMeters);
    var C2 = G * motor.KtNMPerAmp / (motor.rOhms * rMeters);

    final double C3 = 1 / massKg + rbMeters * rbMeters / JKgMetersSquared;
    final double C4 = 1 / massKg - rbMeters * rbMeters / JKgMetersSquared;
    var A = Matrix.mat(Nat.N2(), Nat.N2()).fill(C3 * C1, C4 * C1, C4 * C1, C3 * C1);
    var B = Matrix.mat(Nat.N2(), Nat.N2()).fill(C3 * C2, C4 * C2, C4 * C2, C3 * C2);
    var C = Matrix.mat(Nat.N2(), Nat.N2()).fill(1.0, 0.0, 0.0, 1.0);
    var D = Matrix.mat(Nat.N2(), Nat.N2()).fill(0.0, 0.0, 0.0, 0.0);

    return new LinearSystem<>(A, B, C, D);
  }

  /**
   * Create a state-space model of a single jointed arm system. The states of the system are [angle,
   * angular velocity], inputs are [voltage], and outputs are [angle].
   *
   * @param motor The motor (or gearbox) attached to the arm.
   * @param jKgSquaredMeters The moment of inertia J of the arm.
   * @param G The gearing between the motor and arm, in output over input. Most of the time this
   *     will be greater than 1.
   * @return A LinearSystem representing the given characterized constants.
   */
  @SuppressWarnings("ParameterName")
  public static LinearSystem<N2, N1, N1> createSingleJointedArmSystem(
      DCMotor motor, double jKgSquaredMeters, double G) {
    return new LinearSystem<>(
        Matrix.mat(Nat.N2(), Nat.N2())
            .fill(
                0,
                1,
                0,
                -Math.pow(G, 2)
                    * motor.KtNMPerAmp
                    / (motor.KvRadPerSecPerVolt * motor.rOhms * jKgSquaredMeters)),
        VecBuilder.fill(0, G * motor.KtNMPerAmp / (motor.rOhms * jKgSquaredMeters)),
        Matrix.mat(Nat.N1(), Nat.N2()).fill(1, 0),
        new Matrix<>(Nat.N1(), Nat.N1()));
  }

  /**
   * Identify a velocity system from it's kV (volts/(unit/sec)) and kA (volts/(unit/sec^2). These
   * constants cam be found using frc-characterization. The states of the system are [velocity],
   * inputs are [voltage], and outputs are [velocity].
   *
   * <p>The distance unit you choose MUST be an SI unit (i.e. meters or radians). You can use the
   * {@link edu.wpi.first.wpilibj.util.Units} class for converting between unit types.
   *
   * @param kV The velocity gain, in volts per (units per second)
   * @param kA The acceleration gain, in volts per (units per second squared)
   * @return A LinearSystem representing the given characterized constants.
   * @see <a href="https://github.com/wpilibsuite/frc-characterization">
   *     https://github.com/wpilibsuite/frc-characterization</a>
   */
  @SuppressWarnings("ParameterName")
  public static LinearSystem<N1, N1, N1> identifyVelocitySystem(double kV, double kA) {
    return new LinearSystem<>(
        VecBuilder.fill(-kV / kA),
        VecBuilder.fill(1.0 / kA),
        VecBuilder.fill(1.0),
        VecBuilder.fill(0.0));
  }

  /**
   * Identify a position system from it's kV (volts/(unit/sec)) and kA (volts/(unit/sec^2). These
   * constants cam be found using frc-characterization. The states of the system are [position,
   * velocity]^T, inputs are [voltage], and outputs are [position].
   *
   * <p>The distance unit you choose MUST be an SI unit (i.e. meters or radians). You can use the
   * {@link edu.wpi.first.wpilibj.util.Units} class for converting between unit types.
   *
   * @param kV The velocity gain, in volts per (units per second)
   * @param kA The acceleration gain, in volts per (units per second squared)
   * @return A LinearSystem representing the given characterized constants.
   * @see <a href="https://github.com/wpilibsuite/frc-characterization">
   *     https://github.com/wpilibsuite/frc-characterization</a>
   */
  @SuppressWarnings("ParameterName")
  public static LinearSystem<N2, N1, N1> identifyPositionSystem(double kV, double kA) {
    return new LinearSystem<>(
        Matrix.mat(Nat.N2(), Nat.N2()).fill(0.0, 1.0, 0.0, -kV / kA),
        VecBuilder.fill(0.0, 1.0 / kA),
        Matrix.mat(Nat.N1(), Nat.N2()).fill(1.0, 0.0),
        VecBuilder.fill(0.0));
  }

  /**
   * Identify a standard differential drive drivetrain, given the drivetrain's kV and kA in both
   * linear (volts/(meter/sec) and volts/(meter/sec^2)) and angular (volts/(radian/sec) and
   * volts/(radian/sec^2)) cases. This can be found using frc-characterization. The states of the
   * system are [left velocity, right velocity]^T, inputs are [left voltage, right voltage]^T, and
   * outputs are [left velocity, right velocity]^T.
   *
   * @param kVLinear The linear velocity gain, volts per (meter per second).
   * @param kALinear The linear acceleration gain, volts per (meter per second squared).
   * @param kVAngular The angular velocity gain, volts per (radians per second).
   * @param kAAngular The angular acceleration gain, volts per (radians per second squared).
   * @return A LinearSystem representing the given characterized constants.
   * @see <a href="https://github.com/wpilibsuite/frc-characterization">
   *     https://github.com/wpilibsuite/frc-characterization</a>
   */
  @SuppressWarnings("ParameterName")
  public static LinearSystem<N2, N2, N2> identifyDrivetrainSystem(
      double kVLinear, double kALinear, double kVAngular, double kAAngular) {

    final double c = 0.5 / (kALinear * kAAngular);
    final double A1 = c * (-kALinear * kVAngular - kVLinear * kAAngular);
    final double A2 = c * (kALinear * kVAngular - kVLinear * kAAngular);
    final double B1 = c * (kALinear + kAAngular);
    final double B2 = c * (kAAngular - kALinear);

    return new LinearSystem<>(
        Matrix.mat(Nat.N2(), Nat.N2()).fill(A1, A2, A2, A1),
        Matrix.mat(Nat.N2(), Nat.N2()).fill(B1, B2, B2, B1),
        Matrix.mat(Nat.N2(), Nat.N2()).fill(1, 0, 0, 1),
        Matrix.mat(Nat.N2(), Nat.N2()).fill(0, 0, 0, 0));
  }
}
