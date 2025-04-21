// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.system.plant;

import edu.wpi.first.math.MatBuilder;
import edu.wpi.first.math.Matrix;
import edu.wpi.first.math.Nat;
import edu.wpi.first.math.VecBuilder;
import edu.wpi.first.math.numbers.N1;
import edu.wpi.first.math.numbers.N2;
import edu.wpi.first.math.system.LinearSystem;

/** Linear system ID utility functions. */
public final class LinearSystemId {
  private LinearSystemId() {
    // Utility class
  }

  /**
   * Create a state-space model of an elevator system. The states of the system are [position,
   * velocity]ᵀ, inputs are [voltage], and outputs are [position, velocity]ᵀ.
   *
   * @param motor The motor (or gearbox) attached to the carriage.
   * @param massKg The mass of the elevator carriage, in kilograms.
   * @param radiusMeters The radius of the elevator's driving drum, in meters.
   * @param gearing The reduction between motor and drum, as a ratio of output to input.
   * @return A LinearSystem representing the given characterized constants.
   * @throws IllegalArgumentException if massKg &lt;= 0, radiusMeters &lt;= 0, or gearing &lt;= 0.
   */
  public static LinearSystem<N2, N1, N2> createElevatorSystem(
      DCMotor motor, double massKg, double radiusMeters, double gearing) {
    if (massKg <= 0.0) {
      throw new IllegalArgumentException("massKg must be greater than zero.");
    }
    if (radiusMeters <= 0.0) {
      throw new IllegalArgumentException("radiusMeters must be greater than zero.");
    }
    if (gearing <= 0) {
      throw new IllegalArgumentException("gearing must be greater than zero.");
    }

    return new LinearSystem<>(
        MatBuilder.fill(
            Nat.N2(),
            Nat.N2(),
            0,
            1,
            0,
            -Math.pow(gearing, 2)
                * motor.KtNMPerAmp
                / (motor.rOhms * radiusMeters * radiusMeters * massKg * motor.KvRadPerSecPerVolt)),
        VecBuilder.fill(0, gearing * motor.KtNMPerAmp / (motor.rOhms * radiusMeters * massKg)),
        Matrix.eye(Nat.N2()),
        new Matrix<>(Nat.N2(), Nat.N1()));
  }

  /**
   * Create a state-space model of a flywheel system. The states of the system are [angular
   * velocity], inputs are [voltage], and outputs are [angular velocity].
   *
   * @param motor The motor (or gearbox) attached to the flywheel.
   * @param JKgMetersSquared The moment of inertia J of the flywheel.
   * @param gearing The reduction between motor and drum, as a ratio of output to input.
   * @return A LinearSystem representing the given characterized constants.
   * @throws IllegalArgumentException if JKgMetersSquared &lt;= 0 or gearing &lt;= 0.
   */
  public static LinearSystem<N1, N1, N1> createFlywheelSystem(
      DCMotor motor, double JKgMetersSquared, double gearing) {
    if (JKgMetersSquared <= 0.0) {
      throw new IllegalArgumentException("J must be greater than zero.");
    }
    if (gearing <= 0.0) {
      throw new IllegalArgumentException("gearing must be greater than zero.");
    }

    return new LinearSystem<>(
        VecBuilder.fill(
            -gearing
                * gearing
                * motor.KtNMPerAmp
                / (motor.KvRadPerSecPerVolt * motor.rOhms * JKgMetersSquared)),
        VecBuilder.fill(gearing * motor.KtNMPerAmp / (motor.rOhms * JKgMetersSquared)),
        Matrix.eye(Nat.N1()),
        new Matrix<>(Nat.N1(), Nat.N1()));
  }

  /**
   * Create a state-space model of a DC motor system. The states of the system are [angular
   * position, angular velocity]ᵀ, inputs are [voltage], and outputs are [angular position, angular
   * velocity]ᵀ.
   *
   * @param motor The motor (or gearbox) attached to system.
   * @param JKgMetersSquared The moment of inertia J of the DC motor.
   * @param gearing The reduction between motor and drum, as a ratio of output to input.
   * @return A LinearSystem representing the given characterized constants.
   * @throws IllegalArgumentException if JKgMetersSquared &lt;= 0 or gearing &lt;= 0.
   */
  public static LinearSystem<N2, N1, N2> createDCMotorSystem(
      DCMotor motor, double JKgMetersSquared, double gearing) {
    if (JKgMetersSquared <= 0.0) {
      throw new IllegalArgumentException("J must be greater than zero.");
    }
    if (gearing <= 0.0) {
      throw new IllegalArgumentException("gearing must be greater than zero.");
    }

    return new LinearSystem<>(
        MatBuilder.fill(
            Nat.N2(),
            Nat.N2(),
            0,
            1,
            0,
            -gearing
                * gearing
                * motor.KtNMPerAmp
                / (motor.KvRadPerSecPerVolt * motor.rOhms * JKgMetersSquared)),
        VecBuilder.fill(0, gearing * motor.KtNMPerAmp / (motor.rOhms * JKgMetersSquared)),
        Matrix.eye(Nat.N2()),
        new Matrix<>(Nat.N2(), Nat.N1()));
  }

  /**
   * Create a state-space model of a DC motor system from its kV (volts/(unit/sec)) and kA
   * (volts/(unit/sec²)). These constants can be found using SysId. the states of the system are
   * [position, velocity]ᵀ, inputs are [voltage], and outputs are [position].
   *
   * <p>The distance unit you choose MUST be an SI unit (i.e. meters or radians). You can use the
   * {@link edu.wpi.first.math.util.Units} class for converting between unit types.
   *
   * <p>The parameters provided by the user are from this feedforward model:
   *
   * <p>u = K_v v + K_a a
   *
   * @param kV The velocity gain, in volts/(unit/sec)
   * @param kA The acceleration gain, in volts/(unit/sec²)
   * @return A LinearSystem representing the given characterized constants.
   * @throws IllegalArgumentException if kV &lt; 0 or kA &lt;= 0.
   * @see <a href="https://github.com/wpilibsuite/sysid">https://github.com/wpilibsuite/sysid</a>
   */
  public static LinearSystem<N2, N1, N2> createDCMotorSystem(double kV, double kA) {
    if (kV < 0.0) {
      throw new IllegalArgumentException("Kv must be greater than or equal to zero.");
    }
    if (kA <= 0.0) {
      throw new IllegalArgumentException("Ka must be greater than zero.");
    }

    return new LinearSystem<>(
        MatBuilder.fill(Nat.N2(), Nat.N2(), 0, 1, 0, -kV / kA),
        VecBuilder.fill(0, 1 / kA),
        Matrix.eye(Nat.N2()),
        new Matrix<>(Nat.N2(), Nat.N1()));
  }

  /**
   * Create a state-space model of a differential drive drivetrain. In this model, the states are
   * [left velocity, right velocity]ᵀ, inputs are [left voltage, right voltage]ᵀ, and outputs are
   * [left velocity, right velocity]ᵀ.
   *
   * @param motor The motor (or gearbox) driving the drivetrain.
   * @param massKg The mass of the robot in kilograms.
   * @param rMeters The radius of the wheels in meters.
   * @param rbMeters The radius of the base (half the track width) in meters.
   * @param JKgMetersSquared The moment of inertia of the robot.
   * @param gearing The gearing reduction as output over input.
   * @return A LinearSystem representing a differential drivetrain.
   * @throws IllegalArgumentException if m &lt;= 0, r &lt;= 0, rb &lt;= 0, J &lt;= 0, or gearing
   *     &lt;= 0.
   */
  public static LinearSystem<N2, N2, N2> createDrivetrainVelocitySystem(
      DCMotor motor,
      double massKg,
      double rMeters,
      double rbMeters,
      double JKgMetersSquared,
      double gearing) {
    if (massKg <= 0.0) {
      throw new IllegalArgumentException("massKg must be greater than zero.");
    }
    if (rMeters <= 0.0) {
      throw new IllegalArgumentException("rMeters must be greater than zero.");
    }
    if (rbMeters <= 0.0) {
      throw new IllegalArgumentException("rbMeters must be greater than zero.");
    }
    if (JKgMetersSquared <= 0.0) {
      throw new IllegalArgumentException("JKgMetersSquared must be greater than zero.");
    }
    if (gearing <= 0.0) {
      throw new IllegalArgumentException("gearing must be greater than zero.");
    }

    var C1 =
        -(gearing * gearing)
            * motor.KtNMPerAmp
            / (motor.KvRadPerSecPerVolt * motor.rOhms * rMeters * rMeters);
    var C2 = gearing * motor.KtNMPerAmp / (motor.rOhms * rMeters);

    final double C3 = 1 / massKg + rbMeters * rbMeters / JKgMetersSquared;
    final double C4 = 1 / massKg - rbMeters * rbMeters / JKgMetersSquared;
    var A = MatBuilder.fill(Nat.N2(), Nat.N2(), C3 * C1, C4 * C1, C4 * C1, C3 * C1);
    var B = MatBuilder.fill(Nat.N2(), Nat.N2(), C3 * C2, C4 * C2, C4 * C2, C3 * C2);
    var C = MatBuilder.fill(Nat.N2(), Nat.N2(), 1.0, 0.0, 0.0, 1.0);
    var D = MatBuilder.fill(Nat.N2(), Nat.N2(), 0.0, 0.0, 0.0, 0.0);

    return new LinearSystem<>(A, B, C, D);
  }

  /**
   * Create a state-space model of a single jointed arm system. The states of the system are [angle,
   * angular velocity]ᵀ, inputs are [voltage], and outputs are [angle, angular velocity]ᵀ.
   *
   * @param motor The motor (or gearbox) attached to the arm.
   * @param JKgSquaredMeters The moment of inertia J of the arm.
   * @param gearing The gearing between the motor and arm, in output over input. Most of the time
   *     this will be greater than 1.
   * @return A LinearSystem representing the given characterized constants.
   */
  public static LinearSystem<N2, N1, N2> createSingleJointedArmSystem(
      DCMotor motor, double JKgSquaredMeters, double gearing) {
    if (JKgSquaredMeters <= 0.0) {
      throw new IllegalArgumentException("JKgSquaredMeters must be greater than zero.");
    }
    if (gearing <= 0.0) {
      throw new IllegalArgumentException("gearing must be greater than zero.");
    }

    return new LinearSystem<>(
        MatBuilder.fill(
            Nat.N2(),
            Nat.N2(),
            0,
            1,
            0,
            -Math.pow(gearing, 2)
                * motor.KtNMPerAmp
                / (motor.KvRadPerSecPerVolt * motor.rOhms * JKgSquaredMeters)),
        VecBuilder.fill(0, gearing * motor.KtNMPerAmp / (motor.rOhms * JKgSquaredMeters)),
        Matrix.eye(Nat.N2()),
        new Matrix<>(Nat.N2(), Nat.N1()));
  }

  /**
   * Create a state-space model for a 1 DOF velocity system from its kV (volts/(unit/sec)) and kA
   * (volts/(unit/sec²). These constants cam be found using SysId. The states of the system are
   * [velocity], inputs are [voltage], and outputs are [velocity].
   *
   * <p>The distance unit you choose MUST be an SI unit (i.e. meters or radians). You can use the
   * {@link edu.wpi.first.math.util.Units} class for converting between unit types.
   *
   * <p>The parameters provided by the user are from this feedforward model:
   *
   * <p>u = K_v v + K_a a
   *
   * @param kV The velocity gain, in volts/(unit/sec)
   * @param kA The acceleration gain, in volts/(unit/sec²)
   * @return A LinearSystem representing the given characterized constants.
   * @throws IllegalArgumentException if kV &lt; 0 or kA &lt;= 0.
   * @see <a href="https://github.com/wpilibsuite/sysid">https://github.com/wpilibsuite/sysid</a>
   */
  public static LinearSystem<N1, N1, N1> identifyVelocitySystem(double kV, double kA) {
    if (kV < 0.0) {
      throw new IllegalArgumentException("Kv must be greater than or equal to zero.");
    }
    if (kA <= 0.0) {
      throw new IllegalArgumentException("Ka must be greater than zero.");
    }

    return new LinearSystem<>(
        VecBuilder.fill(-kV / kA),
        VecBuilder.fill(1.0 / kA),
        VecBuilder.fill(1.0),
        VecBuilder.fill(0.0));
  }

  /**
   * Create a state-space model for a 1 DOF position system from its kV (volts/(unit/sec)) and kA
   * (volts/(unit/sec²). These constants cam be found using SysId. The states of the system are
   * [position, velocity]ᵀ, inputs are [voltage], and outputs are [position, velocity]ᵀ.
   *
   * <p>The distance unit you choose MUST be an SI unit (i.e. meters or radians). You can use the
   * {@link edu.wpi.first.math.util.Units} class for converting between unit types.
   *
   * <p>The parameters provided by the user are from this feedforward model:
   *
   * <p>u = K_v v + K_a a
   *
   * @param kV The velocity gain, in volts/(unit/sec)
   * @param kA The acceleration gain, in volts/(unit/sec²)
   * @return A LinearSystem representing the given characterized constants.
   * @throws IllegalArgumentException if kV &lt; 0 or kA &lt;= 0.
   * @see <a href="https://github.com/wpilibsuite/sysid">https://github.com/wpilibsuite/sysid</a>
   */
  public static LinearSystem<N2, N1, N2> identifyPositionSystem(double kV, double kA) {
    if (kV < 0.0) {
      throw new IllegalArgumentException("Kv must be greater than or equal to zero.");
    }
    if (kA <= 0.0) {
      throw new IllegalArgumentException("Ka must be greater than zero.");
    }

    return new LinearSystem<>(
        MatBuilder.fill(Nat.N2(), Nat.N2(), 0.0, 1.0, 0.0, -kV / kA),
        VecBuilder.fill(0.0, 1.0 / kA),
        Matrix.eye(Nat.N2()),
        new Matrix<>(Nat.N2(), Nat.N1()));
  }

  /**
   * Identify a differential drive drivetrain given the drivetrain's kV and kA in both linear
   * {(volts/(meter/sec), (volts/(meter/sec²))} and angular {(volts/(radian/sec)),
   * (volts/(radian/sec²))} cases. These constants can be found using SysId.
   *
   * <p>States: [[left velocity], [right velocity]]<br>
   * Inputs: [[left voltage], [right voltage]]<br>
   * Outputs: [[left velocity], [right velocity]]
   *
   * @param kVLinear The linear velocity gain in volts per (meters per second).
   * @param kALinear The linear acceleration gain in volts per (meters per second squared).
   * @param kVAngular The angular velocity gain in volts per (meters per second).
   * @param kAAngular The angular acceleration gain in volts per (meters per second squared).
   * @return A LinearSystem representing the given characterized constants.
   * @throws IllegalArgumentException if kVLinear &lt;= 0, kALinear &lt;= 0, kVAngular &lt;= 0, or
   *     kAAngular &lt;= 0.
   * @see <a href="https://github.com/wpilibsuite/sysid">https://github.com/wpilibsuite/sysid</a>
   */
  public static LinearSystem<N2, N2, N2> identifyDrivetrainSystem(
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
    final double A2 = 0.5 * -(kVLinear / kALinear - kVAngular / kAAngular);
    final double B1 = 0.5 * (1.0 / kALinear + 1.0 / kAAngular);
    final double B2 = 0.5 * (1.0 / kALinear - 1.0 / kAAngular);

    return new LinearSystem<>(
        MatBuilder.fill(Nat.N2(), Nat.N2(), A1, A2, A2, A1),
        MatBuilder.fill(Nat.N2(), Nat.N2(), B1, B2, B2, B1),
        MatBuilder.fill(Nat.N2(), Nat.N2(), 1, 0, 0, 1),
        MatBuilder.fill(Nat.N2(), Nat.N2(), 0, 0, 0, 0));
  }

  /**
   * Identify a differential drive drivetrain given the drivetrain's kV and kA in both linear
   * {(volts/(meter/sec)), (volts/(meter/sec²))} and angular {(volts/(radian/sec)),
   * (volts/(radian/sec²))} cases. This can be found using SysId.
   *
   * <p>States: [[left velocity], [right velocity]]<br>
   * Inputs: [[left voltage], [right voltage]]<br>
   * Outputs: [[left velocity], [right velocity]]
   *
   * @param kVLinear The linear velocity gain in volts per (meters per second).
   * @param kALinear The linear acceleration gain in volts per (meters per second squared).
   * @param kVAngular The angular velocity gain in volts per (radians per second).
   * @param kAAngular The angular acceleration gain in volts per (radians per second squared).
   * @param trackwidth The distance between the differential drive's left and right wheels, in
   *     meters.
   * @return A LinearSystem representing the given characterized constants.
   * @throws IllegalArgumentException if kVLinear &lt;= 0, kALinear &lt;= 0, kVAngular &lt;= 0,
   *     kAAngular &lt;= 0, or trackwidth &lt;= 0.
   * @see <a href="https://github.com/wpilibsuite/sysid">https://github.com/wpilibsuite/sysid</a>
   */
  public static LinearSystem<N2, N2, N2> identifyDrivetrainSystem(
      double kVLinear, double kALinear, double kVAngular, double kAAngular, double trackwidth) {
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
    if (trackwidth <= 0.0) {
      throw new IllegalArgumentException("trackwidth must be greater than zero.");
    }

    // We want to find a factor to include in Kv,angular that will convert
    // `u = Kv,angular omega` to `u = Kv,angular v`.
    //
    // v = omega r
    // omega = v/r
    // omega = 1/r v
    // omega = 1/(trackwidth/2) v
    // omega = 2/trackwidth v
    //
    // So multiplying by 2/trackwidth converts the angular gains from V/(rad/s)
    // to V/(m/s).
    return identifyDrivetrainSystem(
        kVLinear, kALinear, kVAngular * 2.0 / trackwidth, kAAngular * 2.0 / trackwidth);
  }
}
