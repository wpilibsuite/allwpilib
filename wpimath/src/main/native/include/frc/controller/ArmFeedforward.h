// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/MathExtras.h>
#include <wpi/SymbolExports.h>

#include "units/angle.h"
#include "units/angular_velocity.h"
#include "units/math.h"
#include "units/voltage.h"
#include "wpimath/MathShared.h"

namespace frc {
/**
 * A helper class that computes feedforward outputs for a simple arm (modeled as
 * a motor acting against the force of gravity on a beam suspended at an angle).
 */
class WPILIB_DLLEXPORT ArmFeedforward {
 public:
  using Angle = units::radians;
  using Velocity = units::radians_per_second;
  using Acceleration = units::compound_unit<units::radians_per_second,
                                            units::inverse<units::second>>;
  using kv_unit =
      units::compound_unit<units::volts,
                           units::inverse<units::radians_per_second>>;
  using ka_unit =
      units::compound_unit<units::volts, units::inverse<Acceleration>>;

  /**
   * Creates a new ArmFeedforward with the specified gains.
   *
   * @param kS The static gain, in volts.
   * @param kG The gravity gain, in volts.
   * @param kV The velocity gain, in volt seconds per radian.
   * @param kA The acceleration gain, in volt seconds² per radian.
   */
  constexpr ArmFeedforward(
      units::volt_t kS, units::volt_t kG, units::unit_t<kv_unit> kV,
      units::unit_t<ka_unit> kA = units::unit_t<ka_unit>(0))
      : kS(kS), kG(kG), kV(kV), kA(kA) {
    if (kV.value() < 0) {
      wpi::math::MathSharedStore::ReportError(
          "kV must be a non-negative number, got {}!", kV.value());
      kV = units::unit_t<kv_unit>{0};
      wpi::math::MathSharedStore::ReportWarning("kV defaulted to 0.");
    }
    if (kA.value() < 0) {
      wpi::math::MathSharedStore::ReportError(
          "kA must be a non-negative number, got {}!", kA.value());
      kA = units::unit_t<ka_unit>{0};
      wpi::math::MathSharedStore::ReportWarning("kA defaulted to 0;");
    }
  }

  /**
   * Calculates the feedforward from the gains and setpoints.
   *
   * @param angle        The angle setpoint, in radians. This angle should be
   *                     measured from the horizontal (i.e. if the provided
   *                     angle is 0, the arm should be parallel to the floor).
   *                     If your encoder does not follow this convention, an
   *                     offset should be added.
   * @param velocity     The velocity setpoint, in radians per second.
   * @param acceleration The acceleration setpoint, in radians per second².
   * @return The computed feedforward, in volts.
   */
  units::volt_t Calculate(units::unit_t<Angle> angle,
                          units::unit_t<Velocity> velocity,
                          units::unit_t<Acceleration> acceleration =
                              units::unit_t<Acceleration>(0)) const {
    return kS * wpi::sgn(velocity) + kG * units::math::cos(angle) +
           kV * velocity + kA * acceleration;
  }

  // Rearranging the main equation from the calculate() method yields the
  // formulas for the methods below:

  /**
   * Calculates the maximum achievable velocity given a maximum voltage supply,
   * a position, and an acceleration.  Useful for ensuring that velocity and
   * acceleration constraints for a trapezoidal profile are simultaneously
   * achievable - enter the acceleration constraint, and this will give you
   * a simultaneously-achievable velocity constraint.
   *
   * @param maxVoltage   The maximum voltage that can be supplied to the arm.
   * @param angle        The angle of the arm. This angle should be measured
   *                     from the horizontal (i.e. if the provided angle is 0,
   *                     the arm should be parallel to the floor). If your
   *                     encoder does not follow this convention, an offset
   *                     should be added.
   * @param acceleration The acceleration of the arm.
   * @return The maximum possible velocity at the given acceleration and angle.
   */
  units::unit_t<Velocity> MaxAchievableVelocity(
      units::volt_t maxVoltage, units::unit_t<Angle> angle,
      units::unit_t<Acceleration> acceleration) {
    // Assume max velocity is positive
    return (maxVoltage - kS - kG * units::math::cos(angle) -
            kA * acceleration) /
           kV;
  }

  /**
   * Calculates the minimum achievable velocity given a maximum voltage supply,
   * a position, and an acceleration.  Useful for ensuring that velocity and
   * acceleration constraints for a trapezoidal profile are simultaneously
   * achievable - enter the acceleration constraint, and this will give you
   * a simultaneously-achievable velocity constraint.
   *
   * @param maxVoltage   The maximum voltage that can be supplied to the arm.
   * @param angle        The angle of the arm. This angle should be measured
   *                     from the horizontal (i.e. if the provided angle is 0,
   *                     the arm should be parallel to the floor). If your
   *                     encoder does not follow this convention, an offset
   *                     should be added.
   * @param acceleration The acceleration of the arm.
   * @return The minimum possible velocity at the given acceleration and angle.
   */
  units::unit_t<Velocity> MinAchievableVelocity(
      units::volt_t maxVoltage, units::unit_t<Angle> angle,
      units::unit_t<Acceleration> acceleration) {
    // Assume min velocity is negative, ks flips sign
    return (-maxVoltage + kS - kG * units::math::cos(angle) -
            kA * acceleration) /
           kV;
  }

  /**
   * Calculates the maximum achievable acceleration given a maximum voltage
   * supply, a position, and a velocity. Useful for ensuring that velocity and
   * acceleration constraints for a trapezoidal profile are simultaneously
   * achievable - enter the velocity constraint, and this will give you
   * a simultaneously-achievable acceleration constraint.
   *
   * @param maxVoltage The maximum voltage that can be supplied to the arm.
   * @param angle      The angle of the arm. This angle should be measured
   *                   from the horizontal (i.e. if the provided angle is 0,
   *                   the arm should be parallel to the floor). If your
   *                   encoder does not follow this convention, an offset
   *                   should be added.
   * @param velocity   The velocity of the arm.
   * @return The maximum possible acceleration at the given velocity and angle.
   */
  units::unit_t<Acceleration> MaxAchievableAcceleration(
      units::volt_t maxVoltage, units::unit_t<Angle> angle,
      units::unit_t<Velocity> velocity) {
    return (maxVoltage - kS * wpi::sgn(velocity) -
            kG * units::math::cos(angle) - kV * velocity) /
           kA;
  }

  /**
   * Calculates the minimum achievable acceleration given a maximum voltage
   * supply, a position, and a velocity. Useful for ensuring that velocity and
   * acceleration constraints for a trapezoidal profile are simultaneously
   * achievable - enter the velocity constraint, and this will give you
   * a simultaneously-achievable acceleration constraint.
   *
   * @param maxVoltage The maximum voltage that can be supplied to the arm.
   * @param angle      The angle of the arm. This angle should be measured
   *                   from the horizontal (i.e. if the provided angle is 0,
   *                   the arm should be parallel to the floor). If your
   *                   encoder does not follow this convention, an offset
   *                   should be added.
   * @param velocity   The velocity of the arm.
   * @return The minimum possible acceleration at the given velocity and angle.
   */
  units::unit_t<Acceleration> MinAchievableAcceleration(
      units::volt_t maxVoltage, units::unit_t<Angle> angle,
      units::unit_t<Velocity> velocity) {
    return MaxAchievableAcceleration(-maxVoltage, angle, velocity);
  }

  /// The static gain, in volts.
  const units::volt_t kS;

  /// The gravity gain, in volts.
  const units::volt_t kG;

  /// The velocity gain, in volt seconds per radian.
  const units::unit_t<kv_unit> kV;

  /// The acceleration gain, in volt seconds² per radian.
  const units::unit_t<ka_unit> kA;
};
}  // namespace frc

#include "frc/controller/proto/ArmFeedforwardProto.h"
#include "frc/controller/struct/ArmFeedforwardStruct.h"
