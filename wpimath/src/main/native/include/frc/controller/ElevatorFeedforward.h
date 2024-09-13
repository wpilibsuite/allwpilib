// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/MathExtras.h>

#include "frc/EigenCore.h"
#include "frc/controller/LinearPlantInversionFeedforward.h"
#include "frc/system/plant/LinearSystemId.h"
#include "units/length.h"
#include "units/time.h"
#include "units/voltage.h"
#include "wpimath/MathShared.h"

namespace frc {
/**
 * A helper class that computes feedforward outputs for a simple elevator
 * (modeled as a motor acting against the force of gravity).
 */
class ElevatorFeedforward {
 public:
  using Distance = units::meters;
  using Velocity =
      units::compound_unit<Distance, units::inverse<units::seconds>>;
  using Acceleration =
      units::compound_unit<Velocity, units::inverse<units::seconds>>;
  using kv_unit = units::compound_unit<units::volts, units::inverse<Velocity>>;
  using ka_unit =
      units::compound_unit<units::volts, units::inverse<Acceleration>>;

  /**
   * Creates a new ElevatorFeedforward with the specified gains.
   *
   * @param kS The static gain, in volts.
   * @param kG The gravity gain, in volts.
   * @param kV The velocity gain, in volt seconds per distance.
   * @param kA The acceleration gain, in volt seconds² per distance.
   */
  constexpr ElevatorFeedforward(
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
   * @param velocity     The velocity setpoint, in distance per second.
   * @param acceleration The acceleration setpoint, in distance per second².
   * @return The computed feedforward, in volts.
   */
  constexpr units::volt_t Calculate(units::unit_t<Velocity> velocity,
                                    units::unit_t<Acceleration> acceleration =
                                        units::unit_t<Acceleration>(0)) {
    return kS * wpi::sgn(velocity) + kG + kV * velocity + kA * acceleration;
  }

  /**
   * Calculates the feedforward from the gains and setpoints.
   *
   * @param currentVelocity The current velocity setpoint, in distance per
   *                        second.
   * @param nextVelocity    The next velocity setpoint, in distance per second.
   * @param dt              Time between velocity setpoints in seconds.
   * @return The computed feedforward, in volts.
   */
  units::volt_t Calculate(units::unit_t<Velocity> currentVelocity,
                          units::unit_t<Velocity> nextVelocity,
                          units::second_t dt) const {
    // Discretize the affine model.
    //
    //   dx/dt = Ax + Bu + c
    //   dx/dt = Ax + B(u + B⁺c)
    //   xₖ₊₁ = eᴬᵀxₖ + A⁻¹(eᴬᵀ - I)B(uₖ + B⁺cₖ)
    //   xₖ₊₁ = A_d xₖ + B_d (uₖ + B⁺cₖ)
    //   xₖ₊₁ = A_d xₖ + B_duₖ + B_d B⁺cₖ
    //
    // Solve for uₖ.
    //
    //   B_duₖ = xₖ₊₁ − A_d xₖ − B_d B⁺cₖ
    //   uₖ = B_d⁺(xₖ₊₁ − A_d xₖ − B_d B⁺cₖ)
    //   uₖ = B_d⁺(xₖ₊₁ − A_d xₖ) − B⁺cₖ
    //
    // For an elevator with the model
    // dx/dt = -Kv/Ka x + 1/Ka u - Kg/Ka - Ks/Ka sgn(x),
    // A = -Kv/Ka, B = 1/Ka, and c = -(Kg/Ka + Ks/Ka sgn(x)). Substitute in B
    // assuming sgn(x) is a constant for the duration of the step.
    //
    //   uₖ = B_d⁺(xₖ₊₁ − A_d xₖ) − Ka(-(Kg/Ka + Ks/Ka sgn(x)))
    //   uₖ = B_d⁺(xₖ₊₁ − A_d xₖ) + Ka(Kg/Ka + Ks/Ka sgn(x))
    //   uₖ = B_d⁺(xₖ₊₁ − A_d xₖ) + Kg + Ks sgn(x)
    auto plant = LinearSystemId::IdentifyVelocitySystem<Distance>(kV, kA);
    LinearPlantInversionFeedforward<1, 1> feedforward{plant, dt};

    Vectord<1> r{currentVelocity.value()};
    Vectord<1> nextR{nextVelocity.value()};

    return kG + kS * wpi::sgn(currentVelocity.value()) +
           units::volt_t{feedforward.Calculate(r, nextR)(0)};
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
  constexpr units::unit_t<Velocity> MinAchievableVelocity(
      units::volt_t maxVoltage, units::unit_t<Acceleration> acceleration) {
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
  constexpr units::unit_t<Acceleration> MaxAchievableAcceleration(
      units::volt_t maxVoltage, units::unit_t<Velocity> velocity) {
    return (maxVoltage - kS * wpi::sgn(velocity) - kG - kV * velocity) / kA;
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

  /// The static gain.
  const units::volt_t kS;

  /// The gravity gain.
  const units::volt_t kG;

  /// The velocity gain.
  const units::unit_t<kv_unit> kV;

  /// The acceleration gain.
  const units::unit_t<ka_unit> kA;
};
}  // namespace frc

#include "frc/controller/proto/ElevatorFeedforwardProto.h"
#include "frc/controller/struct/ElevatorFeedforwardStruct.h"
