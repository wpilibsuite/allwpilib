// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "frc/StateSpaceUtil.h"
#include "frc/system/LinearSystem.h"
#include "frc/system/plant/DCMotor.h"
#include "units/acceleration.h"
#include "units/angular_acceleration.h"
#include "units/angular_velocity.h"
#include "units/moment_of_inertia.h"
#include "units/velocity.h"
#include "units/voltage.h"

namespace frc {
class LinearSystemId {
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
   */
  static LinearSystem<2, 1, 1> ElevatorSystem(DCMotor motor,
                                              units::kilogram_t m,
                                              units::meter_t r, double G) {
    auto A = frc::MakeMatrix<2, 2>(
        0.0, 1.0, 0.0,
        (-std::pow(G, 2) * motor.Kt /
         (motor.R * units::math::pow<2>(r) * m * motor.Kv))
            .to<double>());
    auto B = frc::MakeMatrix<2, 1>(
        0.0, (G * motor.Kt / (motor.R * r * m)).to<double>());
    auto C = frc::MakeMatrix<1, 2>(1.0, 0.0);
    auto D = frc::MakeMatrix<1, 1>(0.0);

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
   */
  static LinearSystem<2, 1, 1> SingleJointedArmSystem(
      DCMotor motor, units::kilogram_square_meter_t J, double G) {
    auto A = frc::MakeMatrix<2, 2>(
        0.0, 1.0, 0.0,
        (-std::pow(G, 2) * motor.Kt / (motor.Kv * motor.R * J)).to<double>());
    auto B =
        frc::MakeMatrix<2, 1>(0.0, (G * motor.Kt / (motor.R * J)).to<double>());
    auto C = frc::MakeMatrix<1, 2>(1.0, 0.0);
    auto D = frc::MakeMatrix<1, 1>(0.0);

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
   */
  template <typename Distance, typename = std::enable_if_t<
                                   std::is_same_v<units::meter, Distance> ||
                                   std::is_same_v<units::radian, Distance>>>
  static LinearSystem<1, 1, 1> IdentifyVelocitySystem(
      decltype(1_V / Velocity_t<Distance>(1)) kV,
      decltype(1_V / Acceleration_t<Distance>(1)) kA) {
    auto A = frc::MakeMatrix<1, 1>(-kV.template to<double>() /
                                   kA.template to<double>());
    auto B = frc::MakeMatrix<1, 1>(1.0 / kA.template to<double>());
    auto C = frc::MakeMatrix<1, 1>(1.0);
    auto D = frc::MakeMatrix<1, 1>(0.0);

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
   */
  template <typename Distance, typename = std::enable_if_t<
                                   std::is_same_v<units::meter, Distance> ||
                                   std::is_same_v<units::radian, Distance>>>
  static LinearSystem<2, 1, 1> IdentifyPositionSystem(
      decltype(1_V / Velocity_t<Distance>(1)) kV,
      decltype(1_V / Acceleration_t<Distance>(1)) kA) {
    auto A = frc::MakeMatrix<2, 2>(
        0.0, 1.0, 0.0, -kV.template to<double>() / kA.template to<double>());
    auto B = frc::MakeMatrix<2, 1>(0.0, 1.0 / kA.template to<double>());
    auto C = frc::MakeMatrix<1, 2>(1.0, 0.0);
    auto D = frc::MakeMatrix<1, 1>(0.0);

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
   * @param kVlinear The linear velocity gain, in volt seconds per distance.
   * @param kAlinear The linear acceleration gain, in volt seconds^2 per
   * distance.
   * @param kVangular The angular velocity gain, in volt seconds per angle.
   * @param kAangular The angular acceleration gain, in volt seconds^2 per
   * angle.
   */
  static LinearSystem<2, 2, 2> IdentifyDrivetrainSystem(
      decltype(1_V / 1_mps) kVlinear, decltype(1_V / 1_mps_sq) kAlinear,
      decltype(1_V / 1_rad_per_s) kVangular,
      decltype(1_V / 1_rad_per_s_sq) kAangular) {
    double c = 0.5 / (kAlinear.to<double>() * kAangular.to<double>());
    double A1 = c * (-kAlinear.to<double>() * kVangular.to<double>() -
                     kVlinear.to<double>() * kAangular.to<double>());
    double A2 = c * (kAlinear.to<double>() * kVangular.to<double>() -
                     kVlinear.to<double>() * kAangular.to<double>());
    double B1 = c * (kAlinear.to<double>() + kAangular.to<double>());
    double B2 = c * (kAangular.to<double>() - kAlinear.to<double>());

    auto A = frc::MakeMatrix<2, 2>(A1, A2, A2, A1);
    auto B = frc::MakeMatrix<2, 2>(B1, B2, B2, B1);
    auto C = frc::MakeMatrix<2, 2>(1.0, 0.0, 0.0, 1.0);
    auto D = frc::MakeMatrix<2, 2>(0.0, 0.0, 0.0, 0.0);

    return LinearSystem<2, 2, 2>(A, B, C, D);
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
   */
  static LinearSystem<1, 1, 1> FlywheelSystem(DCMotor motor,
                                              units::kilogram_square_meter_t J,
                                              double G) {
    auto A = frc::MakeMatrix<1, 1>(
        (-std::pow(G, 2) * motor.Kt / (motor.Kv * motor.R * J)).to<double>());
    auto B = frc::MakeMatrix<1, 1>((G * motor.Kt / (motor.R * J)).to<double>());
    auto C = frc::MakeMatrix<1, 1>(1.0);
    auto D = frc::MakeMatrix<1, 1>(0.0);

    return LinearSystem<1, 1, 1>(A, B, C, D);
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
   * @param G Gear ratio from motor to wheel.
   * @param J Moment of inertia.
   */
  static LinearSystem<2, 2, 2> DrivetrainVelocitySystem(
      const DCMotor& motor, units::kilogram_t m, units::meter_t r,
      units::meter_t rb, units::kilogram_square_meter_t J, double G) {
    auto C1 = -std::pow(G, 2) * motor.Kt /
              (motor.Kv * motor.R * units::math::pow<2>(r));
    auto C2 = G * motor.Kt / (motor.R * r);

    auto A = frc::MakeMatrix<2, 2>(
        ((1 / m + units::math::pow<2>(rb) / J) * C1).to<double>(),
        ((1 / m - units::math::pow<2>(rb) / J) * C1).to<double>(),
        ((1 / m - units::math::pow<2>(rb) / J) * C1).to<double>(),
        ((1 / m + units::math::pow<2>(rb) / J) * C1).to<double>());
    auto B = frc::MakeMatrix<2, 2>(
        ((1 / m + units::math::pow<2>(rb) / J) * C2).to<double>(),
        ((1 / m - units::math::pow<2>(rb) / J) * C2).to<double>(),
        ((1 / m - units::math::pow<2>(rb) / J) * C2).to<double>(),
        ((1 / m + units::math::pow<2>(rb) / J) * C2).to<double>());
    auto C = frc::MakeMatrix<2, 2>(1.0, 0.0, 0.0, 1.0);
    auto D = frc::MakeMatrix<2, 2>(0.0, 0.0, 0.0, 0.0);

    return LinearSystem<2, 2, 2>(A, B, C, D);
  }
};

}  // namespace frc
