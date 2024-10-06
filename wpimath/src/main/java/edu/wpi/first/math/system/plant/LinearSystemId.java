// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.system.plant;

import static edu.wpi.first.units.Units.KilogramSquareMeters;
import static edu.wpi.first.units.Units.Kilograms;
import static edu.wpi.first.units.Units.Meters;

import edu.wpi.first.math.MatBuilder;
import edu.wpi.first.math.Matrix;
import edu.wpi.first.math.Nat;
import edu.wpi.first.math.VecBuilder;
import edu.wpi.first.math.numbers.N1;
import edu.wpi.first.math.numbers.N2;
import edu.wpi.first.math.system.LinearSystem;
import edu.wpi.first.units.Measure;
import edu.wpi.first.units.PerUnit;
import edu.wpi.first.units.Unit;
import edu.wpi.first.units.measure.Distance;
import edu.wpi.first.units.measure.Mass;
import edu.wpi.first.units.measure.MomentOfInertia;

/** Linear system ID utility functions. */
public final class LinearSystemId {
  private LinearSystemId() {
    // Utility class
  }

  /**
   * Create a state-space model of an elevator system. The states of the system are [position,
   * velocity]ᵀ, inputs are [voltage], and outputs are [position].
   *
   * @param motor The motor (or gearbox) attached to the carriage.
   * @param massKg The mass of the elevator carriage, in kilograms.
   * @param drumRadiusMeters The radius of the elevator's driving drum, in meters.
   * @param gearing The reduction between motor and drum, as a ratio of output to input.
   * @return A LinearSystem representing the given characterized constants.
   * @throws IllegalArgumentException if massKg &lt;= 0, drumRadiusMeters &lt;= 0, or gearing &lt;=
   *     0.
   */
  public static LinearSystem<N2, N1, N2> createElevatorSystem(
      DCMotor motor, double massKg, double drumRadiusMeters, double gearing) {
    if (massKg <= 0.0) {
      throw new IllegalArgumentException("massKg must be greater than zero.");
    }
    if (drumRadiusMeters <= 0.0) {
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
                / (motor.rOhms
                    * drumRadiusMeters
                    * drumRadiusMeters
                    * massKg
                    * motor.KvRadPerSecPerVolt)),
        VecBuilder.fill(0, gearing * motor.KtNMPerAmp / (motor.rOhms * drumRadiusMeters * massKg)),
        Matrix.eye(Nat.N2()),
        new Matrix<>(Nat.N2(), Nat.N1()));
  }

  /**
   * Create a state-space model of an elevator system. The states of the system are [position,
   * velocity]ᵀ, inputs are [voltage], and outputs are [position].
   *
   * @param motor The motor (or gearbox) attached to the carriage.
   * @param mass The mass of the elevator carriage.
   * @param drumRadius The radius of the elevator's driving drum.
   * @param gearing The reduction between motor and drum, as a ratio of output to input.
   * @return A LinearSystem representing the given characterized constants.
   * @throws IllegalArgumentException if mass &lt;= 0, drumRadius &lt;= 0, or gearing &lt;= 0.
   */
  public static LinearSystem<N2, N1, N2> createElevatorSystem(
      DCMotor motor, Mass mass, Distance drumRadius, double gearing) {
    return createElevatorSystem(motor, mass.in(Kilograms), drumRadius.in(Meters), gearing);
  }

  /**
   * Create a state-space model of an elevator system. The states of the system are [position,
   * velocity]ᵀ, inputs are [torque], and outputs are [position].
   *
   * @param massKg The mass of the elevator carriage, in kilograms.
   * @param drumRadiusMeters The radius of the elevator's driving drum, in meters.
   * @return A LinearSystem representing the given characterized constants.
   * @throws IllegalArgumentException if massKg &lt;= 0, drumRadiusMeters &lt;= 0.
   */
  public static LinearSystem<N2, N1, N2> createElevatorTorqueSystem(
      double massKg, double drumRadiusMeters) {
    if (massKg <= 0.0) {
      throw new IllegalArgumentException("massKg must be greater than zero.");
    }
    if (drumRadiusMeters <= 0.0) {
      throw new IllegalArgumentException("drumRadiusMeters must be greater than zero.");
    }

    return new LinearSystem<>(
        Matrix.eye(Nat.N2()),
        VecBuilder.fill(0, 1.0 / (massKg * drumRadiusMeters)),
        Matrix.eye(Nat.N2()),
        new Matrix<>(Nat.N2(), Nat.N1()));
  }

  /**
   * Create a state-space model of an elevator system. The states of the system are [position,
   * velocity]ᵀ, inputs are [torque], and outputs are [position].
   *
   * @param mass The mass of the elevator carriage.
   * @param drumRadius The radius of the elevator's driving drum.
   * @return A LinearSystem representing the given characterized constants.
   * @throws IllegalArgumentException if mass &lt;= 0, drumRadius &lt;= 0.
   */
  public static LinearSystem<N2, N1, N2> createElevatorTorqueSystem(
      Mass mass, Distance drumRadius) {
    return createElevatorTorqueSystem(mass.in(Kilograms), drumRadius.in(Meters));
  }

  /**
   * Create a state-space model of a flywheel system. The states of the system are [angular
   * velocity], inputs are [voltage], and outputs are [angular velocity].
   *
   * @param gearbox The gearbox attached to the flywheel.
   * @param JKgMetersSquared The moment of inertia J of the flywheel.
   * @return A LinearSystem representing the given characterized constants.
   * @throws IllegalArgumentException if JKgMetersSquared &leq; 0.
   */
  public static LinearSystem<N1, N1, N1> createFlywheelSystem(
      Gearbox gearbox, double JKgMetersSquared) {
    if (JKgMetersSquared <= 0.0) {
      throw new IllegalArgumentException("J must be greater than zero.");
    }

    return new LinearSystem<>(
        VecBuilder.fill(
            -Math.pow(gearbox.reduction, 2)
                * gearbox.numMotors
                * gearbox.motorType.KtNMPerAmp
                / (gearbox.motorType.KvRadPerSecPerVolt
                    * gearbox.motorType.rOhms
                    * JKgMetersSquared)),
        VecBuilder.fill(
            gearbox.numMotors
                * gearbox.reduction
                * gearbox.motorType.KtNMPerAmp
                / (gearbox.motorType.rOhms * JKgMetersSquared)),
        Matrix.eye(Nat.N1()),
        new Matrix<>(Nat.N1(), Nat.N1()));
  }

  /**
   * Create a state-space model of a flywheel system. The states of the system are [angular
   * velocity], inputs are [voltage], and outputs are [angular velocity].
   *
   * @param gearbox The gearbox attached to the flywheel.
   * @param J The moment of inertia J of the flywheel.
   * @return A LinearSystem representing the given characterized constants.
   * @throws IllegalArgumentException if J &leq; 0.
   */
  public static LinearSystem<N1, N1, N1> createFlywheelSystem(Gearbox gearbox, MomentOfInertia J) {
    return createFlywheelSystem(gearbox, J.in(KilogramSquareMeters));
  }

  /**
   * Create a state-space model of a flywheel system. The states of the system are [angular
   * velocity], inputs are [torque], and outputs are [angular velocity].
   *
   * @param JKgMetersSquared The moment of inertia J of the flywheel.
   * @return A LinearSystem representing the given characterized constants.
   * @throws IllegalArgumentException if JKgMetersSquared &lt;= 0.
   */
  public static LinearSystem<N1, N1, N1> createFlywheelTorqueSystem(double JKgMetersSquared) {
    if (JKgMetersSquared <= 0.0) {
      throw new IllegalArgumentException("J must be greater than zero.");
    }

    return new LinearSystem<>(
        VecBuilder.fill(0),
        VecBuilder.fill(1.0 / JKgMetersSquared),
        Matrix.eye(Nat.N1()),
        new Matrix<>(Nat.N1(), Nat.N1()));
  }

  /**
   * Create a state-space model of a flywheel system. The states of the system are [angular
   * velocity], inputs are [torque], and outputs are [angular velocity].
   *
   * @param J The moment of inertia J of the flywheel.
   * @return A LinearSystem representing the given characterized constants.
   * @throws IllegalArgumentException if J &lt;= 0.
   */
  public static LinearSystem<N1, N1, N1> createFlywheelTorqueSystem(MomentOfInertia J) {
    return createFlywheelTorqueSystem(J.in(KilogramSquareMeters));
  }

  /**
   * Create a state-space model of a DC motor system. The states of the system are [angular
   * position, angular velocity], inputs are [voltage], and outputs are [angular position, angular
   * velocity].
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
   * Create a state-space model of a DC motor system. The states of the system are [angular
   * position, angular velocity], inputs are [voltage], and outputs are [angular position, angular
   * velocity].
   *
   * @param motor The motor (or gearbox) attached to system.
   * @param J The moment of inertia J of the DC motor.
   * @param gearing The reduction between motor and drum, as a ratio of output to input.
   * @return A LinearSystem representing the given characterized constants.
   * @throws IllegalArgumentException if J &lt;= 0 or gearing &lt;= 0.
   */
  public static LinearSystem<N2, N1, N2> createDCMotorSystem(
      DCMotor motor, MomentOfInertia J, double gearing) {
    return createDCMotorSystem(motor, J.in(KilogramSquareMeters), gearing);
  }

  /**
   * Create a state-space model of a DC motor system from its kV (volts/(unit/sec)) and kA
   * (volts/(unit/sec²)). These constants can be found using SysId. the states of the system are
   * [position, velocity], inputs are [voltage], and outputs are [position].
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
   * @deprecated Use identifyPositionSystem instead.
   * @see <a href= "https://github.com/wpilibsuite/sysid">https://github.com/wpilibsuite/sysid</a>
   */
  @Deprecated
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
   * Create a state-space model of a DC motor system. The states of the system are [angular
   * position, angular velocity], inputs are [torque], and outputs are [angular position, angular
   * velocity].
   *
   * @param JKgMetersSquared The moment of inertia J of the DC motor.
   * @return A LinearSystem representing the given characterized constants.
   * @throws IllegalArgumentException if JKgMetersSquared &lt;= 0.
   */
  public static LinearSystem<N2, N1, N2> createDCMotorTorqueSystem(double JKgMetersSquared) {
    if (JKgMetersSquared <= 0.0) {
      throw new IllegalArgumentException("J must be greater than zero.");
    }

    return new LinearSystem<>(
        Matrix.eye(Nat.N2()),
        VecBuilder.fill(0, 1.0 / JKgMetersSquared),
        Matrix.eye(Nat.N2()),
        new Matrix<>(Nat.N2(), Nat.N1()));
  }

  /**
   * Create a state-space model of a DC motor system. The states of the system are [angular
   * position, angular velocity], inputs are [torque], and outputs are [angular position, angular
   * velocity].
   *
   * @param J The moment of inertia J of the DC motor.
   * @return A LinearSystem representing the given characterized constants.
   * @throws IllegalArgumentException if J &lt;= 0.
   */
  public static LinearSystem<N2, N1, N2> createDCMotorTorqueSystem(MomentOfInertia J) {
    return createDCMotorTorqueSystem(J.in(KilogramSquareMeters));
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
   * angular velocity], inputs are [voltage], and outputs are [angle].
   *
   * @param motor The motor (or gearbox) attached to the arm.
   * @param JKgSquaredMeters The moment of inertia J of the arm.
   * @param gearing The gearing between the motor and arm, in output over input. Most of the time
   *     this will be greater than 1.
   * @return A LinearSystem representing the given characterized constants.
   * @throws IllegalArgumentException if JKgMetersSquared &lt;= 0 or gearing &lt;= 0.
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
   * Create a state-space model of a single jointed arm system. The states of the system are [angle,
   * angular velocity], inputs are [voltage], and outputs are [angle].
   *
   * @param motor The motor (or gearbox) attached to the arm.
   * @param J The moment of inertia J of the arm.
   * @param gearing The gearing between the motor and arm, in output over input. Most of the time
   *     this will be greater than 1.
   * @return A LinearSystem representing the given characterized constants.
   * @throws IllegalArgumentException if J &lt;= 0 or gearing &lt;= 0.
   */
  public static LinearSystem<N2, N1, N2> createSingleJointedArmSystem(
      DCMotor motor, MomentOfInertia J, double gearing) {
    return createSingleJointedArmSystem(motor, J.in(KilogramSquareMeters), gearing);
  }

  /**
   * Create a state-space model of a single jointed arm system. The states of the system are [angle,
   * angular velocity], inputs are [torque], and outputs are [angle].
   *
   * @param JKgSquaredMeters The moment of inertia J of the arm.
   * @return A LinearSystem representing the given characterized constants.
   * @throws IllegalArgumentException if JKgMetersSquared &lt;= 0.
   */
  public static LinearSystem<N2, N1, N2> createSingleJointedArmTorqueSystem(
      double JKgSquaredMeters) {
    if (JKgSquaredMeters <= 0.0) {
      throw new IllegalArgumentException("JKgSquaredMeters must be greater than zero.");
    }

    return new LinearSystem<>(
        Matrix.eye(Nat.N2()),
        VecBuilder.fill(0, 1.0 / JKgSquaredMeters),
        Matrix.eye(Nat.N2()),
        new Matrix<>(Nat.N2(), Nat.N1()));
  }

  /**
   * Create a state-space model of a single jointed arm system. The states of the system are [angle,
   * angular velocity], inputs are [torque], and outputs are [angle].
   *
   * @param J The moment of inertia J of the arm.
   * @return A LinearSystem representing the given characterized constants.
   * @throws IllegalArgumentException if J &lt;= 0.
   */
  public static LinearSystem<N2, N1, N2> createSingleJointedArmTorqueSystem(MomentOfInertia J) {
    return createSingleJointedArmTorqueSystem(J.in(KilogramSquareMeters));
  }

  /**
   * Create a state-space model of a wheel system. The states of the system are [linear position,
   * linear velocity], inputs are [voltage], and outputs are [linear position, linear velocity].
   *
   * @param motor The motor (or gearbox) of the system.
   * @param massKg The mass of the system, in kilograms.
   * @param radiusMeters The radius of the system , in meters.
   * @param gearing The reduction between motor and output, as a ratio of output to input.
   * @return A LinearSystem representing the given characterized constants.
   * @throws IllegalArgumentException if massKg &lt;= 0, radiusMeters &lt;= 0, or gearing &lt;= 0.
   */
  public static LinearSystem<N2, N1, N2> createWheelSystem(
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
   * Create a state-space model of a wheel system. The states of the system are [linear position,
   * linear velocity], inputs are [voltage], and outputs are [linear position, linear velocity].
   *
   * @param motor The motor (or gearbox) of the system.
   * @param mass The mass of the system.
   * @param radius The radius of the system.
   * @param gearing The reduction between motor and output, as a ratio of output to input.
   * @return A LinearSystem representing the given characterized constants.
   * @throws IllegalArgumentException if mass &lt;= 0, radius &lt;= 0, or gearing &lt;= 0.
   */
  public static LinearSystem<N2, N1, N2> createWheelSystem(
      DCMotor motor, Mass mass, Distance radius, double gearing) {
    return createElevatorSystem(motor, mass.in(Kilograms), radius.in(Meters), gearing);
  }

  /**
   * Create a state-space model of a wheel system. The states of the system are [linear position,
   * linear velocity], inputs are [torque], and outputs are [linear position, linear velocity].
   *
   * @param massKg The mass of the system, in kilograms.
   * @param radiusMeters The radius of the system, in meters.
   * @return A LinearSystem representing the given characterized constants.
   * @throws IllegalArgumentException if massKg &lt;= 0, radiusMeters &lt;= 0.
   */
  public static LinearSystem<N2, N1, N2> createWheelTorqueSystem(
      double massKg, double radiusMeters) {
    if (massKg <= 0.0) {
      throw new IllegalArgumentException("massKg must be greater than zero.");
    }
    if (radiusMeters <= 0.0) {
      throw new IllegalArgumentException("radiusMeters must be greater than zero.");
    }

    return new LinearSystem<>(
        Matrix.eye(Nat.N2()),
        VecBuilder.fill(0, 1.0 / (massKg * radiusMeters)),
        Matrix.eye(Nat.N2()),
        new Matrix<>(Nat.N2(), Nat.N1()));
  }

  /**
   * Create a state-space model of a wheel system. The states of the system are [linear position,
   * linear velocity], inputs are [torque], and outputs are [linear position, linear velocity].
   *
   * @param mass The mass of the system.
   * @param radius The radius of the system.
   * @return A LinearSystem representing the given characterized constants.
   * @throws IllegalArgumentException if mass &lt;= 0, radius &lt;= 0.
   */
  public static LinearSystem<N2, N1, N2> createWheelTorqueSystem(Mass mass, Distance radius) {
    return createElevatorTorqueSystem(mass.in(Kilograms), radius.in(Meters));
  }

  /**
   * Create a state-space model for a 1 DOF velocity system from its kV (input units/(unit/sec)) and
   * kA (input units/(unit/sec²). These constants can be found using SysId. The states of the system
   * are [velocity], inputs are [voltage or torque], and outputs are [velocity].
   *
   * <p>The distance unit you choose MUST be an SI unit (i.e. meters or radians). You can use the
   * {@link edu.wpi.first.math.util.Units} class for converting between unit types.
   *
   * <p>The parameters provided by the user are from this feedforward model:
   *
   * <p>u = K_v v + K_a a
   *
   * @param <U> The input parameter either as voltage or torque.
   * @param <V> The velocity gain parameter either as LinearVelocity or AngularVelocity.
   * @param <A> The acceleration gain parameter either as LinearAcceleration or AngularAcceleration.
   * @param kV The velocity gain, in input units/(unit/sec)
   * @param kA The acceleration gain, in input units/(unit/sec²)
   * @return A LinearSystem representing the given characterized constants.
   * @throws IllegalArgumentException if kV &lt; 0 or kA &lt;= 0.
   * @see <a href= "https://github.com/wpilibsuite/sysid">https://github.com/wpilibsuite/sysid</a>
   */
  public static <U extends Unit, V extends Unit, A extends Unit>
      LinearSystem<N1, N1, N1> identifyVelocitySystem(
          Measure<? extends PerUnit<U, V>> kV, Measure<? extends PerUnit<U, A>> kA) {
    return identifyVelocitySystem(kV.baseUnitMagnitude(), kA.baseUnitMagnitude());
  }

  /**
   * Create a state-space model for a 1 DOF velocity system from its kV (input units/(unit/sec)) and
   * kA (input units/(unit/sec²). These constants can be found using SysId. The states of the system
   * are [velocity], inputs are [voltage or torque], and outputs are [velocity].
   *
   * <p>The distance unit you choose MUST be an SI unit (i.e. meters or radians). You can use the
   * {@link edu.wpi.first.math.util.Units} class for converting between unit types.
   *
   * <p>The parameters provided by the user are from this feedforward model:
   *
   * <p>u = K_v v + K_a a
   *
   * @param kV The velocity gain, in input units/(unit/sec)
   * @param kA The acceleration gain, in input units/(unit/sec²)
   * @return A LinearSystem representing the given characterized constants.
   * @throws IllegalArgumentException if kV &lt; 0 or kA &lt;= 0.
   * @see <a href= "https://github.com/wpilibsuite/sysid">https://github.com/wpilibsuite/sysid</a>
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
   * Create a state-space model for a 1 DOF position system from its kV (input units/(unit/sec)) and
   * kA (input units/(unit/sec²). These constants can be found using SysId. The states of the system
   * are [position, velocity]ᵀ, inputs are [voltage or torque], and outputs are [position].
   *
   * <p>The distance unit you choose MUST be an SI unit (i.e. meters or radians). You can use the
   * {@link edu.wpi.first.math.util.Units} class for converting between unit types.
   *
   * <p>The parameters provided by the user are from this feedforward model:
   *
   * <p>u = K_v v + K_a a
   *
   * @param <U> The input parameter either as voltage or torque.
   * @param <V> The velocity gain parameter either as LinearVelocity or AngularVelocity.
   * @param <A> The acceleration gain parameter either as LinearAcceleration or AngularAcceleration.
   * @param kV The velocity gain, in volts/(unit/sec)
   * @param kA The acceleration gain, in volts/(unit/sec²)
   * @return A LinearSystem representing the given characterized constants.
   * @throws IllegalArgumentException if kV &lt; 0 or kA &lt;= 0.
   * @see <a href= "https://github.com/wpilibsuite/sysid">https://github.com/wpilibsuite/sysid</a>
   */
  public static <U extends Unit, V extends Unit, A extends Unit>
      LinearSystem<N2, N1, N2> identifyPositionSystem(
          Measure<? extends PerUnit<U, V>> kV, Measure<? extends PerUnit<U, A>> kA) {
    return identifyPositionSystem(kV.baseUnitMagnitude(), kA.baseUnitMagnitude());
  }

  /**
   * Create a state-space model for a 1 DOF position system from its kV (input units/(unit/sec)) and
   * kA (input units/(unit/sec²). These constants can be found using SysId. The states of the system
   * are [position, velocity]ᵀ, inputs are [voltage or torque], and outputs are [position].
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
   * @see <a href= "https://github.com/wpilibsuite/sysid">https://github.com/wpilibsuite/sysid</a>
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
   * @see <a href= "https://github.com/wpilibsuite/sysid">https://github.com/wpilibsuite/sysid</a>
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
   * @see <a href= "https://github.com/wpilibsuite/sysid">https://github.com/wpilibsuite/sysid</a>
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
