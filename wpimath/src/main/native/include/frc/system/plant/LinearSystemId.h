// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <concepts>
#include <stdexcept>

#include <wpi/SymbolExports.h>

#include "frc/system/LinearSystem.h"
#include "frc/system/plant/DCMotor.h"
#include "units/acceleration.h"
#include "units/angular_acceleration.h"
#include "units/angular_velocity.h"
#include "units/length.h"
#include "units/moment_of_inertia.h"
#include "units/velocity.h"
#include "units/voltage.h"

namespace frc {
/**
 * Linear system ID utility functions.
 */
class WPILIB_DLLEXPORT LinearSystemId {
 public:
  template <typename Distance>
  using Velocity_t = units::unit_t<
      units::compound_unit<Distance, units::inverse<units::seconds>>>;

  template <typename Distance>
  using Acceleration_t = units::unit_t<units::compound_unit<
      units::compound_unit<Distance, units::inverse<units::seconds>>,
      units::inverse<units::seconds>>>;

  /**
   * Create a state-space model of the elevator system. The states of the system
   * are [position, velocity], inputs are [voltage], and outputs are [position].
   *
   * @param motor The motor (or gearbox) attached to the carriage.
   * @param mass The mass of the elevator carriage, in kilograms.
   * @param radius The radius of the elevator's driving drum, in meters.
   * @param gearing Gear ratio from motor to carriage.
   * @throws std::domain_error if mass <= 0, radius <= 0, or gearing <= 0.
   */
  static LinearSystem<2, 1, 1> ElevatorSystem(DCMotor motor,
                                              units::kilogram_t mass,
                                              units::meter_t radius,
                                              double gearing);

  /**
   * Create a state-space model of a single-jointed arm system.The states of the
   * system are [angle, angular velocity], inputs are [voltage], and outputs are
   * [angle].
   *
   * @param motor The motor (or gearbox) attached to the arm.
   * @param J The moment of inertia J of the arm.
   * @param gearing Gear ratio from motor to arm.
   * @throws std::domain_error if J <= 0 or gearing <= 0.
   */
  static LinearSystem<2, 1, 1> SingleJointedArmSystem(
      DCMotor motor, units::kilogram_square_meter_t J, double gearing);

  /**
   * Create a state-space model for a 1 DOF velocity system from its kV
   * (volts/(unit/sec)) and kA (volts/(unit/sec²)). These constants can be
   * found using SysId. The states of the system are [velocity], inputs are
   * [voltage], and outputs are [velocity].
   *
   * You MUST use an SI unit (i.e. meters or radians) for the Distance template
   * argument. You may still use non-SI units (such as feet or inches) for the
   * actual method arguments; they will automatically be converted to SI
   * internally.
   *
   * The parameters provided by the user are from this feedforward model:
   *
   * u = K_v v + K_a a
   *
   * @param kV The velocity gain, in volts/(unit/sec).
   * @param kA The acceleration gain, in volts/(unit/sec²).
   * @throws std::domain_error if kV < 0 or kA <= 0.
   * @see <a
   * href="https://github.com/wpilibsuite/sysid">https://github.com/wpilibsuite/sysid</a>
   */
  template <typename Distance>
    requires std::same_as<units::meter, Distance> ||
             std::same_as<units::radian, Distance>
  static LinearSystem<1, 1, 1> IdentifyVelocitySystem(
      decltype(1_V / Velocity_t<Distance>(1)) kV,
      decltype(1_V / Acceleration_t<Distance>(1)) kA) {
    if (kV < decltype(kV){0}) {
      throw std::domain_error("Kv must be greater than or equal to zero.");
    }
    if (kA <= decltype(kA){0}) {
      throw std::domain_error("Ka must be greater than zero.");
    }

    Matrixd<1, 1> A{-kV.value() / kA.value()};
    Matrixd<1, 1> B{1.0 / kA.value()};
    Matrixd<1, 1> C{1.0};
    Matrixd<1, 1> D{0.0};

    return LinearSystem<1, 1, 1>(A, B, C, D);
  }

  /**
   * Create a state-space model for a 1 DOF position system from its kV
   * (volts/(unit/sec)) and kA (volts/(unit/sec²)). These constants can be
   * found using SysId. the states of the system are [position, velocity],
   * inputs are [voltage], and outputs are [position].
   *
   * You MUST use an SI unit (i.e. meters or radians) for the Distance template
   * argument. You may still use non-SI units (such as feet or inches) for the
   * actual method arguments; they will automatically be converted to SI
   * internally.
   *
   * The parameters provided by the user are from this feedforward model:
   *
   * u = K_v v + K_a a
   *
   * @param kV The velocity gain, in volts/(unit/sec).
   * @param kA The acceleration gain, in volts/(unit/sec²).
   *
   * @throws std::domain_error if kV < 0 or kA <= 0.
   * @see <a
   * href="https://github.com/wpilibsuite/sysid">https://github.com/wpilibsuite/sysid</a>
   */
  template <typename Distance>
    requires std::same_as<units::meter, Distance> ||
             std::same_as<units::radian, Distance>
  static LinearSystem<2, 1, 1> IdentifyPositionSystem(
      decltype(1_V / Velocity_t<Distance>(1)) kV,
      decltype(1_V / Acceleration_t<Distance>(1)) kA) {
    if (kV < decltype(kV){0}) {
      throw std::domain_error("Kv must be greater than or equal to zero.");
    }
    if (kA <= decltype(kA){0}) {
      throw std::domain_error("Ka must be greater than zero.");
    }

    Matrixd<2, 2> A{{0.0, 1.0}, {0.0, -kV.value() / kA.value()}};
    Matrixd<2, 1> B{0.0, 1.0 / kA.value()};
    Matrixd<1, 2> C{1.0, 0.0};
    Matrixd<1, 1> D{0.0};

    return LinearSystem<2, 1, 1>(A, B, C, D);
  }

  /**
   * Identify a differential drive drivetrain given the drivetrain's kV and kA
   * in both linear {(volts/(meter/sec), (volts/(meter/sec²))} and angular
   * {(volts/(radian/sec), (volts/(radian/sec²))} cases. These constants can be
   * found using SysId.
   *
   * States: [[left velocity], [right velocity]]<br>
   * Inputs: [[left voltage], [right voltage]]<br>
   * Outputs: [[left velocity], [right velocity]]
   *
   * @param kVLinear  The linear velocity gain in volts per (meters per second).
   * @param kALinear  The linear acceleration gain in volts per (meters per
   *                  second squared).
   * @param kVAngular The angular velocity gain in volts per (meters per
   *                  second).
   * @param kAAngular The angular acceleration gain in volts per (meters per
   *                  second squared).
   * @throws domain_error if kVLinear <= 0, kALinear <= 0, kVAngular <= 0,
   *         or kAAngular <= 0.
   * @see <a
   * href="https://github.com/wpilibsuite/sysid">https://github.com/wpilibsuite/sysid</a>
   */
  static LinearSystem<2, 2, 2> IdentifyDrivetrainSystem(
      decltype(1_V / 1_mps) kVLinear, decltype(1_V / 1_mps_sq) kALinear,
      decltype(1_V / 1_mps) kVAngular, decltype(1_V / 1_mps_sq) kAAngular);

  /**
   * Identify a differential drive drivetrain given the drivetrain's kV and kA
   * in both linear {(volts/(meter/sec)), (volts/(meter/sec²))} and angular
   * {(volts/(radian/sec)), (volts/(radian/sec²))} cases. This can be found
   * using SysId.
   *
   * States: [[left velocity], [right velocity]]<br>
   * Inputs: [[left voltage], [right voltage]]<br>
   * Outputs: [[left velocity], [right velocity]]
   *
   * @param kVLinear   The linear velocity gain in volts per (meters per
   * second).
   * @param kALinear   The linear acceleration gain in volts per (meters per
   *                   second squared).
   * @param kVAngular  The angular velocity gain in volts per (radians per
   *                   second).
   * @param kAAngular  The angular acceleration gain in volts per (radians per
   *                   second squared).
   * @param trackwidth The distance between the differential drive's left and
   *                   right wheels, in meters.
   * @throws domain_error if kVLinear <= 0, kALinear <= 0, kVAngular <= 0,
   *         kAAngular <= 0, or trackwidth <= 0.
   * @see <a
   * href="https://github.com/wpilibsuite/sysid">https://github.com/wpilibsuite/sysid</a>
   */
  static LinearSystem<2, 2, 2> IdentifyDrivetrainSystem(
      decltype(1_V / 1_mps) kVLinear, decltype(1_V / 1_mps_sq) kALinear,
      decltype(1_V / 1_rad_per_s) kVAngular,
      decltype(1_V / 1_rad_per_s_sq) kAAngular, units::meter_t trackwidth);

  /**
   * Create a state-space model of a flywheel system, the states of the system
   * are [angular velocity], inputs are [voltage], and outputs are [angular
   * velocity].
   *
   * @param motor The motor (or gearbox) attached to the flywheel.
   * @param J The moment of inertia J of the flywheel.
   * @param gearing Gear ratio from motor to flywheel.
   * @throws std::domain_error if J <= 0 or gearing <= 0.
   */
  static LinearSystem<1, 1, 1> FlywheelSystem(DCMotor motor,
                                              units::kilogram_square_meter_t J,
                                              double gearing);

  /**
   * Create a state-space model of a DC motor system. The states of the system
   * are [angular position, angular velocity], inputs are [voltage], and outputs
   * are [angular position, angular velocity].
   *
   * @param motor The motor (or gearbox) attached to the system.
   * @param J the moment of inertia J of the DC motor.
   * @param gearing Gear ratio from motor to output.
   * @throws std::domain_error if J <= 0 or gearing <= 0.
   * @see <a
   * href="https://github.com/wpilibsuite/sysid">https://github.com/wpilibsuite/sysid</a>
   */
  static LinearSystem<2, 1, 2> DCMotorSystem(DCMotor motor,
                                             units::kilogram_square_meter_t J,
                                             double gearing);

  /**
   * Create a state-space model of a DC motor system from its kV
   * (volts/(unit/sec)) and kA (volts/(unit/sec²)). These constants can be
   * found using SysId. the states of the system are [position, velocity],
   * inputs are [voltage], and outputs are [position].
   *
   * You MUST use an SI unit (i.e. meters or radians) for the Distance template
   * argument. You may still use non-SI units (such as feet or inches) for the
   * actual method arguments; they will automatically be converted to SI
   * internally.
   *
   * The parameters provided by the user are from this feedforward model:
   *
   * u = K_v v + K_a a
   *
   * @param kV The velocity gain, in volts/(unit/sec).
   * @param kA The acceleration gain, in volts/(unit/sec²).
   *
   * @throws std::domain_error if kV < 0 or kA <= 0.
   */
  template <typename Distance>
    requires std::same_as<units::meter, Distance> ||
             std::same_as<units::radian, Distance>
  static LinearSystem<2, 1, 2> DCMotorSystem(
      decltype(1_V / Velocity_t<Distance>(1)) kV,
      decltype(1_V / Acceleration_t<Distance>(1)) kA) {
    if (kV < decltype(kV){0}) {
      throw std::domain_error("Kv must be greater than or equal to zero.");
    }
    if (kA <= decltype(kA){0}) {
      throw std::domain_error("Ka must be greater than zero.");
    }

    Matrixd<2, 2> A{{0.0, 1.0}, {0.0, -kV.value() / kA.value()}};
    Matrixd<2, 1> B{0.0, 1.0 / kA.value()};
    Matrixd<2, 2> C{{1.0, 0.0}, {0.0, 1.0}};
    Matrixd<2, 1> D{{0.0}, {0.0}};

    return LinearSystem<2, 1, 2>(A, B, C, D);
  }

  /**
   * Create a state-space model of differential drive drivetrain. In this model,
   * the states are [left velocity, right velocity], the inputs are [left
   * voltage, right voltage], and the outputs are [left velocity, right
   * velocity]
   *
   * @param motor The motor (or gearbox) driving the drivetrain.
   * @param mass The mass of the robot in kilograms.
   * @param r The radius of the wheels in meters.
   * @param rb The radius of the base (half of the track width), in meters.
   * @param J The moment of inertia of the robot.
   * @param gearing Gear ratio from motor to wheel.
   * @throws std::domain_error if mass <= 0, r <= 0, rb <= 0, J <= 0, or
   *         gearing <= 0.
   */
  static LinearSystem<2, 2, 2> DrivetrainVelocitySystem(
      const DCMotor& motor, units::kilogram_t mass, units::meter_t r,
      units::meter_t rb, units::kilogram_square_meter_t J, double gearing);
};

}  // namespace frc
