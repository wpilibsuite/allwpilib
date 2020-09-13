/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "frc/StateSpaceUtil.h"
#include "frc/system/LinearSystem.h"
#include "frc/system/plant/DCMotor.h"
#include "units/moment_of_inertia.h"

namespace frc {

class LinearSystemId {
 public:
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
  static LinearSystem<1, 1, 1> IdentifyVelocitySystem(double kV, double kA) {
    auto A = frc::MakeMatrix<1, 1>(-kV / kA);
    auto B = frc::MakeMatrix<1, 1>(1.0 / kA);
    auto C = frc::MakeMatrix<1, 1>(1.0);
    auto D = frc::MakeMatrix<1, 1>(0.0);

    return LinearSystem<1, 1, 1>(A, B, C, D);
  }

  /**
   * Constructs the state-space model for a 1 DOF position system from system
   * identification data.
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
  static LinearSystem<2, 1, 1> IdentifyPositionSystem(double kV, double kA) {
    auto A = frc::MakeMatrix<2, 2>(0.0, 1.0, 0.0, -kV / kA);
    auto B = frc::MakeMatrix<2, 1>(0.0, 1.0 / kA);
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
  static LinearSystem<2, 2, 2> IdentifyDrivetrainSystem(double kVlinear,
                                                        double kAlinear,
                                                        double kVangular,
                                                        double kAangular) {
    double c = 0.5 / (kAlinear * kAangular);
    double A1 = c * (-kAlinear * kVangular - kVlinear * kAangular);
    double A2 = c * (kAlinear * kVangular - kVlinear * kAangular);
    double B1 = c * (kAlinear + kAangular);
    double B2 = c * (kAangular - kAlinear);

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
      DCMotor motor, units::kilogram_t m, units::meter_t r, units::meter_t rb,
      units::kilogram_square_meter_t J, double G) {
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
