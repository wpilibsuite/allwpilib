// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/MathExtras.h>

#include "frc/EigenCore.h"
#include "frc/controller/LinearPlantInversionFeedforward.h"
#include "frc/system/plant/LinearSystemId.h"
#include "units/current.h"
#include "units/length.h"
#include "units/time.h"
#include "units/voltage.h"
#include "wpimath/MathShared.h"

namespace frc {
/**
 * A helper class that computes feedforward outputs for a simple elevator
 * (modeled as a motor acting against the force of gravity).
 */

template <class Input>
  requires(units::voltage_unit<Input> || units::current_unit<Input>) 
class ElevatorFeedforward {
 public:
  using Distance = units::meters;
  using Velocity =
      units::compound_unit<Distance, units::inverse<units::seconds>>;
  using Acceleration =
      units::compound_unit<Velocity, units::inverse<units::seconds>>;
  using kv_unit = units::compound_unit<Input, units::inverse<Velocity>>;
  using ka_unit =
      units::compound_unit<Input, units::inverse<Acceleration>>;

  /**
   * Creates a new ElevatorFeedforward with the specified gains.
   *
   * @param kS The static gain, in input units (e.g., volts or amperes).
   * @param kG The gravity gain, in input units (e.g., volts or amperes).
   * @param kV The velocity gain, in input units per (distance/second).
   * @param kA The acceleration gain, in input units per (distance/second²).
   * @param dt The period in seconds.
   * @throws IllegalArgumentException for kv &lt; zero.
   * @throws IllegalArgumentException for ka &lt; zero.
   * @throws IllegalArgumentException for period &le; zero.
   */
  constexpr ElevatorFeedforward(
      units::unit_t<Input>, units::unit_t<Input> kG, units::unit_t<kv_unit> kV,
      units::unit_t<ka_unit> kA = units::unit_t<ka_unit>(0),
      units::second_t dt = 20_ms)
      : kS(kS), kG(kG), kV(kV), kA(kA), m_dt(dt) {
    if (kV.value() < 0) {
      wpi::math::MathSharedStore::ReportError(
          "kV must be a non-negative number, got {}!", kV.value());
      this->kV = units::unit_t<kv_unit>{0};
      wpi::math::MathSharedStore::ReportWarning("kV defaulted to 0.");
    }
    if (kA.value() < 0) {
      wpi::math::MathSharedStore::ReportError(
          "kA must be a non-negative number, got {}!", kA.value());
      this->kA = units::unit_t<ka_unit>{0};
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
   * Calculates the feedforward from the gains and setpoints assuming continuous
   * control.
   *
   * @param velocity     The velocity setpoint.
   * @param acceleration The acceleration setpoint.
   * @return The computed feedforward, in input units (e.g., volts or amperes).
   * @deprecated Use the current/next velocity overload instead.
   */
  [[deprecated("Use the current/next velocity overload instead.")]]
  constexpr units::unit_t<Input> Calculate(
      units::unit_t<Velocity> velocity,
      units::unit_t<Acceleration> acceleration) const {
    return kS * wpi::sgn(velocity) + kG + kV * velocity + kA * acceleration;
  }

  /**
   * Calculates the feedforward from the gains and setpoints assuming continuous
   * control.
   *
   * @param currentVelocity The current velocity setpoint.
   * @param nextVelocity    The next velocity setpoint.
   * @param dt              Time between velocity setpoints in seconds.
   * @return The computed feedforward, in input units (e.g., volts or amperes).
   */
  [[deprecated("Use the current/next velocity overload instead.")]]
  units::unit_t<Input> Calculate(units::unit_t<Velocity> currentVelocity,
                          units::unit_t<Velocity> nextVelocity,
                          units::second_t dt) const {
    // See wpimath/algorithms.md#Elevator_feedforward for derivation
    auto plant = LinearSystemId::IdentifyVelocitySystem<Distance>(kV, kA);
    LinearPlantInversionFeedforward<1, 1> feedforward{plant, dt};

    Vectord<1> r{{currentVelocity.value()}};
    Vectord<1> nextR{{nextVelocity.value()}};

    return kG + kS * wpi::sgn(currentVelocity.value()) +
           units::volt_t{feedforward.Calculate(r, nextR)(0)};
  }

  /**
   * Calculates the feedforward from the gains and setpoint assuming discrete
   * control. Use this method when the setpoint does not change.
   *
   * @param currentVelocity The velocity setpoint.
   * @return The computed feedforward, in input units (e.g., volts or amperes).
   */
  constexpr units::unit_t<Input>Calculate(
      units::unit_t<Velocity> currentVelocity) const {
    return Calculate(currentVelocity, currentVelocity);
  }

  /**
   * Calculates the feedforward from the gains and setpoints assuming discrete
   * control.
   *
   * <p>Note this method is inaccurate when the velocity crosses 0.
   *
   * @param currentVelocity The current velocity setpoint.
   * @param nextVelocity    The next velocity setpoint.
   * @return The computed feedforward, in input units (e.g., volts or amperes).
   */
  constexpr units::unit_t<Input> Calculate(
      units::unit_t<Velocity> currentVelocity,
      units::unit_t<Velocity> nextVelocity) const {
    // See wpimath/algorithms.md#Elevator_feedforward for derivation
    if (kA == decltype(kA)(0)) {
      return kS * wpi::sgn(nextVelocity) + kG + kV * nextVelocity;
    } else {
      double A = -kV.value() / kA.value();
      double B = 1.0 / kA.value();
      double A_d = gcem::exp(A * m_dt.value());
      double B_d = 1.0 / A * (A_d - 1.0) * B;
      return kG + kS * wpi::sgn(currentVelocity) +
             units::unit_t<Input>{
                 1.0 / B_d *
                 (nextVelocity.value() - A_d * currentVelocity.value())};
    }
  }

  // Rearranging the main equation from the calculate() method yields the
  // formulas for the methods below:

  /**
   * Calculates the maximum achievable velocity given a maximum input (e.g.,
   * volts or amperes) and an acceleration.  Useful for ensuring that velocity and
   * acceleration constraints for a trapezoidal profile are simultaneously
   * achievable - enter the acceleration constraint, and this will give you
   * a simultaneously-achievable velocity constraint.
   *
   * @param maxInput The maximum input (e.g., volts or amperes) that can be
   * supplied to the motor.
   * @param acceleration The acceleration of the elevator.
   * @return The maximum possible velocity at the given acceleration.
   */
  constexpr units::unit_t<Velocity> MaxAchievableVelocity(
      units::unit_t<Input> maxInput, units::unit_t<Acceleration> acceleration) {
    // Assume max velocity is positive
    return (maxInput - kS - kG - kA * acceleration) / kV;
  }

  /**
   * Calculates the minimum achievable velocity given a maximum input (e.g.,
   * volts or amperes) and an acceleration.  Useful for ensuring that velocity and
   * acceleration constraints for a trapezoidal profile are simultaneously
   * achievable - enter the acceleration constraint, and this will give you
   * a simultaneously-achievable velocity constraint.
   *
   * @param maxInput The maximum input (e.g., volts or amperes) that can be
   * supplied to the motor.
   * @param acceleration The acceleration of the elevator.
   * @return The minimum possible velocity at the given acceleration.
   */
  constexpr units::unit_t<Velocity> MinAchievableVelocity(
      units::unit_t<Input> maxInput, units::unit_t<Acceleration> acceleration) {
    // Assume min velocity is negative, ks flips sign
    return (-maxInput + kS - kG - kA * acceleration) / kV;
  }

  /**
   * Calculates the maximum achievable velocity given a maximum input (e.g.,
   * volts or amperes) and an acceleration.  Useful for ensuring that velocity and
   * acceleration constraints for a trapezoidal profile are simultaneously
   * achievable - enter the velocity constraint, and this will give you
   * a simultaneously-achievable acceleration constraint.
   *
   * @param maxInput The maximum input (e.g., volts or amperes) that can be
   * supplied to the motor.
   * @param velocity The velocity of the elevator.
   * @return The maximum possible acceleration at the given velocity.
   */
  constexpr units::unit_t<Acceleration> MaxAchievableAcceleration(
      units::unit_t<Input> maxInput, units::unit_t<Velocity> velocity) {
    return (maxInput - kS * wpi::sgn(velocity) - kG - kV * velocity) / kA;
  }

  /**
   * Calculates the minimum achievable velocity given a maximum input (e.g.,
   * volts or amperes) and an acceleration.  Useful for ensuring that velocity and
   * acceleration constraints for a trapezoidal profile are simultaneously
   * achievable - enter the velocity constraint, and this will give you
   * a simultaneously-achievable acceleration constraint.
   *
   * @param maxInput The maximum input (e.g., volts or amperes) that can be
   * supplied to the motor.   
   * @param velocity The velocity of the elevator.
   * @return The minimum possible acceleration at the given velocity.
   */
  constexpr units::unit_t<Acceleration> MinAchievableAcceleration(
      units::unit_t<Input> maxInput, units::unit_t<Velocity> velocity) {
    return MaxAchievableAcceleration(-maxInput, velocity);
  }

  /**
   * Sets the static gain.
   *
   * @param kS The static gain.
   */
  constexpr void SetKs(units::unit_t<Input> kS) { this->kS = kS; }

  /**
   * Sets the gravity gain.
   *
   * @param kG The gravity gain.
   */
  constexpr void SetKg(units::unit_t<Input> kG) { this->kG = kG; }

  /**
   * Sets the velocity gain.
   *
   * @param kV The velocity gain.
   */
  constexpr void SetKv(units::unit_t<kv_unit> kV) { this->kV = kV; }

  /**
   * Sets the acceleration gain.
   *
   * @param kA The acceleration gain.
   */
  constexpr void SetKa(units::unit_t<ka_unit> kA) { this->kA = kA; }

  /**
   * Returns the static gain.
   *
   * @return The static gain.
   */
  constexpr units::unit_t<Input> GetKs() const { return kS; }

  /**
   * Returns the gravity gain.
   *
   * @return The gravity gain.
   */
  constexpr units::unit_t<Input> GetKg() const { return kG; }

  /**
   * Returns the velocity gain.
   *
   * @return The velocity gain.
   */
  constexpr units::unit_t<kv_unit> GetKv() const { return kV; }

  /**
   * Returns the acceleration gain.
   *
   * @return The acceleration gain.
   */
  constexpr units::unit_t<ka_unit> GetKa() const { return kA; }

 private:
  /// The static gain.
  units::unit_t<Input> kS;

  /// The gravity gain.
  units::unit_t<Input> kG;

  /// The velocity gain.
  units::unit_t<kv_unit> kV;

  /// The acceleration gain.
  units::unit_t<ka_unit> kA;

  /** The period. */
  units::second_t m_dt;
};
}  // namespace frc

#include "frc/controller/proto/ElevatorFeedforwardProto.h"
#include "frc/controller/struct/ElevatorFeedforwardStruct.h"
