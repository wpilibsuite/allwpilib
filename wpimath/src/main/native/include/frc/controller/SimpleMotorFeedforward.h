// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/MathExtras.h>
#include <wpi/sendable/Sendable.h>
#include <wpi/sendable/SendableBuilder.h>
#include <wpi/sendable/SendableHelper.h>

#include "Eigen/Core"
#include "frc/controller/LinearPlantInversionFeedforward.h"
#include "frc/system/plant/LinearSystemId.h"
#include "units/time.h"
#include "units/voltage.h"

namespace frc {
/**
 * A helper class that computes feedforward voltages for a simple
 * permanent-magnet DC motor.
 */
template <class Distance>
class WPILIB_DLLEXPORT SimpleMotorFeedforward
    : public wpi::Sendable,
      public wpi::SendableHelper<SimpleMotorFeedforward<Distance>> {
 public:
  using Velocity =
      units::compound_unit<Distance, units::inverse<units::seconds>>;
  using Acceleration =
      units::compound_unit<Velocity, units::inverse<units::seconds>>;
  using kv_unit = units::compound_unit<units::volts, units::inverse<Velocity>>;
  using ka_unit =
      units::compound_unit<units::volts, units::inverse<Acceleration>>;

  SimpleMotorFeedforward() = default;

  /**
   * Creates a new SimpleMotorFeedforward with the specified gains.
   *
   * @param kS The static gain, in volts.
   * @param kV The velocity gain, in volt seconds per distance.
   * @param kA The acceleration gain, in volt seconds^2 per distance.
   */
  SimpleMotorFeedforward(units::volt_t kS, units::unit_t<kv_unit> kV,
                         units::unit_t<ka_unit> kA = units::unit_t<ka_unit>(0))
      : kS(kS), kV(kV), kA(kA) {}

  /**
   * Gets the most recent output voltage.
   *
   * @return Most recent output.
   */
  units::volt_t GetOutput() const { return m_output; }

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
    m_velocity = currentVelocity;
    m_acceleration = (nextVelocity - currentVelocity) / dt;
    auto plant = LinearSystemId::IdentifyVelocitySystem<Distance>(kV, kA);
    LinearPlantInversionFeedforward<1, 1> feedforward(plant.A(), plant.B(), dt);

    Eigen::Vector<double, 1> r{currentVelocity.value()};
    Eigen::Vector<double, 1> nextR{nextVelocity.value()};

    m_output = kS * wpi::sgn(currentVelocity.value()) +
               units::volt_t{feedforward.Calculate(r, nextR)(0)};

    return m_output;
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
   * @param maxVoltage The maximum voltage that can be supplied to the motor.
   * @param acceleration The acceleration of the motor.
   * @return The maximum possible velocity at the given acceleration.
   */
  constexpr units::unit_t<Velocity> MaxAchievableVelocity(
      units::volt_t maxVoltage,
      units::unit_t<Acceleration> acceleration) const {
    // Assume max velocity is positive
    return (maxVoltage - kS - kA * acceleration) / kV;
  }

  /**
   * Calculates the minimum achievable velocity given a maximum voltage supply
   * and an acceleration.  Useful for ensuring that velocity and
   * acceleration constraints for a trapezoidal profile are simultaneously
   * achievable - enter the acceleration constraint, and this will give you
   * a simultaneously-achievable velocity constraint.
   *
   * @param maxVoltage The maximum voltage that can be supplied to the motor.
   * @param acceleration The acceleration of the motor.
   * @return The minimum possible velocity at the given acceleration.
   */
  constexpr units::unit_t<Velocity> MinAchievableVelocity(
      units::volt_t maxVoltage,
      units::unit_t<Acceleration> acceleration) const {
    // Assume min velocity is positive, ks flips sign
    return (-maxVoltage + kS - kA * acceleration) / kV;
  }

  /**
   * Calculates the maximum achievable acceleration given a maximum voltage
   * supply and a velocity. Useful for ensuring that velocity and
   * acceleration constraints for a trapezoidal profile are simultaneously
   * achievable - enter the velocity constraint, and this will give you
   * a simultaneously-achievable acceleration constraint.
   *
   * @param maxVoltage The maximum voltage that can be supplied to the motor.
   * @param velocity The velocity of the motor.
   * @return The maximum possible acceleration at the given velocity.
   */
  constexpr units::unit_t<Acceleration> MaxAchievableAcceleration(
      units::volt_t maxVoltage, units::unit_t<Velocity> velocity) const {
    return (maxVoltage - kS * wpi::sgn(velocity) - kV * velocity) / kA;
  }

  /**
   * Calculates the minimum achievable acceleration given a maximum voltage
   * supply and a velocity. Useful for ensuring that velocity and
   * acceleration constraints for a trapezoidal profile are simultaneously
   * achievable - enter the velocity constraint, and this will give you
   * a simultaneously-achievable acceleration constraint.
   *
   * @param maxVoltage The maximum voltage that can be supplied to the motor.
   * @param velocity The velocity of the motor.
   * @return The minimum possible acceleration at the given velocity.
   */
  constexpr units::unit_t<Acceleration> MinAchievableAcceleration(
      units::volt_t maxVoltage, units::unit_t<Velocity> velocity) const {
    return MaxAchievableAcceleration(-maxVoltage, velocity);
  }

  units::volt_t kS{0};
  units::unit_t<kv_unit> kV{0};
  units::unit_t<ka_unit> kA{0};

  void InitSendable(wpi::SendableBuilder& builder) override {
    builder.SetSmartDashboardType("SimpleMotorFeedforward");
    builder.AddDoubleProperty(
        "kS", [this] { return kS.value(); },
        [this](double kS) { this->kS = units::volt_t{kS}; });
    builder.AddDoubleProperty(
        "kV", [this] { return kV.value(); },
        [this](double kV) { this->kV = units::unit_t<kv_unit>{kV}; });
    builder.AddDoubleProperty(
        "kA", [this] { return kA.value(); },
        [this](double kS) { this->kA = units::unit_t<ka_unit>{kA}; });
    builder.AddDoubleProperty(
        "velocity", [this] { return m_velocity.value(); }, nullptr);
    builder.AddDoubleProperty(
        "velocityVoltage", [this] { return (m_velocity * kV).value(); },
        nullptr);
    builder.AddDoubleProperty(
        "acceleration", [this] { return m_acceleration.value(); }, nullptr);
    builder.AddDoubleProperty(
        "accelerationVoltage", [this] { return (m_acceleration * kA).value(); },
        nullptr);
    builder.AddDoubleProperty(
        "outputVoltage", [this] { return m_output.value(); }, nullptr);
  }

 private:
  units::unit_t<Velocity> m_velocity;
  units::unit_t<Acceleration> m_acceleration;
  units::volt_t m_output;
};
}  // namespace frc
