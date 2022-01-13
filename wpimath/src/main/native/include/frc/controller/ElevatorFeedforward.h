// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/MathExtras.h>

#include "frc/controller/SimpleMotorFeedforward.h"
#include "units/time.h"
#include "units/voltage.h"

namespace frc {
/**
 * A helper class that computes feedforward outputs for a simple elevator
 * (modeled as a motor acting against the force of gravity).
 */
template <class Distance>
class WPILIB_DLLEXPORT ElevatorFeedforward
    : public wpi::Sendable,
      public wpi::SendableHelper<ElevatorFeedforward<Distance>> {
 public:
  using Velocity =
      units::compound_unit<Distance, units::inverse<units::seconds>>;
  using Acceleration =
      units::compound_unit<Velocity, units::inverse<units::seconds>>;
  using kv_unit = units::compound_unit<units::volts, units::inverse<Velocity>>;
  using ka_unit =
      units::compound_unit<units::volts, units::inverse<Acceleration>>;

  ElevatorFeedforward() = default;

  /**
   * Creates a new ElevatorFeedforward with the specified gains.
   *
   * @param kS The static gain, in volts.
   * @param kG The gravity gain, in volts.
   * @param kV The velocity gain, in volt seconds per distance.
   * @param kA The acceleration gain, in volt seconds^2 per distance.
   */
  ElevatorFeedforward(units::volt_t kS, units::volt_t kG,
                      units::unit_t<kv_unit> kV,
                      units::unit_t<ka_unit> kA = units::unit_t<ka_unit>(0))
      : kG(kG), m_simpleFeedforward(kS, kV, kA) {}

  /**
   * Gets the most recent output voltage.
   *
   * @return Most recent output.
   */
  units::volt_t GetOutput() const {
    return m_simpleFeedforward.GetOutput() + kG;
  }

  /**
   * Calculates the feedforward from the gains and setpoints.
   *
   * @param currentVelocity The current velocity setpoint.
   * @param nextVelocity    The next velocity setpoint.
   * @param dt              The time until the next velocity setpoint.
   * @return The computed feedforward, in volts.
   */
  units::volt_t Calculate(units::unit_t<Velocity> currentVelocity,
                          units::unit_t<Velocity> nextVelocity,
                          units::second_t dt) {
    return m_simpleFeedforward.Calculate(currentVelocity, nextVelocity, dt) +
           kG;
  }

  /**
   * Calculates the feedforward from the gains and setpoints.
   *
   * @param velocity     The velocity setpoint.
   * @param acceleration The acceleration setpoint.
   * @return The computed feedforward, in volts.
   */
  units::volt_t Calculate(units::unit_t<Velocity> velocity,
                          units::unit_t<Acceleration> acceleration =
                              units::unit_t<Acceleration>(0)) {
    return Calculate(velocity, velocity + acceleration * 20_ms, 20_ms);
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
  constexpr units::unit_t<Velocity> MaxAchievableVelocity(
      units::volt_t maxVoltage, units::unit_t<Acceleration> acceleration) {
    // Assume max velocity is positive
    return m_simpleFeedforward.MaxAchievableVelocity(maxVoltage, acceleration) -
           kG / m_simpleFeedforward.kV;
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
  constexpr units::unit_t<Velocity> MinAchievableVelocity(
      units::volt_t maxVoltage, units::unit_t<Acceleration> acceleration) {
    // Assume min velocity is negative, ks flips sign
    return m_simpleFeedforward.MinAchievableVelocity(maxVoltage, acceleration) -
           kG / m_simpleFeedforward.kV;
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
  constexpr units::unit_t<Acceleration> MaxAchievableAcceleration(
      units::volt_t maxVoltage, units::unit_t<Velocity> velocity) {
    return m_simpleFeedforward.MaxAchievableAcceleration(maxVoltage, velocity) -
           kG / m_simpleFeedforward.kA;
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
  constexpr units::unit_t<Acceleration> MinAchievableAcceleration(
      units::volt_t maxVoltage, units::unit_t<Velocity> velocity) {
    return MaxAchievableAcceleration(-maxVoltage, velocity);
  }

  void InitSendable(wpi::SendableBuilder& builder) override {
    m_simpleFeedforward.InitSendable(builder);
    builder.SetSmartDashboardType("ElevatorFeedforward");
    builder.AddDoubleProperty(
        "kG", [this] { return kG.value(); },
        [this](double kG) { this->kG = units::volt_t{kG}; });
    builder.AddDoubleProperty(
        "outputVoltage", [this] { return GetOutput().value(); }, nullptr);
  }

  units::volt_t kG{0};
  SimpleMotorFeedforward<Distance> m_simpleFeedforward;
};
}  // namespace frc
