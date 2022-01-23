// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/MathExtras.h>
#include <wpi/SymbolExports.h>

#include "frc/controller/SimpleMotorFeedforward.h"
#include "units/angle.h"
#include "units/angular_velocity.h"
#include "units/math.h"
#include "units/voltage.h"

namespace frc {
/**
 * A helper class that computes feedforward outputs for a simple arm (modeled as
 * a motor acting against the force of gravity on a beam suspended at an angle).
 */
class WPILIB_DLLEXPORT ArmFeedforward
    : public wpi::Sendable,
      public wpi::SendableHelper<ArmFeedforward> {
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

  ArmFeedforward() = default;

  /**
   * Creates a new ArmFeedforward with the specified gains.
   *
   * @param kS   The static gain, in volts.
   * @param kG The gravity gain, in volts.
   * @param kV   The velocity gain, in volt seconds per radian.
   * @param kA   The acceleration gain, in volt seconds^2 per radian.
   */
  ArmFeedforward(units::volt_t kS, units::volt_t kG, units::unit_t<kv_unit> kV,
                 units::unit_t<ka_unit> kA = units::unit_t<ka_unit>(0))
      : kG(kG), m_simpleFeedforward(kS, kV, kA) {}

  /**
   * Gets the most recent output voltage.
   *
   * @return Most recent output.
   */
  units::volt_t GetOutput() const {
    return m_simpleFeedforward.GetOutput() + kG * units::math::cos(m_angle);
  }

  /**
   * Returns the most recent angle of the arm, in radians.
   *
   * @return The position of the arm in radians.
   */
  units::radian_t GetAngle() const { return m_angle; }

  /**
   * Calculates the feedforward from the gains and setpoints.
   *
   * @param angle           The angle setpoint.
   * @param currentVelocity The current velocity setpoint.
   * @param nextVelocity    The next velocity setpoint.
   * @param dt              The time until the next velocity setpoint.
   * @return The computed feedforward, in volts.
   */
  units::volt_t Calculate(units::unit_t<Angle> angle,
                          units::unit_t<Velocity> currentVelocity,
                          units::unit_t<Velocity> nextVelocity,
                          units::second_t dt) {
    m_angle = angle;
    return m_simpleFeedforward.Calculate(currentVelocity, nextVelocity, dt) +
           kG * units::math::cos(angle);
  }

  /**
   * Calculates the feedforward from the gains and setpoints.
   *
   * @param angle        The angle setpoint.
   * @param velocity     The velocity setpoint.
   * @param acceleration The acceleration setpoint.
   * @return The computed feedforward, in volts.
   */
  units::volt_t Calculate(units::unit_t<Angle> angle,
                          units::unit_t<Velocity> velocity,
                          units::unit_t<Acceleration> acceleration =
                              units::unit_t<Acceleration>(0)) {
    return Calculate(angle, velocity, velocity + acceleration * 20_ms, 20_ms);
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
   * @param maxVoltage The maximum voltage that can be supplied to the arm.
   * @param angle The angle of the arm
   * @param acceleration The acceleration of the arm.
   * @return The maximum possible velocity at the given acceleration and angle.
   */
  units::unit_t<Velocity> MaxAchievableVelocity(
      units::volt_t maxVoltage, units::unit_t<Angle> angle,
      units::unit_t<Acceleration> acceleration) {
    // Assume max velocity is positive
    return m_simpleFeedforward.MaxAchievableVelocity(maxVoltage, acceleration) -
           kG * units::math::cos(angle) / m_simpleFeedforward.kV;
  }

  /**
   * Calculates the minimum achievable velocity given a maximum voltage supply,
   * a position, and an acceleration.  Useful for ensuring that velocity and
   * acceleration constraints for a trapezoidal profile are simultaneously
   * achievable - enter the acceleration constraint, and this will give you
   * a simultaneously-achievable velocity constraint.
   *
   * @param maxVoltage The maximum voltage that can be supplied to the arm.
   * @param angle The angle of the arm
   * @param acceleration The acceleration of the arm.
   * @return The minimum possible velocity at the given acceleration and angle.
   */
  units::unit_t<Velocity> MinAchievableVelocity(
      units::volt_t maxVoltage, units::unit_t<Angle> angle,
      units::unit_t<Acceleration> acceleration) {
    // Assume min velocity is negative, ks flips sign
    return m_simpleFeedforward.MinAchievableVelocity(maxVoltage, acceleration) -
           kG * units::math::cos(angle) / m_simpleFeedforward.kV;
  }

  /**
   * Calculates the maximum achievable acceleration given a maximum voltage
   * supply, a position, and a velocity. Useful for ensuring that velocity and
   * acceleration constraints for a trapezoidal profile are simultaneously
   * achievable - enter the velocity constraint, and this will give you
   * a simultaneously-achievable acceleration constraint.
   *
   * @param maxVoltage The maximum voltage that can be supplied to the arm.
   * @param angle The angle of the arm
   * @param velocity The velocity of the arm.
   * @return The maximum possible acceleration at the given velocity and angle.
   */
  units::unit_t<Acceleration> MaxAchievableAcceleration(
      units::volt_t maxVoltage, units::unit_t<Angle> angle,
      units::unit_t<Velocity> velocity) {
    return m_simpleFeedforward.MaxAchievableAcceleration(maxVoltage, velocity) -
           kG * units::math::cos(angle) / m_simpleFeedforward.kA;
  }

  /**
   * Calculates the minimum achievable acceleration given a maximum voltage
   * supply, a position, and a velocity. Useful for ensuring that velocity and
   * acceleration constraints for a trapezoidal profile are simultaneously
   * achievable - enter the velocity constraint, and this will give you
   * a simultaneously-achievable acceleration constraint.
   *
   * @param maxVoltage The maximum voltage that can be supplied to the arm.
   * @param angle The angle of the arm
   * @param velocity The velocity of the arm.
   * @return The minimum possible acceleration at the given velocity and angle.
   */
  units::unit_t<Acceleration> MinAchievableAcceleration(
      units::volt_t maxVoltage, units::unit_t<Angle> angle,
      units::unit_t<Velocity> velocity) {
    return MaxAchievableAcceleration(-maxVoltage, angle, velocity);
  }

  void InitSendable(wpi::SendableBuilder& builder) override {
    m_simpleFeedforward.InitSendable(builder);
    builder.SetSmartDashboardType("ArmFeedforward");
    builder.AddDoubleProperty(
        "kG", [this] { return kG.value(); },
        [this](double kG) { this->kG = units::volt_t{kG}; });
    builder.AddDoubleProperty(
        "gravityVoltage",
        [this] { return (kG * units::math::cos(m_angle)).value(); }, nullptr);
    builder.AddDoubleProperty(
        "outputVoltage", [this] { return GetOutput().value(); }, nullptr);
  }

  SimpleMotorFeedforward<Angle> m_simpleFeedforward;
  units::volt_t kG{0};

 private:
  units::unit_t<Angle> m_angle;
};
}  // namespace frc
