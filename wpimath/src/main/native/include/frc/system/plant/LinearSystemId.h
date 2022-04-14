// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

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
   * Constructs the state-space model for an elevator.
   *
   * States: [[position], [velocity]]
   * Inputs: [[voltage]]
   * Outputs: [[position]]
   *
   * @param motor Instance of DCMotor.
   * @param m Carriage mass.
   * @param r Pulley radius.
   * @param G Gear ratio from motor to carriage.
   * @throws std::domain_error if m <= 0, r <= 0, or G <= 0.
   */
  static LinearSystem<2, 1, 1> ElevatorSystem(DCMotor motor,
                                              units::kilogram_t m,
                                              units::meter_t r, double G) {
    if (m <= 0_kg) {
      throw std::domain_error("m must be greater than zero.");
    }
    if (r <= 0_m) {
      throw std::domain_error("r must be greater than zero.");
    }
    if (G <= 0.0) {
      throw std::domain_error("G must be greater than zero.");
    }

    Eigen::Matrix<double, 2, 2> A{
        {0.0, 1.0},
        {0.0, (-std::pow(G, 2) * motor.Kt /
               (motor.R * units::math::pow<2>(r) * m * motor.Kv))
                  .value()}};
    Eigen::Matrix<double, 2, 1> B{0.0,
                                  (G * motor.Kt / (motor.R * r * m)).value()};
    Eigen::Matrix<double, 1, 2> C{1.0, 0.0};
    Eigen::Matrix<double, 1, 1> D{0.0};

    return LinearSystem<2, 1, 1>(A, B, C, D);
  }

  /**
   * Constructs the state-space model for a single-jointed arm.
   *
   * States: [[angle], [angular velocity]]
   * Inputs: [[voltage]]
   * Outputs: [[angle]]
   *
   * @param motor Instance of DCMotor.
   * @param J Moment of inertia.
   * @param G Gear ratio from motor to carriage.
   * @throws std::domain_error if J <= 0 or G <= 0.
   */
  static LinearSystem<2, 1, 1> SingleJointedArmSystem(
      DCMotor motor, units::kilogram_square_meter_t J, double G) {
    if (J <= 0_kg_sq_m) {
      throw std::domain_error("J must be greater than zero.");
    }
    if (G <= 0.0) {
      throw std::domain_error("G must be greater than zero.");
    }

    Eigen::Matrix<double, 2, 2> A{
        {0.0, 1.0},
        {0.0, (-std::pow(G, 2) * motor.Kt / (motor.Kv * motor.R * J)).value()}};
    Eigen::Matrix<double, 2, 1> B{0.0, (G * motor.Kt / (motor.R * J)).value()};
    Eigen::Matrix<double, 1, 2> C{1.0, 0.0};
    Eigen::Matrix<double, 1, 1> D{0.0};

    return LinearSystem<2, 1, 1>(A, B, C, D);
  }

  /**
   * Constructs the state-space model for a 1 DOF velocity-only system from
   * system identification data.
   *
   * You MUST use an SI unit (i.e. meters or radians) for the Distance template
   * argument. You may still use non-SI units (such as feet or inches) for the
   * actual method arguments; they will automatically be converted to SI
   * internally.
   *
   * States: [[velocity]]
   * Inputs: [[voltage]]
   * Outputs: [[velocity]]
   *
   * The parameters provided by the user are from this feedforward model:
   *
   * u = K_v v + K_a a
   *
   * @param kV The velocity gain, in volt seconds per distance.
   * @param kA The acceleration gain, in volt seconds^2 per distance.
   * @throws std::domain_error if kV <= 0 or kA <= 0.
   */
  template <typename Distance, typename = std::enable_if_t<
                                   std::is_same_v<units::meter, Distance> ||
                                   std::is_same_v<units::radian, Distance>>>
  static LinearSystem<1, 1, 1> IdentifyVelocitySystem(
      decltype(1_V / Velocity_t<Distance>(1)) kV,
      decltype(1_V / Acceleration_t<Distance>(1)) kA) {
    if (kV <= decltype(kV){0}) {
      throw std::domain_error("Kv must be greater than zero.");
    }
    if (kA <= decltype(kA){0}) {
      throw std::domain_error("Ka must be greater than zero.");
    }

    Eigen::Matrix<double, 1, 1> A{-kV.value() / kA.value()};
    Eigen::Matrix<double, 1, 1> B{1.0 / kA.value()};
    Eigen::Matrix<double, 1, 1> C{1.0};
    Eigen::Matrix<double, 1, 1> D{0.0};

    return LinearSystem<1, 1, 1>(A, B, C, D);
  }

  /**
   * Constructs the state-space model for a 1 DOF position system from system
   * identification data.
   *
   * You MUST use an SI unit (i.e. meters or radians) for the Distance template
   * argument. You may still use non-SI units (such as feet or inches) for the
   * actual method arguments; they will automatically be converted to SI
   * internally.
   *
   * States: [[position], [velocity]]
   * Inputs: [[voltage]]
   * Outputs: [[position]]
   *
   * The parameters provided by the user are from this feedforward model:
   *
   * u = K_v v + K_a a
   *
   * @param kV The velocity gain, in volt seconds per distance.
   * @param kA The acceleration gain, in volt seconds^2 per distance.
   * @throws std::domain_error if kV <= 0 or kA <= 0.
   */
  template <typename Distance, typename = std::enable_if_t<
                                   std::is_same_v<units::meter, Distance> ||
                                   std::is_same_v<units::radian, Distance>>>
  static LinearSystem<2, 1, 1> IdentifyPositionSystem(
      decltype(1_V / Velocity_t<Distance>(1)) kV,
      decltype(1_V / Acceleration_t<Distance>(1)) kA) {
    if (kV <= decltype(kV){0}) {
      throw std::domain_error("Kv must be greater than zero.");
    }
    if (kA <= decltype(kA){0}) {
      throw std::domain_error("Ka must be greater than zero.");
    }

    Eigen::Matrix<double, 2, 2> A{{0.0, 1.0}, {0.0, -kV.value() / kA.value()}};
    Eigen::Matrix<double, 2, 1> B{0.0, 1.0 / kA.value()};
    Eigen::Matrix<double, 1, 2> C{1.0, 0.0};
    Eigen::Matrix<double, 1, 1> D{0.0};

    return LinearSystem<2, 1, 1>(A, B, C, D);
  }

  /**
   * Constructs the state-space model for a 2 DOF drivetrain velocity system
   * from system identification data.
   *
   * States: [[left velocity], [right velocity]]
   * Inputs: [[left voltage], [right voltage]]
   * Outputs: [[left velocity], [right velocity]]
   *
   * @param kVlinear  The linear velocity gain in volts per (meter per second).
   * @param kAlinear  The linear acceleration gain in volts per (meter per
   *                  second squared).
   * @param kVangular The angular velocity gain in volts per (meter per second).
   * @param kAangular The angular acceleration gain in volts per (meter per
   *                  second squared).
   * @throws domain_error if kVlinear <= 0, kAlinear <= 0, kVangular <= 0,
   *         or kAangular <= 0.
   */
  static LinearSystem<2, 2, 2> IdentifyDrivetrainSystem(
      decltype(1_V / 1_mps) kVlinear, decltype(1_V / 1_mps_sq) kAlinear,
      decltype(1_V / 1_mps) kVangular, decltype(1_V / 1_mps_sq) kAangular) {
    if (kVlinear <= decltype(kVlinear){0}) {
      throw std::domain_error("Kv,linear must be greater than zero.");
    }
    if (kAlinear <= decltype(kAlinear){0}) {
      throw std::domain_error("Ka,linear must be greater than zero.");
    }
    if (kVangular <= decltype(kVangular){0}) {
      throw std::domain_error("Kv,angular must be greater than zero.");
    }
    if (kAangular <= decltype(kAangular){0}) {
      throw std::domain_error("Ka,angular must be greater than zero.");
    }

    double A1 = -(kVlinear.value() / kAlinear.value() +
                  kVangular.value() / kAangular.value());
    double A2 = -(kVlinear.value() / kAlinear.value() -
                  kVangular.value() / kAangular.value());
    double B1 = 1.0 / kAlinear.value() + 1.0 / kAangular.value();
    double B2 = 1.0 / kAlinear.value() - 1.0 / kAangular.value();

    Eigen::Matrix<double, 2, 2> A =
        0.5 * Eigen::Matrix<double, 2, 2>{{A1, A2}, {A2, A1}};
    Eigen::Matrix<double, 2, 2> B =
        0.5 * Eigen::Matrix<double, 2, 2>{{B1, B2}, {B2, B1}};
    Eigen::Matrix<double, 2, 2> C{{1.0, 0.0}, {0.0, 1.0}};
    Eigen::Matrix<double, 2, 2> D{{0.0, 0.0}, {0.0, 0.0}};

    return LinearSystem<2, 2, 2>(A, B, C, D);
  }

  /**
   * Constructs the state-space model for a 2 DOF drivetrain velocity system
   * from system identification data.
   *
   * States: [[left velocity], [right velocity]]
   * Inputs: [[left voltage], [right voltage]]
   * Outputs: [[left velocity], [right velocity]]
   *
   * @param kVlinear   The linear velocity gain in volts per (meter per second).
   * @param kAlinear   The linear acceleration gain in volts per (meter per
   *                   second squared).
   * @param kVangular  The angular velocity gain in volts per (radian per
   *                   second).
   * @param kAangular  The angular acceleration gain in volts per (radian per
   *                   second squared).
   * @param trackwidth The width of the drivetrain.
   * @throws domain_error if kVlinear <= 0, kAlinear <= 0, kVangular <= 0,
   *         kAangular <= 0, or trackwidth <= 0.
   */
  static LinearSystem<2, 2, 2> IdentifyDrivetrainSystem(
      decltype(1_V / 1_mps) kVlinear, decltype(1_V / 1_mps_sq) kAlinear,
      decltype(1_V / 1_rad_per_s) kVangular,
      decltype(1_V / 1_rad_per_s_sq) kAangular, units::meter_t trackwidth) {
    if (kVlinear <= decltype(kVlinear){0}) {
      throw std::domain_error("Kv,linear must be greater than zero.");
    }
    if (kAlinear <= decltype(kAlinear){0}) {
      throw std::domain_error("Ka,linear must be greater than zero.");
    }
    if (kVangular <= decltype(kVangular){0}) {
      throw std::domain_error("Kv,angular must be greater than zero.");
    }
    if (kAangular <= decltype(kAangular){0}) {
      throw std::domain_error("Ka,angular must be greater than zero.");
    }
    if (trackwidth <= 0_m) {
      throw std::domain_error("r must be greater than zero.");
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
    return IdentifyDrivetrainSystem(kVlinear, kAlinear,
                                    kVangular * 2.0 / trackwidth * 1_rad,
                                    kAangular * 2.0 / trackwidth * 1_rad);
  }

  /**
   * Constructs the state-space model for a flywheel.
   *
   * States: [[angular velocity]]
   * Inputs: [[voltage]]
   * Outputs: [[angular velocity]]
   *
   * @param motor Instance of DCMotor.
   * @param J Moment of inertia.
   * @param G Gear ratio from motor to carriage.
   * @throws std::domain_error if J <= 0 or G <= 0.
   */
  static LinearSystem<1, 1, 1> FlywheelSystem(DCMotor motor,
                                              units::kilogram_square_meter_t J,
                                              double G) {
    if (J <= 0_kg_sq_m) {
      throw std::domain_error("J must be greater than zero.");
    }
    if (G <= 0.0) {
      throw std::domain_error("G must be greater than zero.");
    }

    Eigen::Matrix<double, 1, 1> A{
        (-std::pow(G, 2) * motor.Kt / (motor.Kv * motor.R * J)).value()};
    Eigen::Matrix<double, 1, 1> B{(G * motor.Kt / (motor.R * J)).value()};
    Eigen::Matrix<double, 1, 1> C{1.0};
    Eigen::Matrix<double, 1, 1> D{0.0};

    return LinearSystem<1, 1, 1>(A, B, C, D);
  }

  /**
   * Constructs the state-space model for a DC motor motor.
   *
   * States: [[angular position, angular velocity]]
   * Inputs: [[voltage]]
   * Outputs: [[angular position, angular velocity]]
   *
   * @param motor Instance of DCMotor.
   * @param J Moment of inertia.
   * @param G Gear ratio from motor to carriage.
   * @throws std::domain_error if J <= 0 or G <= 0.
   */
  static LinearSystem<2, 1, 2> DCMotorSystem(DCMotor motor,
                                             units::kilogram_square_meter_t J,
                                             double G) {
    if (J <= 0_kg_sq_m) {
      throw std::domain_error("J must be greater than zero.");
    }
    if (G <= 0.0) {
      throw std::domain_error("G must be greater than zero.");
    }

    Eigen::Matrix<double, 2, 2> A{
        {0.0, 1.0},
        {0.0, (-std::pow(G, 2) * motor.Kt / (motor.Kv * motor.R * J)).value()}};
    Eigen::Matrix<double, 2, 1> B{0.0, (G * motor.Kt / (motor.R * J)).value()};
    Eigen::Matrix<double, 2, 2> C{{1.0, 0.0}, {0.0, 1.0}};
    Eigen::Matrix<double, 2, 1> D{0.0, 0.0};

    return LinearSystem<2, 1, 2>(A, B, C, D);
  }

  /**
   * Constructs the state-space model for a drivetrain.
   *
   * States: [[left velocity], [right velocity]]
   * Inputs: [[left voltage], [right voltage]]
   * Outputs: [[left velocity], [right velocity]]
   *
   * @param motor Instance of DCMotor.
   * @param m Drivetrain mass.
   * @param r Wheel radius.
   * @param rb Robot radius.
   * @param J Moment of inertia.
   * @param G Gear ratio from motor to wheel.
   * @throws std::domain_error if m <= 0, r <= 0, rb <= 0, J <= 0, or
   *         G <= 0.
   */
  static LinearSystem<2, 2, 2> DrivetrainVelocitySystem(
      const DCMotor& motor, units::kilogram_t m, units::meter_t r,
      units::meter_t rb, units::kilogram_square_meter_t J, double G) {
    if (m <= 0_kg) {
      throw std::domain_error("m must be greater than zero.");
    }
    if (r <= 0_m) {
      throw std::domain_error("r must be greater than zero.");
    }
    if (rb <= 0_m) {
      throw std::domain_error("rb must be greater than zero.");
    }
    if (J <= 0_kg_sq_m) {
      throw std::domain_error("J must be greater than zero.");
    }
    if (G <= 0.0) {
      throw std::domain_error("G must be greater than zero.");
    }

    auto C1 = -std::pow(G, 2) * motor.Kt /
              (motor.Kv * motor.R * units::math::pow<2>(r));
    auto C2 = G * motor.Kt / (motor.R * r);

    Eigen::Matrix<double, 2, 2> A{
        {((1 / m + units::math::pow<2>(rb) / J) * C1).value(),
         ((1 / m - units::math::pow<2>(rb) / J) * C1).value()},
        {((1 / m - units::math::pow<2>(rb) / J) * C1).value(),
         ((1 / m + units::math::pow<2>(rb) / J) * C1).value()}};
    Eigen::Matrix<double, 2, 2> B{
        {((1 / m + units::math::pow<2>(rb) / J) * C2).value(),
         ((1 / m - units::math::pow<2>(rb) / J) * C2).value()},
        {((1 / m - units::math::pow<2>(rb) / J) * C2).value(),
         ((1 / m + units::math::pow<2>(rb) / J) * C2).value()}};
    Eigen::Matrix<double, 2, 2> C{{1.0, 0.0}, {0.0, 1.0}};
    Eigen::Matrix<double, 2, 2> D{{0.0, 0.0}, {0.0, 0.0}};

    return LinearSystem<2, 2, 2>(A, B, C, D);
  }
};

}  // namespace frc
