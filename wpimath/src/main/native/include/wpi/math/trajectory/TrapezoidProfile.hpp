// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <format>
#include <iostream>
#include <type_traits>

#include "wpi/math/util/MathShared.hpp"
#include "wpi/units/math.hpp"
#include "wpi/units/time.hpp"

namespace wpi::math {

/**
 * A trapezoid-shaped velocity profile.
 *
 * While this class can be used for a profiled movement from start to finish,
 * the intended usage is to filter a reference's dynamics based on trapezoidal
 * velocity constraints. To compute the reference obeying this constraint, do
 * the following.
 *
 * Initialization:
 * @code{.cpp}
 * TrapezoidProfile::Constraints constraints{kMaxV, kMaxA};
 * TrapezoidProfile::State previousProfiledReference = initialReference;
 * TrapezoidProfile profile{constraints};
 * @endcode
 *
 * Run on update:
 * @code{.cpp}
 * previousProfiledReference = profile.Calculate(timeSincePreviousUpdate,
 *                                               previousProfiledReference,
 *                                               unprofiledReference);
 * @endcode
 *
 * where `unprofiledReference` is free to change between calls. Note that when
 * the unprofiled reference is within the constraints, `Calculate()` returns the
 * unprofiled reference unchanged.
 *
 * Otherwise, a timer can be started to provide monotonic values for
 * `Calculate()` and to determine when the profile has completed via
 * `IsFinished()`.
 */
template <class Distance>
class TrapezoidProfile {
 public:
  using Distance_t = wpi::units::unit_t<Distance>;
  using Velocity =
      wpi::units::compound_unit<Distance,
                                wpi::units::inverse<wpi::units::seconds>>;
  using Velocity_t = wpi::units::unit_t<Velocity>;
  using Acceleration =
      wpi::units::compound_unit<Velocity,
                                wpi::units::inverse<wpi::units::seconds>>;
  using Acceleration_t = wpi::units::unit_t<Acceleration>;

  /**
   * Profile constraints.
   */
  class Constraints {
   public:
    /// Maximum velocity.
    Velocity_t maxVelocity{0};

    /// Maximum acceleration.
    Acceleration_t maxAcceleration{0};

    /**
     * Default constructor.
     */
    constexpr Constraints() {
      if (!std::is_constant_evaluated()) {
        wpi::math::MathSharedStore::ReportUsage("TrapezoidProfile", "");
      }
    }

    /**
     * Constructs constraints for a Trapezoid Profile.
     *
     * @param maxVelocity Maximum velocity, must be non-negative.
     * @param maxAcceleration Maximum acceleration, must be non-negative.
     */
    constexpr Constraints(Velocity_t maxVelocity,
                          Acceleration_t maxAcceleration)
        : maxVelocity{maxVelocity}, maxAcceleration{maxAcceleration} {
      if (!std::is_constant_evaluated()) {
        wpi::math::MathSharedStore::ReportUsage("TrapezoidProfile", "");
      }

      if (maxVelocity < Velocity_t{0} || maxAcceleration < Acceleration_t{0}) {
        throw std::domain_error("Constraints must be non-negative");
      }
    }
  };

  /**
   * Profile state.
   */
  class State {
   public:
    /// The position at this state.
    Distance_t position{0};

    /// The velocity at this state.
    Velocity_t velocity{0};

    constexpr bool operator==(const State&) const = default;
  };

  /**
   * Profile timings
   */
  class ProfileTiming {
   public:
    wpi::units::second_t accelTime;
    wpi::units::second_t cruiseTime;
    wpi::units::second_t decelTime;

    constexpr bool operator==(const ProfileTiming&) const = default;
  };

  /**
   * Constructs a TrapezoidProfile.
   *
   * @param constraints The constraints on the profile, like maximum velocity.
   */
  constexpr TrapezoidProfile(Constraints constraints)  // NOLINT
      : m_constraints(constraints) {}

  constexpr TrapezoidProfile(const TrapezoidProfile&) = default;
  constexpr TrapezoidProfile& operator=(const TrapezoidProfile&) = default;
  constexpr TrapezoidProfile(TrapezoidProfile&&) = default;
  constexpr TrapezoidProfile& operator=(TrapezoidProfile&&) = default;

  /**
   * Calculates the position and velocity for the profile at a time t where the
   * current state is at time t = 0.
   *
   * @param t How long to advance from the current state toward the desired
   *     state.
   * @param current The current state.
   * @param goal The desired state when the profile is complete.
   * @return The position and velocity of the profile at time t.
   */
  constexpr State Calculate(wpi::units::second_t t, State current, State goal) {
    State sample{current};
    wpi::units::second_t recoveryTime = AdjustStates(current, goal);
    double sign = GetSign(current, goal);
    Acceleration_t acceleration = sign * m_constraints.maxAcceleration;
    m_profile = GenerateProfile(sign, current, goal);

    // The accelTime and recoveryTime will always be in the same direction
    // since if the sign of the profile differs from the sign of recovery
    // acceleration, the profile basically starts at max velocity.
    m_profile.accelTime += recoveryTime;

    auto advance = [&](wpi::units::second_t time, Acceleration_t acceleration,
                       State& state) {
      state.position +=
          state.velocity * time + acceleration / 2.0 * time * time;
      state.velocity += acceleration * time;
    };
    advance(wpi::units::math::min(t, m_profile.accelTime),
            recoveryTime > 0.0_s && sample.velocity * sign > Velocity_t{0.0}
                ? -acceleration
                : acceleration,
            sample);

    if (t > m_profile.accelTime) {
      t -= m_profile.accelTime;
      advance(wpi::units::math::min(t, m_profile.cruiseTime),
              Acceleration_t{0.0}, sample);

      if (t > m_profile.cruiseTime) {
        t -= m_profile.cruiseTime;
        advance(wpi::units::math::min(t, m_profile.decelTime), -acceleration,
                sample);
        if (t > m_profile.decelTime) {
          sample = goal;
        }
      }
    }

    return sample;
  }

  /**
   * Returns the time to get between two states. This does not affect the
   * internal variables, and as a result, may be used for states not on the
   * active trajectory.
   *
   * @param target The target distance.
   * @return The time left until a target distance in the profile is reached, or
   * zero if no goal was set.
   */
  constexpr wpi::units::second_t TimeLeftUntil(State current,
                                               State goal) const {
    wpi::units::second_t recoveryTime = AdjustStates(current, goal);
    double sign = GetSign(current, goal);
    ProfileTiming profile = GenerateProfile(sign, current, goal);

    profile.accelTime += recoveryTime;

    return profile.accelTime + profile.cruiseTime + profile.decelTime;
  }

  /**
   * Returns the total time the profile takes to reach the goal.
   *
   * @return The total time the profile takes to reach the goal, or zero if no
   * goal was set.
   */
  constexpr wpi::units::second_t TotalTime() const {
    return m_profile.accelTime + m_profile.cruiseTime + m_profile.decelTime;
  }

  /**
   * Returns true if the profile has reached the goal.
   *
   * The profile has reached the goal if the time since the profile started has
   * exceeded the profile's total time.
   *
   * @param t The time since the beginning of the profile.
   * @return True if the profile has reached the goal.
   */
  constexpr bool IsFinished(wpi::units::second_t t) const {
    return t >= TotalTime();
  }

 private:
  /**
   * Adjusts the profile states to be within the constraints and returns the
   * time needed to bring the current state back within the constraints.
   *
   * In order to smoothly return to a state within the constraints, the current
   * state is modified to be the result of accelerating towards a valid
   * velocity at the maximum acceleration. This method returns the time this
   * transition takes. By contrast, the goal velocity is simply clamped
   * to the valid region.
   *
   * @param current The current state to be adjusted.
   * @param goal The goal state state to be adjusted.
   * @return The time taken to make the current state valid.
   */
  constexpr wpi::units::second_t AdjustStates(State& current,
                                              State& goal) const {
    if (wpi::units::math::abs(goal.velocity) > m_constraints.maxVelocity) {
      goal.velocity =
          wpi::units::math::copysign(m_constraints.maxVelocity, goal.velocity);
    }

    wpi::units::second_t recoveryTime{0.0};
    Velocity_t violationAmount =
        wpi::units::math::abs(current.velocity) - m_constraints.maxVelocity;

    if (violationAmount > Velocity_t{0.0}) {
      recoveryTime = violationAmount / m_constraints.maxAcceleration;
      current.position +=
          current.velocity * recoveryTime +
          wpi::units::math::copysign(m_constraints.maxAcceleration,
                                     -current.velocity) *
              recoveryTime * recoveryTime / 2.0;
      current.velocity = wpi::units::math::copysign(m_constraints.maxVelocity,
                                                    current.velocity);
    }

    return recoveryTime;
  }

  /**
   * Returns the sign of the profile.
   *
   * The current and goal states must be within the profile constraints for a
   * valid sign.
   *
   * @param current The initial state, adjusted not to violate the constraints.
   * @param goal The goal state of the profile.
   * @return 1.0 if the profile direcetion is positive, -1.0 if it is not.
   */
  constexpr double GetSign(const State& current, const State& goal) const {
    Distance_t dx = goal.position - current.position;

    // Threshold distance is the distance to traverse between the initial and
    // final velocities with extremal acceleration.
    // v₂² − v₁² = 2ax
    // Because the acceleration has the sign of v₂ − v₁, we can factor v₂² − v₁²
    // and take the absolute value of the v₂ − v₁ term.
    // |v₂ − v₁|(v₂ + v₁) / 2a = x
    Distance_t thresholdDistance =
        wpi::units::math::abs(goal.velocity - current.velocity) /
        m_constraints.maxAcceleration * (current.velocity + goal.velocity) /
        2.0;

    // Make sure that we always choose the fastest feasible direction.
    return wpi::units::math::copysign(
        wpi::units::dimensionless_t{1.0},
        dx == thresholdDistance ? (goal.velocity + current.velocity).value()
                                : (dx - thresholdDistance).value());
  }

  /**
   * Generates profile timings from valid current and goal states.
   *
   * Returns the time for each section of the profile from current
   * and goal states with valid velocities.
   *
   * @param current The valid current state.
   * @param goal The valid goal state.
   * @return The time for each section of the profile.
   */
  constexpr ProfileTiming GenerateProfile(double sign, const State& current,
                                          const State& goal) const {
    ProfileTiming profile{};

    Acceleration_t acceleration = sign * m_constraints.maxAcceleration;
    Velocity_t velocityLimit = sign * m_constraints.maxVelocity;
    Distance_t distance = goal.position - current.position;

    // x₁ + x₂ = Δx
    // vₚ² − v₁² = 2ax₁
    // vₚ² − v₂² = 2ax₂
    // Δx = x₁ + x₂ = (2vₚ² − (v₁² + v₂²)) / 2a
    // vₚ = √(aΔx + (v₁² + v₂²) / 2)
    Velocity_t peakVelocity =
        sign * wpi::units::math::sqrt((goal.velocity * goal.velocity +
                                       current.velocity * current.velocity) /
                                          2 +
                                      acceleration * distance);

    // Handle the case where we hit maximum velocity.
    if (sign * peakVelocity > m_constraints.maxVelocity) {
      profile.accelTime = (velocityLimit - current.velocity) / acceleration;
      profile.decelTime = (velocityLimit - goal.velocity) / acceleration;

      // Δx = x₁ + x₂ + x₃ = (2vₚ² − (v₁² + v₂²)) / 2a + x₃
      // x₃ = Δx - (2vₚ² − (v₁² + v₂²)) / 2a
      // cruiseTime = x₃ / vₚ
      profile.cruiseTime = (distance - (2 * velocityLimit * velocityLimit -
                                        (current.velocity * current.velocity +
                                         goal.velocity * goal.velocity)) /
                                           (2 * acceleration)) /
                           velocityLimit;
    } else {
      profile.accelTime = (peakVelocity - current.velocity) / acceleration;
      profile.decelTime = (peakVelocity - goal.velocity) / acceleration;
    }

    return profile;
  }

  Constraints m_constraints;
  ProfileTiming m_profile;
};

}  // namespace wpi::math
