// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.system;

import org.wpilib.math.linalg.MatBuilder;
import org.wpilib.math.numbers.N1;
import org.wpilib.math.numbers.N2;
import org.wpilib.math.util.Nat;

/** Linear system factories. */
public final class Models {
  private Models() {
    // Utility class
  }

  /**
   * Creates a flywheel state-space model from physical constants.
   *
   * <p>The states are [angular velocity], the inputs are [voltage], and the outputs are [angular
   * velocity].
   *
   * @param motor The motor (or gearbox) attached to the flywheel.
   * @param J The moment of inertia J of the flywheel.
   * @param gearing Gear ratio from motor to flywheel (greater than 1 is a reduction).
   * @return Flywheel state-space model.
   * @throws IllegalArgumentException if J &lt;= 0 or gearing &lt;= 0.
   */
  public static LinearSystem<N1, N1, N1> flywheelFromPhysicalConstants(
      DCMotor motor, double J, double gearing) {
    if (J <= 0.0) {
      throw new IllegalArgumentException("J must be greater than zero.");
    }
    if (gearing <= 0.0) {
      throw new IllegalArgumentException("gearing must be greater than zero.");
    }

    var A =
        MatBuilder.fill(
            Nat.N1(), Nat.N1(), -Math.pow(gearing, 2) * motor.Kt / (motor.Kv * motor.R * J));
    var B = MatBuilder.fill(Nat.N1(), Nat.N1(), gearing * motor.Kt / (motor.R * J));
    var C = MatBuilder.fill(Nat.N1(), Nat.N1(), 1.0);
    var D = MatBuilder.fill(Nat.N1(), Nat.N1(), 0.0);

    return new LinearSystem<>(A, B, C, D);
  }

  /**
   * Creates a flywheel state-space model from SysId constants kᵥ (V/(rad/s)) and kₐ (V/(rad/s²))
   * from the feedforward model u = kᵥv + kₐa.
   *
   * <p>The states are [velocity], the inputs are [voltage], and the outputs are [velocity].
   *
   * @param kV The velocity gain, in V/(rad/s).
   * @param kA The acceleration gain, in V/(rad/s²).
   * @return Flywheel state-space model.
   * @throws IllegalArgumentException if kV &lt; 0 or kA &lt;= 0.
   * @see <a
   *     href="https://github.com/wpilibsuite/allwpilib/tree/main/sysid">https://github.com/wpilibsuite/allwpilib/tree/main/sysid</a>
   */
  public static LinearSystem<N1, N1, N1> flywheelFromSysId(double kV, double kA) {
    if (kV < 0.0) {
      throw new IllegalArgumentException("Kv must be greater than or equal to zero.");
    }
    if (kA <= 0.0) {
      throw new IllegalArgumentException("Ka must be greater than zero.");
    }

    var A = MatBuilder.fill(Nat.N1(), Nat.N1(), -kV / kA);
    var B = MatBuilder.fill(Nat.N1(), Nat.N1(), 1.0 / kA);
    var C = MatBuilder.fill(Nat.N1(), Nat.N1(), 1.0);
    var D = MatBuilder.fill(Nat.N1(), Nat.N1(), 0.0);

    return new LinearSystem<>(A, B, C, D);
  }

  /**
   * Creates an elevator state-space model from physical constants.
   *
   * <p>The states are [position, velocity], the inputs are [voltage], and the outputs are
   * [position, velocity].
   *
   * @param motor The motor (or gearbox) attached to the carriage.
   * @param mass The mass of the elevator carriage, in kilograms.
   * @param radius The radius of the elevator's driving drum, in meters.
   * @param gearing Gear ratio from motor to carriage (greater than 1 is a reduction).
   * @return Elevator state-space model.
   * @throws IllegalArgumentException if mass &lt;= 0, radius &lt;= 0, or gearing &lt;= 0.
   */
  public static LinearSystem<N2, N1, N2> elevatorFromPhysicalConstants(
      DCMotor motor, double mass, double radius, double gearing) {
    if (mass <= 0.0) {
      throw new IllegalArgumentException("mass must be greater than zero.");
    }
    if (radius <= 0.0) {
      throw new IllegalArgumentException("radius must be greater than zero.");
    }
    if (gearing <= 0.0) {
      throw new IllegalArgumentException("gearing must be greater than zero.");
    }

    var A =
        MatBuilder.fill(
            Nat.N2(),
            Nat.N2(),
            0.0,
            1.0,
            0.0,
            -Math.pow(gearing, 2) * motor.Kt / (motor.R * Math.pow(radius, 2) * mass * motor.Kv));
    var B =
        MatBuilder.fill(Nat.N2(), Nat.N1(), 0.0, gearing * motor.Kt / (motor.R * radius * mass));
    var C = MatBuilder.fill(Nat.N2(), Nat.N2(), 1.0, 0.0, 0.0, 1.0);
    var D = MatBuilder.fill(Nat.N2(), Nat.N1(), 0.0, 0.0);

    return new LinearSystem<>(A, B, C, D);
  }

  /**
   * Creates an elevator state-space model from SysId constants kᵥ (V/(m/s)) and kₐ (V/(m/s²)) from
   * the feedforward model u = kᵥv + kₐa.
   *
   * <p>The states are [position, velocity], the inputs are [voltage], and the outputs are
   * [position, velocity].
   *
   * @param kV The velocity gain, in V/(m/s).
   * @param kA The acceleration gain, in V/(m/s²).
   * @return Elevator state-space model.
   * @throws IllegalArgumentException if kV &lt; 0 or kA &lt;= 0.
   * @see <a
   *     href="https://github.com/wpilibsuite/allwpilib/tree/main/sysid">https://github.com/wpilibsuite/allwpilib/tree/main/sysid</a>
   */
  public static LinearSystem<N2, N1, N2> elevatorFromSysId(double kV, double kA) {
    if (kV < 0.0) {
      throw new IllegalArgumentException("Kv must be greater than or equal to zero.");
    }
    if (kA <= 0.0) {
      throw new IllegalArgumentException("Ka must be greater than zero.");
    }

    var A = MatBuilder.fill(Nat.N2(), Nat.N2(), 0.0, 1.0, 0.0, -kV / kA);
    var B = MatBuilder.fill(Nat.N2(), Nat.N1(), 0.0, 1.0 / kA);
    var C = MatBuilder.fill(Nat.N2(), Nat.N2(), 1.0, 0.0, 0.0, 1.0);
    var D = MatBuilder.fill(Nat.N2(), Nat.N1(), 0.0, 0.0);

    return new LinearSystem<>(A, B, C, D);
  }

  /**
   * Create a single-jointed arm state-space model from physical constants.
   *
   * <p>The states are [angle, angular velocity], the inputs are [voltage], and the outputs are
   * [angle, angular velocity].
   *
   * @param motor The motor (or gearbox) attached to the arm.
   * @param J The moment of inertia J of the arm.
   * @param gearing Gear ratio from motor to arm (greater than 1 is a reduction).
   * @return Single-jointed arm state-space model.
   * @throws IllegalArgumentException if J &lt;= 0 or gearing &lt;= 0.
   */
  public static LinearSystem<N2, N1, N2> singleJointedArmFromPhysicalConstants(
      DCMotor motor, double J, double gearing) {
    if (J <= 0.0) {
      throw new IllegalArgumentException("J must be greater than zero.");
    }
    if (gearing <= 0.0) {
      throw new IllegalArgumentException("gearing must be greater than zero.");
    }

    var A =
        MatBuilder.fill(
            Nat.N2(),
            Nat.N2(),
            0.0,
            1.0,
            0.0,
            -Math.pow(gearing, 2) * motor.Kt / (motor.Kv * motor.R * J));
    var B = MatBuilder.fill(Nat.N2(), Nat.N1(), 0.0, gearing * motor.Kt / (motor.R * J));
    var C = MatBuilder.fill(Nat.N2(), Nat.N2(), 1.0, 0.0, 0.0, 1.0);
    var D = MatBuilder.fill(Nat.N2(), Nat.N1(), 0.0, 0.0);

    return new LinearSystem<>(A, B, C, D);
  }

  /**
   * Creates a single-jointed arm state-space model from SysId constants kᵥ (V/(rad/s)) and kₐ
   * (V/(rad/s²)) from the feedforward model u = kᵥv + kₐa.
   *
   * <p>The states are [position, velocity], the inputs are [voltage], and the outputs are
   * [position, velocity].
   *
   * @param kV The velocity gain, in volts/(unit/sec).
   * @param kA The acceleration gain, in volts/(unit/sec²).
   * @return Single-jointed arm state-space model.
   * @throws IllegalArgumentException if kV &lt; 0 or kA &lt;= 0.
   * @see <a
   *     href="https://github.com/wpilibsuite/allwpilib/tree/main/sysid">https://github.com/wpilibsuite/allwpilib/tree/main/sysid</a>
   */
  public static LinearSystem<N2, N1, N2> singleJointedArmFromSysId(double kV, double kA) {
    if (kV < 0.0) {
      throw new IllegalArgumentException("Kv must be greater than or equal to zero.");
    }
    if (kA <= 0.0) {
      throw new IllegalArgumentException("Ka must be greater than zero.");
    }

    var A = MatBuilder.fill(Nat.N2(), Nat.N2(), 0.0, 1.0, 0.0, -kV / kA);
    var B = MatBuilder.fill(Nat.N2(), Nat.N1(), 0.0, 1.0 / kA);
    var C = MatBuilder.fill(Nat.N2(), Nat.N2(), 1.0, 0.0, 0.0, 1.0);
    var D = MatBuilder.fill(Nat.N2(), Nat.N1(), 0.0, 0.0);

    return new LinearSystem<>(A, B, C, D);
  }

  /**
   * Creates a differential drive state-space model from physical constants.
   *
   * <p>The states are [left velocity, right velocity], the inputs are [left voltage, right
   * voltage], and the outputs are [left velocity, right velocity].
   *
   * @param motor The motor (or gearbox) driving the drivetrain.
   * @param mass The mass of the robot in kilograms.
   * @param r The radius of the wheels in meters.
   * @param rb The radius of the base (half of the trackwidth), in meters.
   * @param J The moment of inertia of the robot.
   * @param gearing Gear ratio from motor to wheel (greater than 1 is a reduction).
   * @return Differential drive state-space model.
   * @throws IllegalArgumentException if mass &lt;= 0, r &lt;= 0, rb &lt;= 0, J &lt;= 0, or gearing
   *     &lt;= 0.
   */
  public static LinearSystem<N2, N2, N2> differentialDriveFromPhysicalConstants(
      DCMotor motor, double mass, double r, double rb, double J, double gearing) {
    if (mass <= 0.0) {
      throw new IllegalArgumentException("mass must be greater than zero.");
    }
    if (r <= 0.0) {
      throw new IllegalArgumentException("r must be greater than zero.");
    }
    if (rb <= 0.0) {
      throw new IllegalArgumentException("rb must be greater than zero.");
    }
    if (J <= 0.0) {
      throw new IllegalArgumentException("J must be greater than zero.");
    }
    if (gearing <= 0.0) {
      throw new IllegalArgumentException("gearing must be greater than zero.");
    }

    double C1 = -Math.pow(gearing, 2) * motor.Kt / (motor.Kv * motor.R * Math.pow(r, 2));
    double C2 = gearing * motor.Kt / (motor.R * r);

    var A =
        MatBuilder.fill(
            Nat.N2(),
            Nat.N2(),
            (1 / mass + Math.pow(rb, 2) / J) * C1,
            (1 / mass - Math.pow(rb, 2) / J) * C1,
            (1 / mass - Math.pow(rb, 2) / J) * C1,
            (1 / mass + Math.pow(rb, 2) / J) * C1);
    var B =
        MatBuilder.fill(
            Nat.N2(),
            Nat.N2(),
            (1 / mass + Math.pow(rb, 2) / J) * C2,
            (1 / mass - Math.pow(rb, 2) / J) * C2,
            (1 / mass - Math.pow(rb, 2) / J) * C2,
            (1 / mass + Math.pow(rb, 2) / J) * C2);
    var C = MatBuilder.fill(Nat.N2(), Nat.N2(), 1.0, 0.0, 0.0, 1.0);
    var D = MatBuilder.fill(Nat.N2(), Nat.N2(), 0.0, 0.0, 0.0, 0.0);

    return new LinearSystem<>(A, B, C, D);
  }

  /**
   * Creates a differential drive state-space model from SysId constants kᵥ and kₐ in both linear
   * {(V/(m/s), (V/(m/s²))} and angular {(V/(rad/s), (V/(rad/s²))} cases.
   *
   * <p>The states are [left velocity, right velocity], the inputs are [left voltage, right
   * voltage], and the outputs are [left velocity, right velocity].
   *
   * @param kVLinear The linear velocity gain in volts per (meters per second).
   * @param kALinear The linear acceleration gain in volts per (meters per second squared).
   * @param kVAngular The angular velocity gain in volts per (meters per second).
   * @param kAAngular The angular acceleration gain in volts per (meters per second squared).
   * @return Differential drive state-space model.
   * @throws IllegalArgumentException if kVLinear &lt;= 0, kALinear &lt;= 0, kVAngular &lt;= 0, or
   *     kAAngular &lt;= 0.
   * @see <a
   *     href="https://github.com/wpilibsuite/allwpilib/tree/main/sysid">https://github.com/wpilibsuite/allwpilib/tree/main/sysid</a>
   */
  public static LinearSystem<N2, N2, N2> differentialDriveFromSysId(
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

    double A1 = -0.5 * (kVLinear / kALinear + kVAngular / kAAngular);
    double A2 = -0.5 * (kVLinear / kALinear - kVAngular / kAAngular);
    double B1 = 0.5 / kALinear + 0.5 / kAAngular;
    double B2 = 0.5 / kALinear - 0.5 / kAAngular;

    var A = MatBuilder.fill(Nat.N2(), Nat.N2(), A1, A2, A2, A1);
    var B = MatBuilder.fill(Nat.N2(), Nat.N2(), B1, B2, B2, B1);
    var C = MatBuilder.fill(Nat.N2(), Nat.N2(), 1.0, 0.0, 0.0, 1.0);
    var D = MatBuilder.fill(Nat.N2(), Nat.N2(), 0.0, 0.0, 0.0, 0.0);

    return new LinearSystem<>(A, B, C, D);
  }

  /**
   * Creates a differential drive state-space model from SysId constants kᵥ and kₐ in both linear
   * {(V/(m/s), (V/(m/s²))} and angular {(V/(rad/s), (V/(rad/s²))} cases.
   *
   * <p>The states are [left velocity, right velocity], the inputs are [left voltage, right
   * voltage], and the outputs are [left velocity, right velocity].
   *
   * @param kVLinear The linear velocity gain in volts per (meters per second).
   * @param kALinear The linear acceleration gain in volts per (meters per second squared).
   * @param kVAngular The angular velocity gain in volts per (radians per second).
   * @param kAAngular The angular acceleration gain in volts per (radians per second squared).
   * @param trackwidth The distance between the differential drive's left and right wheels, in
   *     meters.
   * @return Differential drive state-space model.
   * @throws IllegalArgumentException if kVLinear &lt;= 0, kALinear &lt;= 0, kVAngular &lt;= 0,
   *     kAAngular &lt;= 0, or trackwidth &lt;= 0.
   * @see <a
   *     href="https://github.com/wpilibsuite/allwpilib/tree/main/sysid">https://github.com/wpilibsuite/allwpilib/tree/main/sysid</a>
   */
  public static LinearSystem<N2, N2, N2> differentialDriveFromSysId(
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
      throw new IllegalArgumentException("r must be greater than zero.");
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
    return differentialDriveFromSysId(
        kVLinear, kALinear, kVAngular * 2.0 / trackwidth, kAAngular * 2.0 / trackwidth);
  }
}
