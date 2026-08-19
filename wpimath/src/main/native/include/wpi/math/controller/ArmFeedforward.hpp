// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/math/util/MathShared.hpp"
#include "wpi/units/angle.hpp"
#include "wpi/units/angular_acceleration.hpp"
#include "wpi/units/angular_velocity.hpp"
#include "wpi/units/core.hpp"
#include "wpi/units/time.hpp"
#include "wpi/units/voltage.hpp"
#include "wpi/util/MathExtras.hpp"
#include "wpi/util/SymbolExports.hpp"

namespace wpi::math {
/**
 * A helper class that computes feedforward outputs for a simple arm (modeled as
 * a motor acting against the force of gravity on a beam suspended at an angle).
 */
class WPILIB_DLLEXPORT ArmFeedforward {
 public:
  using Angle = wpi::units::radians_;
  using Velocity = wpi::units::radians_per_second_;
  using Acceleration = wpi::units::radians_per_second_squared_;
  using kv_unit = wpi::units::compound_conversion_factor<
      wpi::units::volts_, wpi::units::inverse<wpi::units::radians_per_second_>>;
  using ka_unit =
      wpi::units::compound_conversion_factor<wpi::units::volts_,
                                             wpi::units::inverse<Acceleration>>;

  /**
   * Creates a new ArmFeedforward with the specified gains.
   *
   * @param kS The static gain, in volts.
   * @param kG The gravity gain, in volts.
   * @param kV The velocity gain, in volt seconds per radian.
   * @param kA The acceleration gain, in volt seconds² per radian.
   * @param dt The period in seconds.
   * @throws IllegalArgumentException for kv &lt; zero.
   * @throws IllegalArgumentException for ka &lt; zero.
   * @throws IllegalArgumentException for period &le; zero.
   */
  constexpr ArmFeedforward(
      wpi::units::volts<> kS, wpi::units::volts<> kG,
      wpi::units::unit<kv_unit> kV,
      wpi::units::unit<ka_unit> kA = wpi::units::unit<ka_unit>(0),
      wpi::units::seconds<> dt = 20_ms)
      : kS(kS), kG(kG), kV(kV), kA(kA), m_dt(dt) {
    if (kV.value() < 0) {
      wpi::math::MathSharedStore::ReportError(
          "kV must be a non-negative number, got {}!", kV.value());
      this->kV = wpi::units::unit<kv_unit>{0};
      wpi::math::MathSharedStore::ReportWarning("kV defaulted to 0.");
    }
    if (kA.value() < 0) {
      wpi::math::MathSharedStore::ReportError(
          "kA must be a non-negative number, got {}!", kA.value());
      this->kA = wpi::units::unit<ka_unit>{0};
      wpi::math::MathSharedStore::ReportWarning("kA defaulted to 0;");
    }
    if (dt <= 0_ms) {
      wpi::math::MathSharedStore::ReportError(
          "period must be a positive number, got {}!", dt.value());
      this->m_dt = 20_ms;
      wpi::math::MathSharedStore::ReportWarning("period defaulted to 20 ms.");
    }
  }

  /**
   * Calculates the feedforward from the gains and reference assuming discrete
   * control. Use this method when the velocity does not change.
   *
   * @param currentAngle The current angle. This angle should be measured from
   * the horizontal (i.e. if the provided angle is 0, the arm should be parallel
   * to the floor). If your encoder does not follow this convention, an offset
   * should be added.
   * @param currentVelocity The current velocity.
   * @return The computed feedforward in volts.
   */
  constexpr wpi::units::volts<> Calculate(
      wpi::units::unit<Angle> currentAngle,
      wpi::units::unit<Velocity> currentVelocity) const {
    return kS * wpi::util::sgn(currentVelocity) +
           kG * wpi::units::cos(currentAngle) + kV * currentVelocity;
  }

  /**
   * Calculates the feedforward from the gains and references assuming discrete
   * control.
   *
   * @param currentAngle The current angle. This angle should be measured from
   * the horizontal (i.e. if the provided angle is 0, the arm should be parallel
   * to the floor). If your encoder does not follow this convention, an offset
   * should be added.
   * @param currentVelocity The current velocity.
   * @param nextVelocity    The next velocity.
   * @return The computed feedforward in volts.
   */
  wpi::units::volts<> Calculate(wpi::units::unit<Angle> currentAngle,
                                wpi::units::unit<Velocity> currentVelocity,
                                wpi::units::unit<Velocity> nextVelocity) const;

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
  constexpr wpi::units::unit<Velocity> MaxAchievableVelocity(
      wpi::units::volts<> maxVoltage, wpi::units::unit<Angle> angle,
      wpi::units::unit<Acceleration> acceleration) {
    // Assume max velocity is positive
    return (maxVoltage - kS - kG * wpi::units::cos(angle) - kA * acceleration) /
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
  constexpr wpi::units::unit<Velocity> MinAchievableVelocity(
      wpi::units::volts<> maxVoltage, wpi::units::unit<Angle> angle,
      wpi::units::unit<Acceleration> acceleration) {
    // Assume min velocity is negative, ks flips sign
    return (-maxVoltage + kS - kG * wpi::units::cos(angle) -
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
  constexpr wpi::units::unit<Acceleration> MaxAchievableAcceleration(
      wpi::units::volts<> maxVoltage, wpi::units::unit<Angle> angle,
      wpi::units::unit<Velocity> velocity) {
    return (maxVoltage - kS * wpi::util::sgn(velocity) -
            kG * wpi::units::cos(angle) - kV * velocity) /
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
  constexpr wpi::units::unit<Acceleration> MinAchievableAcceleration(
      wpi::units::volts<> maxVoltage, wpi::units::unit<Angle> angle,
      wpi::units::unit<Velocity> velocity) {
    return MaxAchievableAcceleration(-maxVoltage, angle, velocity);
  }

  /**
   * Sets the static gain.
   *
   * This setter is intended for online tuning only. Feedforward gains are
   * assumed constant, so gain scheduling means the system was not correctly
   * modeled.
   *
   * @param kS The static gain.
   */
  constexpr void SetKs(wpi::units::volts<> kS) { this->kS = kS; }

  /**
   * Sets the gravity gain.
   *
   * This setter is intended for online tuning only. Feedforward gains are
   * assumed constant, so gain scheduling means the system was not correctly
   * modeled.
   *
   * @param kG The gravity gain.
   */
  constexpr void SetKg(wpi::units::volts<> kG) { this->kG = kG; }

  /**
   * Sets the velocity gain.
   *
   * This setter is intended for online tuning only. Feedforward gains are
   * assumed constant, so gain scheduling means the system was not correctly
   * modeled.
   *
   * @param kV The velocity gain.
   */
  constexpr void SetKv(wpi::units::unit<kv_unit> kV) { this->kV = kV; }

  /**
   * Sets the acceleration gain.
   *
   * This setter is intended for online tuning only. Feedforward gains are
   * assumed constant, so gain scheduling means the system was not correctly
   * modeled.
   *
   * @param kA The acceleration gain.
   */
  constexpr void SetKa(wpi::units::unit<ka_unit> kA) { this->kA = kA; }

  /**
   * Returns the static gain.
   *
   * @return The static gain.
   */
  constexpr wpi::units::volts<> GetKs() const { return kS; }

  /**
   * Returns the gravity gain.
   *
   * @return The gravity gain.
   */
  constexpr wpi::units::volts<> GetKg() const { return kG; }

  /**
   * Returns the velocity gain.
   *
   * @return The velocity gain.
   */
  constexpr wpi::units::unit<kv_unit> GetKv() const { return kV; }

  /**
   * Returns the acceleration gain.
   *
   * @return The acceleration gain.
   */
  constexpr wpi::units::unit<ka_unit> GetKa() const { return kA; }

 private:
  /// The static gain, in volts.
  wpi::units::volts<> kS;

  /// The gravity gain, in volts.
  wpi::units::volts<> kG;

  /// The velocity gain, in V/(rad/s)volt seconds per radian.
  wpi::units::unit<kv_unit> kV;

  /// The acceleration gain, in V/(rad/s²).
  wpi::units::unit<ka_unit> kA;

  /** The period. */
  wpi::units::seconds<> m_dt;
};
}  // namespace wpi::math

#include "wpi/math/controller/proto/ArmFeedforwardProto.hpp"
#include "wpi/math/controller/struct/ArmFeedforwardStruct.hpp"
