/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <units/units.h>
#include <wpi/MathExtras.h>

namespace frc {
/**
 * A helper class that computes feedforward voltages for a simple
 * permanent-magnet DC motor.
 */
template <class Distance>
class SimpleMotorFeedforward {
  using Velocity =
      units::compound_unit<Distance, units::inverse<units::seconds>>;
  using Acceleration =
      units::compound_unit<Velocity, units::inverse<units::seconds>>;
  using kv_unit = units::compound_unit<units::volts, units::inverse<Velocity>>;
  using ka_unit =
      units::compound_unit<units::volts, units::inverse<Acceleration>>;

 public:
  constexpr SimpleMotorFeedforward() = default;

  /**
   * Creates a new SimpleMotorFeedforward with the specified gains.
   *
   * @param kS The static gain, in volts.
   * @param kV The velocity gain, in volt seconds per distance.
   * @param kA The acceleration gain, in volt seconds^2 per distance.
   */
  constexpr SimpleMotorFeedforward(
      units::volt_t kS, units::unit_t<kv_unit> kV,
      units::unit_t<ka_unit> kA = units::unit_t<ka_unit>(0))
      : kS(kS), kV(kV), kA(kA) {}

  /**
   * Calculates the feedforward from the gains and setpoints.
   *
   * @param velocity     The velocity setpoint, in distance per second.
   * @param acceleration The acceleration setpoint, in distance per second^2.
   * @return The computed feedforward, in volts.
   */
  constexpr units::volt_t Calculate(units::unit_t<Velocity> velocity,
                                    units::unit_t<Acceleration> acceleration =
                                        units::unit_t<Acceleration>(0)) const {
    return kS * wpi::sgn(velocity) + kV * velocity + kA * acceleration;

    units::volt_t kS{0};
    units::unit_t<kv_unit> kV{0};
    units::unit_t<ka_unit> kA{0};
  }
};
}  // namespace frc
