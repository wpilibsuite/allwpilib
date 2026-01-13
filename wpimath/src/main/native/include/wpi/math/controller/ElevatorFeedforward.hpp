// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/math/controller/LinearPlantInversionFeedforward.hpp"
#include "wpi/math/linalg/EigenCore.hpp"
#include "wpi/math/util/MathShared.hpp"
#include "wpi/units/length.hpp"
#include "wpi/units/time.hpp"
#include "wpi/units/voltage.hpp"
#include "wpi/util/MathExtras.hpp"

namespace wpi::math {
/**
 * A helper class that computes feedforward outputs for a simple elevator
 * (modeled as a motor acting against the force of gravity).
 */
class ElevatorFeedforward {
 public:
  using Distance = wpi::units::meters;
  using Velocity =
      wpi::units::compound_unit<Distance,
                                wpi::units::inverse<wpi::units::seconds>>;
  using Acceleration =
      wpi::units::compound_unit<Velocity,
                                wpi::units::inverse<wpi::units::seconds>>;
  using kv_unit = wpi::units::compound_unit<wpi::units::volts,
                                            wpi::units::inverse<Velocity>>;
  using ka_unit = wpi::units::compound_unit<wpi::units::volts,
                                            wpi::units::inverse<Acceleration>>;

  /**
   * Creates a new ElevatorFeedforward with the specified gains.
   *
   * @param kS The static gain, in volts.
   * @param kG The gravity gain, in volts.
   * @param kV The velocity gain, in volt seconds per distance.
   * @param kA The acceleration gain, in volt seconds² per distance.
   * @param dt The period in seconds.
   * @throws IllegalArgumentException for kv &lt; zero.
   * @throws IllegalArgumentException for ka &lt; zero.
   * @throws IllegalArgumentException for period &le; zero.
   */
  constexpr ElevatorFeedforward(
      wpi::units::volt_t kS, wpi::units::volt_t kG,
      wpi::units::unit_t<kv_unit> kV,
      wpi::units::unit_t<ka_unit> kA = wpi::units::unit_t<ka_unit>(0),
      wpi::units::second_t dt = 20_ms)
      : kS(kS), kG(kG), kV(kV), kA(kA), m_dt(dt) {
    if (kV.value() < 0) {
      wpi::math::MathSharedStore::ReportError(
          "kV must be a non-negative number, got {}!", kV.value());
      this->kV = wpi::units::unit_t<kv_unit>{0};
      wpi::math::MathSharedStore::ReportWarning("kV defaulted to 0.");
    }
    if (kA.value() < 0) {
      wpi::math::MathSharedStore::ReportError(
          "kA must be a non-negative number, got {}!", kA.value());
      this->kA = wpi::units::unit_t<ka_unit>{0};
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
   * Calculates the feedforward from the gains and setpoint assuming discrete
   * control. Use this method when the setpoint does not change.
   *
   * @param currentVelocity The velocity setpoint.
   * @return The computed feedforward, in volts.
   */
  constexpr wpi::units::volt_t Calculate(
      wpi::units::unit_t<Velocity> currentVelocity) const {
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
   * @return The computed feedforward, in volts.
   */
  constexpr wpi::units::volt_t Calculate(
      wpi::units::unit_t<Velocity> currentVelocity,
      wpi::units::unit_t<Velocity> nextVelocity) const {
    // See wpimath/algorithms.md#Elevator_feedforward for derivation
    if (kA < decltype(kA)(1e-9)) {
      return kS * wpi::util::sgn(nextVelocity) + kG + kV * nextVelocity;
    } else {
      double A = -kV.value() / kA.value();
      double B = 1.0 / kA.value();
      double A_d = gcem::exp(A * m_dt.value());
      double B_d = A > -1e-9 ? B * m_dt.value() : 1.0 / A * (A_d - 1.0) * B;
      return kG + kS * wpi::util::sgn(currentVelocity) +
             wpi::units::volt_t{
                 1.0 / B_d *
                 (nextVelocity.value() - A_d * currentVelocity.value())};
    }
  }

  // Rearranging the main equation from the calculate() method yields the
  // formulas for the methods below:

  /**
   * Calculates the maximum achievable velocity given a maximum voltage supply
   * and an acceleration.  Useful for ensuring that velocity and
   * acceleration constraints for a trapezoidal profile are simultaneously
   * achievable - enter the acceleration constraint, and this will give you
   * a simultaneously-achievable velocity constraint.
   *
   * @param maxVoltage The maximum voltage that can be supplied to the elevator.
   * @param acceleration The acceleration of the elevator.
   * @return The maximum possible velocity at the given acceleration.
   */
  constexpr wpi::units::unit_t<Velocity> MaxAchievableVelocity(
      wpi::units::volt_t maxVoltage,
      wpi::units::unit_t<Acceleration> acceleration) {
    // Assume max velocity is positive
    return (maxVoltage - kS - kG - kA * acceleration) / kV;
  }

  /**
   * Calculates the minimum achievable velocity given a maximum voltage supply
   * and an acceleration.  Useful for ensuring that velocity and
   * acceleration constraints for a trapezoidal profile are simultaneously
   * achievable - enter the acceleration constraint, and this will give you
   * a simultaneously-achievable velocity constraint.
   *
   * @param maxVoltage The maximum voltage that can be supplied to the elevator.
   * @param acceleration The acceleration of the elevator.
   * @return The minimum possible velocity at the given acceleration.
   */
  constexpr wpi::units::unit_t<Velocity> MinAchievableVelocity(
      wpi::units::volt_t maxVoltage,
      wpi::units::unit_t<Acceleration> acceleration) {
    // Assume min velocity is negative, ks flips sign
    return (-maxVoltage + kS - kG - kA * acceleration) / kV;
  }

  /**
   * Calculates the maximum achievable acceleration given a maximum voltage
   * supply and a velocity. Useful for ensuring that velocity and
   * acceleration constraints for a trapezoidal profile are simultaneously
   * achievable - enter the velocity constraint, and this will give you
   * a simultaneously-achievable acceleration constraint.
   *
   * @param maxVoltage The maximum voltage that can be supplied to the elevator.
   * @param velocity The velocity of the elevator.
   * @return The maximum possible acceleration at the given velocity.
   */
  constexpr wpi::units::unit_t<Acceleration> MaxAchievableAcceleration(
      wpi::units::volt_t maxVoltage, wpi::units::unit_t<Velocity> velocity) {
    return (maxVoltage - kS * wpi::util::sgn(velocity) - kG - kV * velocity) /
           kA;
  }

  /**
   * Calculates the minimum achievable acceleration given a maximum voltage
   * supply and a velocity. Useful for ensuring that velocity and
   * acceleration constraints for a trapezoidal profile are simultaneously
   * achievable - enter the velocity constraint, and this will give you
   * a simultaneously-achievable acceleration constraint.
   *
   * @param maxVoltage The maximum voltage that can be supplied to the elevator.
   * @param velocity The velocity of the elevator.
   * @return The minimum possible acceleration at the given velocity.
   */
  constexpr wpi::units::unit_t<Acceleration> MinAchievableAcceleration(
      wpi::units::volt_t maxVoltage, wpi::units::unit_t<Velocity> velocity) {
    return MaxAchievableAcceleration(-maxVoltage, velocity);
  }

  /**
   * Sets the static gain.
   *
   * @param kS The static gain.
   */
  constexpr void SetKs(wpi::units::volt_t kS) { this->kS = kS; }

  /**
   * Sets the gravity gain.
   *
   * @param kG The gravity gain.
   */
  constexpr void SetKg(wpi::units::volt_t kG) { this->kG = kG; }

  /**
   * Sets the velocity gain.
   *
   * @param kV The velocity gain.
   */
  constexpr void SetKv(wpi::units::unit_t<kv_unit> kV) { this->kV = kV; }

  /**
   * Sets the acceleration gain.
   *
   * @param kA The acceleration gain.
   */
  constexpr void SetKa(wpi::units::unit_t<ka_unit> kA) { this->kA = kA; }

  /**
   * Returns the static gain.
   *
   * @return The static gain.
   */
  constexpr wpi::units::volt_t GetKs() const { return kS; }

  /**
   * Returns the gravity gain.
   *
   * @return The gravity gain.
   */
  constexpr wpi::units::volt_t GetKg() const { return kG; }

  /**
   * Returns the velocity gain.
   *
   * @return The velocity gain.
   */
  constexpr wpi::units::unit_t<kv_unit> GetKv() const { return kV; }

  /**
   * Returns the acceleration gain.
   *
   * @return The acceleration gain.
   */
  constexpr wpi::units::unit_t<ka_unit> GetKa() const { return kA; }

 private:
  /// The static gain.
  wpi::units::volt_t kS;

  /// The gravity gain.
  wpi::units::volt_t kG;

  /// The velocity gain.
  wpi::units::unit_t<kv_unit> kV;

  /// The acceleration gain.
  wpi::units::unit_t<ka_unit> kA;

  /** The period. */
  wpi::units::second_t m_dt;
};
}  // namespace wpi::math

#include "wpi/math/controller/proto/ElevatorFeedforwardProto.hpp"
#include "wpi/math/controller/struct/ElevatorFeedforwardStruct.hpp"
