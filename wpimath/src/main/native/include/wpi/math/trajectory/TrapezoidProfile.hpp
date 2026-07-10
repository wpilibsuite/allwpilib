// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

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
      if !consteval {
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
      if !consteval {
        wpi::math::MathSharedStore::ReportUsage("TrapezoidProfile", "");
      }

      if (maxVelocity <= Velocity_t{0} ||
          maxAcceleration <= Acceleration_t{0}) {
        throw std::domain_error("Constraints must be positive");
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
   * Profile timings.
   */
  class ProfileTiming {
   public:
    /// The time the profile spends in the first leg.
    wpi::units::second_t t_1;
    /// The time the profile spends at the velocity limit.
    wpi::units::second_t t_2;
    /// The time the profile spends in the last leg.
    wpi::units::second_t t_3;

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
   * @param t The amount of time to advance from the current state toward the
   * desired state.
   * @param current The current state.
   * @param goal The desired state when the profile is complete.
   * @return The position and velocity of the profile at time t.
   */
  constexpr State Calculate(wpi::units::second_t t, State current, State goal) {
    // Sampled trajectory should start at the current state, regardless of
    // validity.
    State sample{current};

    // Adjust states so that they are within the constraints and get the time
    // required for the current state to return to a valid state.
    wpi::units::second_t recoveryTime = AdjustStates(current, goal);
    double sign = GetSign(current, goal);
    m_profile = GenerateProfile(sign, current, goal);

    // In the case that the sign of the profile and the sign of the acceleration
    // are identical, the recovery can be treated as an extension of the first
    // segment. In the case that they differ, the recovered state will have a
    // velocity of v_l and the above calculated t_1 will be zero. To handle
    // this, a check can be added on the first segment to ensure proper
    // recovery.
    m_profile.t_1 += recoveryTime;

    auto advance = [](wpi::units::second_t time, Acceleration_t acceleration,
                      State& state) {
      // x = x_i + v_i t + at² / 2   (2)
      state.position +=
          state.velocity * time + acceleration / 2.0 * time * time;
      // v = v_i + at   (1)
      state.velocity += acceleration * time;
    };

    Acceleration_t acceleration = sign * m_constraints.maxAcceleration;
    advance(wpi::units::math::min(t, m_profile.t_1),
            // Handle recovery to a feasible state if necessary.
            recoveryTime > 0.0_s && sample.velocity * sign > Velocity_t{0.0}
                ? -acceleration
                : acceleration,
            sample);

    if (t > m_profile.t_1) {
      t -= m_profile.t_1;
      advance(wpi::units::math::min(t, m_profile.t_2), Acceleration_t{0.0},
              sample);

      if (t > m_profile.t_2) {
        t -= m_profile.t_2;
        advance(wpi::units::math::min(t, m_profile.t_3), -acceleration, sample);

        if (t > m_profile.t_3) {
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
   * @param current The current state.
   * @param goal The goal state.
   * @return The time left until the target state.
   */
  constexpr wpi::units::second_t TimeLeftUntil(State current,
                                               State goal) const {
    // Adjust states so that they are within the constraints and get the time
    // required for the current state to return to a valid state.
    wpi::units::second_t recoveryTime = AdjustStates(current, goal);
    double sign = GetSign(current, goal);
    ProfileTiming profile = GenerateProfile(sign, current, goal);

    profile.t_1 += recoveryTime;

    return profile.t_1 + profile.t_2 + profile.t_3;
  }

  /**
   * Returns the duration of the profile.
   *
   * @return The duration of the profile, or zero if no goal was set.
   */
  constexpr wpi::units::second_t Duration() const {
    return m_profile.t_1 + m_profile.t_2 + m_profile.t_3;
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
    return t >= Duration();
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
      // x = x_i + v_i t + at² / 2   (2)
      current.position +=
          current.velocity * recoveryTime +
          wpi::units::math::copysign(m_constraints.maxAcceleration,
                                     -current.velocity) *
              recoveryTime * recoveryTime / 2.0;
      // The closest valid velocity will have the magnitude of the max velocity.
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
   * @return 1.0 if the profile direction is positive, -1.0 if it is not.
   */
  constexpr double GetSign(const State& current, const State& goal) const {
    Distance_t dx = goal.position - current.position;

    // Calculate threshold displacement
    // d = |v_t - v_i|(v_t + v_i) / (2 a_m)   (9)
    Distance_t d = wpi::units::math::abs(goal.velocity - current.velocity) *
                   (goal.velocity + current.velocity) /
                   (2.0 * m_constraints.maxAcceleration);

    // As discussed in TrapezoidProfile.md, the correct sign must be chosen when
    // dx == d because following a suboptimal profile may lead to "chattering".
    // Additionally, if numerical precision errors cause the calculated optimal
    // sign to change throughout the profile, that may lead to suboptimal states
    // being calculated. To fix this, we add a tolerance such that if |dx - d| <
    // epsilon, we return the sign that would lead to the minimum profile being
    // calculated. We do not have control over the floating point precision
    // error from previous calculations, and as such, it is difficult to bound
    // the possible error. 1e-12 should be good enough for FRC though.
    if (wpi::units::math::abs(dx - d) < Distance_t{1e-12}) {
      return std::copysign(1.0, goal.velocity.value());
    } else {
      if (dx > d) {
        return 1.0;
      } else {
        return -1.0;
      }
    }
  }

  /**
   * Generates profile timings from valid current and goal states.
   *
   * Returns the time for each section of the profile from current
   * and goal states with valid velocities.
   *
   * @param sign The sign of the profile to generate.
   * @param current The valid current state.
   * @param goal The valid goal state.
   * @return The time for each section of the profile.
   */
  constexpr ProfileTiming GenerateProfile(double sign, const State& current,
                                          const State& goal) const {
    ProfileTiming profile{};

    Acceleration_t acceleration = sign * m_constraints.maxAcceleration;
    Velocity_t velocityLimit = sign * m_constraints.maxVelocity;
    Distance_t dx = goal.position - current.position;

    // Calculate the peak velocity to compare to velocity constraint.
    // v_p = √(aΔx + (v_t² + v_i²) / 2)   (8)
    Velocity_t peakVelocity =
        sign * wpi::units::math::sqrt(wpi::units::math::max(
                   acceleration * dx + (goal.velocity * goal.velocity +
                                        current.velocity * current.velocity) /
                                           2,
                   wpi::units::math::pow<2>(Velocity_t{0.0})));

    // Handle the case where we hit maximum velocity.
    if (sign * peakVelocity > m_constraints.maxVelocity) {
      // t_1 = (v_l - v_i) / a   (13)
      profile.t_1 = (velocityLimit - current.velocity) / acceleration;
      // t_3 = (v_l - v_t) / a   (15)
      profile.t_3 = (velocityLimit - goal.velocity) / acceleration;

      // x_1 = (v_p² - v_i²) / (2a)   (6)
      // Substitute v_p for v_l because this is the velocity constrained case.
      // x_1 = (v_l² - v_i²) / (2a)
      Distance_t x_1 = (velocityLimit * velocityLimit -
                        current.velocity * current.velocity) /
                       (2 * acceleration);

      // x_3 = (v_p² - v_t²) / (2a)   (7)
      // Substitute v_p for v_l because this is the velocity constrained case.
      // x_3 = (v_l² - v_t²) / (2a)
      Distance_t x_3 =
          (velocityLimit * velocityLimit - goal.velocity * goal.velocity) /
          (2 * acceleration);

      // x_2 = Δx - x_1 - x_3   (12)
      Distance_t x_2 = dx - x_1 - x_3;

      // t_2 = x_2 / v_l   (14)
      profile.t_2 = x_2 / velocityLimit;
    } else {
      // t_1 = (v_p - v_i) / a   (13)
      profile.t_1 = (peakVelocity - current.velocity) / acceleration;
      // t_3 = (v_p - v_t) / a   (15)
      profile.t_3 = (peakVelocity - goal.velocity) / acceleration;
    }

    return profile;
  }

  Constraints m_constraints;
  ProfileTiming m_profile;
};

}  // namespace wpi::math
