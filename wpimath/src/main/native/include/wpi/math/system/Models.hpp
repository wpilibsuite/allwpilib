// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdexcept>

#include <gcem.hpp>

#include "wpi/math/system/DCMotor.hpp"
#include "wpi/math/system/LinearSystem.hpp"
#include "wpi/units/acceleration.hpp"
#include "wpi/units/angular_acceleration.hpp"
#include "wpi/units/angular_velocity.hpp"
#include "wpi/units/length.hpp"
#include "wpi/units/moment_of_inertia.hpp"
#include "wpi/units/velocity.hpp"
#include "wpi/units/voltage.hpp"
#include "wpi/util/SymbolExports.hpp"

namespace wpi::math {
/**
 * Linear system factories.
 */
class WPILIB_DLLEXPORT Models {
 public:
  template <typename Distance>
  using Velocity_t = wpi::units::unit_t<wpi::units::compound_unit<
      Distance, wpi::units::inverse<wpi::units::seconds>>>;

  template <typename Distance>
  using Acceleration_t = wpi::units::unit_t<wpi::units::compound_unit<
      wpi::units::compound_unit<Distance,
                                wpi::units::inverse<wpi::units::seconds>>,
      wpi::units::inverse<wpi::units::seconds>>>;

  /**
   * Creates a flywheel state-space model from physical constants.
   *
   * The states are [angular velocity], the inputs are [voltage], and the
   * outputs are [angular velocity].
   *
   * @param motor The motor (or gearbox) attached to the flywheel.
   * @param J The moment of inertia J of the flywheel.
   * @param gearing Gear ratio from motor to flywheel (greater than 1 is a
   *     reduction).
   * @throws std::domain_error if J <= 0 or gearing <= 0.
   */
  static constexpr LinearSystem<1, 1, 1> FlywheelFromPhysicalConstants(
      DCMotor motor, wpi::units::kilogram_square_meter_t J, double gearing) {
    if (J <= 0_kg_sq_m) {
      throw std::domain_error("J must be greater than zero.");
    }
    if (gearing <= 0.0) {
      throw std::domain_error("gearing must be greater than zero.");
    }

    Matrixd<1, 1> A{
        {(-gcem::pow(gearing, 2) * motor.Kt / (motor.Kv * motor.R * J))
             .value()}};
    Matrixd<1, 1> B{{(gearing * motor.Kt / (motor.R * J)).value()}};
    Matrixd<1, 1> C{{1.0}};
    Matrixd<1, 1> D{{0.0}};

    return LinearSystem<1, 1, 1>(A, B, C, D);
  }

  /**
   * Creates a flywheel state-space model from SysId constants kᵥ (V/(rad/s))
   * and kₐ (V/(rad/s²)) from the feedforward model u = kᵥv + kₐa.
   *
   * The states are [velocity], the inputs are [voltage], and the outputs are
   * [velocity].
   *
   * @param kV The velocity gain, in V/(rad/s).
   * @param kA The acceleration gain, in V/(rad/s²).
   * @throws std::domain_error if kV < 0 or kA <= 0.
   * @see <a
   * href="https://github.com/wpilibsuite/allwpilib/tree/main/sysid">https://github.com/wpilibsuite/allwpilib/tree/main/sysid</a>
   */
  static constexpr LinearSystem<1, 1, 1> FlywheelFromSysId(
      decltype(1_V / 1_rad_per_s) kV, decltype(1_V / 1_rad_per_s_sq) kA) {
    if (kV < decltype(kV){0}) {
      throw std::domain_error("Kv must be greater than or equal to zero.");
    }
    if (kA <= decltype(kA){0}) {
      throw std::domain_error("Ka must be greater than zero.");
    }

    Matrixd<1, 1> A{{-kV.value() / kA.value()}};
    Matrixd<1, 1> B{{1.0 / kA.value()}};
    Matrixd<1, 1> C{{1.0}};
    Matrixd<1, 1> D{{0.0}};

    return LinearSystem<1, 1, 1>(A, B, C, D);
  }

  /**
   * Creates an elevator state-space model from physical constants.
   *
   * The states are [position, velocity], the inputs are [voltage], and the
   * outputs are [position, velocity].
   *
   * @param motor The motor (or gearbox) attached to the carriage.
   * @param mass The mass of the elevator carriage, in kilograms.
   * @param radius The radius of the elevator's driving drum, in meters.
   * @param gearing Gear ratio from motor to carriage (greater than 1 is a
   *     reduction).
   * @throws std::domain_error if mass <= 0, radius <= 0, or gearing <= 0.
   */
  static constexpr LinearSystem<2, 1, 2> ElevatorFromPhysicalConstants(
      DCMotor motor, wpi::units::kilogram_t mass, wpi::units::meter_t radius,
      double gearing) {
    if (mass <= 0_kg) {
      throw std::domain_error("mass must be greater than zero.");
    }
    if (radius <= 0_m) {
      throw std::domain_error("radius must be greater than zero.");
    }
    if (gearing <= 0.0) {
      throw std::domain_error("gearing must be greater than zero.");
    }

    Matrixd<2, 2> A{
        {0.0, 1.0},
        {0.0, (-gcem::pow(gearing, 2) * motor.Kt /
               (motor.R * wpi::units::math::pow<2>(radius) * mass * motor.Kv))
                  .value()}};
    Matrixd<2, 1> B{{0.0},
                    {(gearing * motor.Kt / (motor.R * radius * mass)).value()}};
    Matrixd<2, 2> C{{1.0, 0.0}, {0.0, 1.0}};
    Matrixd<2, 1> D{{0.0}, {0.0}};

    return LinearSystem<2, 1, 2>(A, B, C, D);
  }

  /**
   * Creates an elevator state-space model from SysId constants kᵥ (V/(m/s)) and
   * kₐ (V/(m/s²)) from the feedforward model u = kᵥv + kₐa.
   *
   * The states are [position, velocity], the inputs are [voltage], and the
   * outputs are [position, velocity].
   *
   * @param kV The velocity gain, in V/(m/s).
   * @param kA The acceleration gain, in V/(m/s²).
   *
   * @throws std::domain_error if kV < 0 or kA <= 0.
   * @see <a
   * href="https://github.com/wpilibsuite/allwpilib/tree/main/sysid">https://github.com/wpilibsuite/allwpilib/tree/main/sysid</a>
   */
  static constexpr LinearSystem<2, 1, 2> ElevatorFromSysId(
      decltype(1_V / 1_mps) kV, decltype(1_V / 1_mps_sq) kA) {
    if (kV < decltype(kV){0}) {
      throw std::domain_error("Kv must be greater than or equal to zero.");
    }
    if (kA <= decltype(kA){0}) {
      throw std::domain_error("Ka must be greater than zero.");
    }

    Matrixd<2, 2> A{{0.0, 1.0}, {0.0, -kV.value() / kA.value()}};
    Matrixd<2, 1> B{{0.0}, {1.0 / kA.value()}};
    Matrixd<2, 2> C{{1.0, 0.0}, {0.0, 1.0}};
    Matrixd<2, 1> D{{0.0}, {0.0}};

    return LinearSystem<2, 1, 2>(A, B, C, D);
  }

  /**
   * Create a single-jointed arm state-space model from physical constants.
   *
   * The states are [angle, angular velocity], the inputs are [voltage], and the
   * outputs are [angle, angular velocity].
   *
   * @param motor The motor (or gearbox) attached to the arm.
   * @param J The moment of inertia J of the arm.
   * @param gearing Gear ratio from motor to arm (greater than 1 is a
   *     reduction).
   * @throws std::domain_error if J <= 0 or gearing <= 0.
   */
  static constexpr LinearSystem<2, 1, 2> SingleJointedArmFromPhysicalConstants(
      DCMotor motor, wpi::units::kilogram_square_meter_t J, double gearing) {
    if (J <= 0_kg_sq_m) {
      throw std::domain_error("J must be greater than zero.");
    }
    if (gearing <= 0.0) {
      throw std::domain_error("gearing must be greater than zero.");
    }

    Matrixd<2, 2> A{
        {0.0, 1.0},
        {0.0, (-gcem::pow(gearing, 2) * motor.Kt / (motor.Kv * motor.R * J))
                  .value()}};
    Matrixd<2, 1> B{{0.0}, {(gearing * motor.Kt / (motor.R * J)).value()}};
    Matrixd<2, 2> C{{1.0, 0.0}, {0.0, 1.0}};
    Matrixd<2, 1> D{{0.0}, {0.0}};

    return LinearSystem<2, 1, 2>(A, B, C, D);
  }

  /**
   * Creates a single-jointed arm state-space model from SysId constants kᵥ
   * (V/(rad/s)) and kₐ (V/(rad/s²)) from the feedforward model u = kᵥv + kₐa.
   *
   * The states are [position, velocity], the inputs are [voltage], and the
   * outputs are [position, velocity].
   *
   * @param kV The velocity gain, in volts/(unit/sec).
   * @param kA The acceleration gain, in volts/(unit/sec²).
   *
   * @throws std::domain_error if kV < 0 or kA <= 0.
   * @see <a
   * href="https://github.com/wpilibsuite/allwpilib/tree/main/sysid">https://github.com/wpilibsuite/allwpilib/tree/main/sysid</a>
   */
  static constexpr LinearSystem<2, 1, 2> SingleJointedArmFromSysId(
      decltype(1_V / 1_rad_per_s) kV, decltype(1_V / 1_rad_per_s_sq) kA) {
    if (kV < decltype(kV){0}) {
      throw std::domain_error("Kv must be greater than or equal to zero.");
    }
    if (kA <= decltype(kA){0}) {
      throw std::domain_error("Ka must be greater than zero.");
    }

    Matrixd<2, 2> A{{0.0, 1.0}, {0.0, -kV.value() / kA.value()}};
    Matrixd<2, 1> B{{0.0}, {1.0 / kA.value()}};
    Matrixd<2, 2> C{{1.0, 0.0}, {0.0, 1.0}};
    Matrixd<2, 1> D{{0.0}, {0.0}};

    return LinearSystem<2, 1, 2>(A, B, C, D);
  }

  /**
   * Creates a differential drive state-space model from physical constants.
   *
   * The states are [left velocity, right velocity], the inputs are [left
   * voltage, right voltage], and the outputs are [left velocity, right
   * velocity].
   *
   * @param motor The motor (or gearbox) driving the drivetrain.
   * @param mass The mass of the robot in kilograms.
   * @param r The radius of the wheels in meters.
   * @param rb The radius of the base (half of the trackwidth), in meters.
   * @param J The moment of inertia of the robot.
   * @param gearing Gear ratio from motor to wheel (greater than 1 is a
   *     reduction).
   * @throws std::domain_error if mass <= 0, r <= 0, rb <= 0, J <= 0, or
   *         gearing <= 0.
   */
  static constexpr LinearSystem<2, 2, 2> DifferentialDriveFromPhysicalConstants(
      const DCMotor& motor, wpi::units::kilogram_t mass, wpi::units::meter_t r,
      wpi::units::meter_t rb, wpi::units::kilogram_square_meter_t J,
      double gearing) {
    if (mass <= 0_kg) {
      throw std::domain_error("mass must be greater than zero.");
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
    if (gearing <= 0.0) {
      throw std::domain_error("gearing must be greater than zero.");
    }

    auto C1 = -gcem::pow(gearing, 2) * motor.Kt /
              (motor.Kv * motor.R * wpi::units::math::pow<2>(r));
    auto C2 = gearing * motor.Kt / (motor.R * r);

    Matrixd<2, 2> A{
        {((1 / mass + wpi::units::math::pow<2>(rb) / J) * C1).value(),
         ((1 / mass - wpi::units::math::pow<2>(rb) / J) * C1).value()},
        {((1 / mass - wpi::units::math::pow<2>(rb) / J) * C1).value(),
         ((1 / mass + wpi::units::math::pow<2>(rb) / J) * C1).value()}};
    Matrixd<2, 2> B{
        {((1 / mass + wpi::units::math::pow<2>(rb) / J) * C2).value(),
         ((1 / mass - wpi::units::math::pow<2>(rb) / J) * C2).value()},
        {((1 / mass - wpi::units::math::pow<2>(rb) / J) * C2).value(),
         ((1 / mass + wpi::units::math::pow<2>(rb) / J) * C2).value()}};
    Matrixd<2, 2> C{{1.0, 0.0}, {0.0, 1.0}};
    Matrixd<2, 2> D{{0.0, 0.0}, {0.0, 0.0}};

    return LinearSystem<2, 2, 2>(A, B, C, D);
  }

  /**
   * Creates a differential drive state-space model from SysId constants kᵥ and
   * kₐ in both linear {(V/(m/s), (V/(m/s²))} and angular {(V/(rad/s),
   * (V/(rad/s²))} cases.
   *
   * The states are [left velocity, right velocity], the inputs are [left
   * voltage, right voltage], and the outputs are [left velocity, right
   * velocity].
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
   * href="https://github.com/wpilibsuite/allwpilib/tree/main/sysid">https://github.com/wpilibsuite/allwpilib/tree/main/sysid</a>
   */
  static constexpr LinearSystem<2, 2, 2> DifferentialDriveFromSysId(
      decltype(1_V / 1_mps) kVLinear, decltype(1_V / 1_mps_sq) kALinear,
      decltype(1_V / 1_mps) kVAngular, decltype(1_V / 1_mps_sq) kAAngular) {
    if (kVLinear <= decltype(kVLinear){0}) {
      throw std::domain_error("Kv,linear must be greater than zero.");
    }
    if (kALinear <= decltype(kALinear){0}) {
      throw std::domain_error("Ka,linear must be greater than zero.");
    }
    if (kVAngular <= decltype(kVAngular){0}) {
      throw std::domain_error("Kv,angular must be greater than zero.");
    }
    if (kAAngular <= decltype(kAAngular){0}) {
      throw std::domain_error("Ka,angular must be greater than zero.");
    }

    double A1 = -0.5 * (kVLinear.value() / kALinear.value() +
                        kVAngular.value() / kAAngular.value());
    double A2 = -0.5 * (kVLinear.value() / kALinear.value() -
                        kVAngular.value() / kAAngular.value());
    double B1 = 0.5 / kALinear.value() + 0.5 / kAAngular.value();
    double B2 = 0.5 / kALinear.value() - 0.5 / kAAngular.value();

    Matrixd<2, 2> A{{A1, A2}, {A2, A1}};
    Matrixd<2, 2> B{{B1, B2}, {B2, B1}};
    Matrixd<2, 2> C{{1.0, 0.0}, {0.0, 1.0}};
    Matrixd<2, 2> D{{0.0, 0.0}, {0.0, 0.0}};

    return LinearSystem<2, 2, 2>(A, B, C, D);
  }

  /**
   * Creates a differential drive state-space model from SysId constants kᵥ and
   * kₐ in both linear {(V/(m/s), (V/(m/s²))} and angular {(V/(rad/s),
   * (V/(rad/s²))} cases.
   *
   * The states are [left velocity, right velocity], the inputs are [left
   * voltage, right voltage], and the outputs are [left velocity, right
   * velocity].
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
   * href="https://github.com/wpilibsuite/allwpilib/tree/main/sysid">https://github.com/wpilibsuite/allwpilib/tree/main/sysid</a>
   */
  static constexpr LinearSystem<2, 2, 2> DifferentialDriveFromSysId(
      decltype(1_V / 1_mps) kVLinear, decltype(1_V / 1_mps_sq) kALinear,
      decltype(1_V / 1_rad_per_s) kVAngular,
      decltype(1_V / 1_rad_per_s_sq) kAAngular,
      wpi::units::meter_t trackwidth) {
    if (kVLinear <= decltype(kVLinear){0}) {
      throw std::domain_error("Kv,linear must be greater than zero.");
    }
    if (kALinear <= decltype(kALinear){0}) {
      throw std::domain_error("Ka,linear must be greater than zero.");
    }
    if (kVAngular <= decltype(kVAngular){0}) {
      throw std::domain_error("Kv,angular must be greater than zero.");
    }
    if (kAAngular <= decltype(kAAngular){0}) {
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
    return DifferentialDriveFromSysId(kVLinear, kALinear,
                                      kVAngular * 2.0 / trackwidth * 1_rad,
                                      kAAngular * 2.0 / trackwidth * 1_rad);
  }
};

}  // namespace wpi::math
