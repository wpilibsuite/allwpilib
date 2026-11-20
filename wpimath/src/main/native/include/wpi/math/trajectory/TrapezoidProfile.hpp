// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <array>
#include <cstddef>
#include <stdexcept>
#include <utility>
#include <vector>

#include "wpi/math/trajectory/Trajectory.hpp"
#include "wpi/math/trajectory/TrapezoidProfileSample.hpp"
#include "wpi/units/base.hpp"
#include "wpi/units/math.hpp"
#include "wpi/units/time.hpp"
#include "wpi/util/UsageReporting.hpp"

namespace wpi::math {

/**
 * TrapezoidProfile constraints.
 */
template <class Distance>
class TrapezoidProfileConstraints {
 public:
  using Sample = TrapezoidProfileSample<Distance>;
  using Velocity_t = typename Sample::Velocity_t;
  using Acceleration_t = typename Sample::Acceleration_t;

  /// Maximum velocity.
  Velocity_t maxVelocity{0};

  /// Maximum acceleration.
  Acceleration_t maxAcceleration{0};

  /**
   * Constructs constraints for a Trapezoid Profile.
   *
   * @param maxVelocity Maximum velocity, must be positive.
   * @param maxAcceleration Maximum acceleration, must be positive.
   */
  constexpr TrapezoidProfileConstraints(Velocity_t maxVelocity,
                                        Acceleration_t maxAcceleration)
      : maxVelocity{maxVelocity}, maxAcceleration{maxAcceleration} {
    if !consteval {
      wpi::util::ReportUsage("TrapezoidProfile", "");
    }

    if (maxVelocity <= Velocity_t{0} || maxAcceleration <= Acceleration_t{0}) {
      throw std::domain_error("Constraints must be positive");
    }
  }
};

/**
 * A trajectory that follows a trapezoid-shaped velocity profile.
 */
template <class Distance>
class TrapezoidProfile : public Trajectory<TrapezoidProfileSample<Distance>> {
 public:
  using Constraints = TrapezoidProfileConstraints<Distance>;
  using Sample = TrapezoidProfileSample<Distance>;
  using Distance_t = typename Sample::Distance_t;
  using Velocity = typename Sample::Velocity;
  using Velocity_t = typename Sample::Velocity_t;
  using Acceleration = typename Sample::Acceleration;
  using Acceleration_t = typename Sample::Acceleration_t;

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
   * Interpolates between two samples along the profile.
   *
   * The profile's samples are its phase transitions, so the acceleration is
   * constant between consecutive samples. The start sample's state is
   * integrated forward using its acceleration, so the result lies exactly on
   * the profile.
   *
   * @param start The starting sample.
   * @param end The ending sample.
   * @param t The interpolation parameter between 0 and 1.
   * @return The interpolated sample.
   */
  Sample Interpolate(const Sample& start, const Sample& end,
                     double t) const final {
    if (t <= 0.0) {
      return start;
    } else if (t >= 1.0) {
      return end;
    }

    const wpi::units::second_t dt = t * (end.time - start.time);

    // v = v_i + at   (1)
    Velocity_t velocity = start.velocity + start.acceleration * dt;
    // x = x_i + v_i t + at² / 2   (2)
    Distance_t position = start.position + start.velocity * dt +
                          start.acceleration / 2.0 * dt * dt;

    return Sample{start.time + dt, position, velocity, start.acceleration};
  }

  /**
   * Generates a profile from the current state to the goal state.
   *
   * The trajectory starts at the current state at t = 0 and ends at the goal
   * state once the profile completes. Sampling past the end of the trajectory
   * returns the goal state.
   *
   * @param constraints The constraints on the profile, like maximum velocity.
   * @param current The current state.
   * @param goal The desired state when the profile is complete.
   * @return The trajectory from the current state to the goal state.
   */
  static TrapezoidProfile<Distance> Generate(const Constraints& constraints,
                                             const State& current, State goal) {
    using Trajectory = TrapezoidProfile<Distance>;
    using Sample = typename Trajectory::Sample;

    State state = current;

    // Adjust states so that they are within the constraints and get the time
    // required for the current state to return to a valid state.
    wpi::units::second_t recoveryTime = AdjustStates(constraints, state, goal);
    double sign = GetSign(constraints, state, goal);
    Timing timing{sign, constraints, state, goal};

    // In the case that the sign of the profile and the sign of the acceleration
    // are identical, the recovery can be treated as an extension of the first
    // segment. In the case that they differ, the recovered state will have a
    // velocity of v_l and the above calculated t_1 will be zero. To handle
    // this, the first segment's acceleration is flipped to ensure proper
    // recovery.
    timing.t_1 += recoveryTime;

    // Floating point error can make segment durations slightly negative, which
    // would place samples before the start of the trajectory
    timing.t_1 = wpi::units::math::max(timing.t_1, 0_s);
    timing.t_2 = wpi::units::math::max(timing.t_2, 0_s);
    timing.t_3 = wpi::units::math::max(timing.t_3, 0_s);

    Acceleration_t acceleration = sign * constraints.maxAcceleration;
    Acceleration_t firstLegAcceleration =
        recoveryTime > 0.0_s && current.velocity * sign > Velocity_t{0.0}
            ? -acceleration
            : acceleration;
    Acceleration_t lastLegAcceleration = -acceleration;

    // The samples are the profile's phase transitions: the start, the end of
    // the first segment, the end of the constant velocity segment, and the end
    // of the profile. Each sample's acceleration is the acceleration of the
    // segment that starts at it. The sampled trajectory starts at the current
    // state, regardless of validity.
    const std::array<wpi::units::second_t, 3> segmentDurations{
        timing.t_1, timing.t_2, timing.t_3};
    const std::array<Acceleration_t, 3> segmentAccelerations{
        firstLegAcceleration, Acceleration_t{0.0}, lastLegAcceleration};

    std::vector<Sample> samples;
    samples.reserve(4);

    wpi::units::second_t time = 0_s;
    Distance_t position = current.position;
    Velocity_t velocity = current.velocity;
    for (size_t i = 0; i < segmentDurations.size(); ++i) {
      samples.emplace_back(time, position, velocity,
                           AccelerationAt(time, timing, firstLegAcceleration,
                                          lastLegAcceleration));

      const auto dt = segmentDurations[i];
      const auto acceleration = segmentAccelerations[i];
      const Velocity_t v_i = velocity;
      // v = v_i + at   (1)
      velocity = v_i + acceleration * dt;
      // x = x_i + v_i t + at² / 2   (2)
      position += v_i * dt + acceleration / 2.0 * dt * dt;
      time += dt;
    }

    // Use the goal state directly for the last sample to avoid accumulating
    // floating point error
    samples.emplace_back(timing.Duration(), goal.position, goal.velocity,
                         Acceleration_t{0.0});

    return Trajectory{std::move(samples)};
  }

 private:
  /**
   * TrapezoidProfile timings.
   */
  class Timing {
   public:
    /// The time the profile spends in the first segment.
    wpi::units::second_t t_1{0.0};

    /// The time the profile spends at the velocity limit.
    wpi::units::second_t t_2{0.0};

    /// The time the profile spends in the last segment.
    wpi::units::second_t t_3{0.0};

    /**
     * Generates profile timings from valid current and goal states.
     *
     * Returns the time for each section of the profile from current
     * and goal states with valid velocities.
     *
     * @param sign The sign of the profile to generate.
     * @param constraints The constraints on the profile, like maximum velocity.
     * @param current The valid current state.
     * @param goal The valid goal state.
     * @return The time for each section of the profile.
     */
    constexpr Timing(double sign, const Constraints& constraints,
                     const State& current, const State& goal) {
      Acceleration_t acceleration = sign * constraints.maxAcceleration;
      Velocity_t velocityLimit = sign * constraints.maxVelocity;
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
      if (sign * peakVelocity > constraints.maxVelocity) {
        // t_1 = (v_l - v_i) / a   (13)
        this->t_1 = (velocityLimit - current.velocity) / acceleration;
        // t_3 = (v_l - v_t) / a   (15)
        this->t_3 = (velocityLimit - goal.velocity) / acceleration;

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
        this->t_2 = x_2 / velocityLimit;
      } else {
        // t_1 = (v_p - v_i) / a   (13)
        this->t_1 = (peakVelocity - current.velocity) / acceleration;
        // t_3 = (v_p - v_t) / a   (15)
        this->t_3 = (peakVelocity - goal.velocity) / acceleration;
      }
    }

    constexpr bool operator==(const Timing&) const = default;

    /**
     * Returns the duration of the profile.
     *
     * @return The duration of the profile, or zero if no goal was set.
     */
    constexpr wpi::units::second_t Duration() const { return t_1 + t_2 + t_3; }
  };

  /**
   * Constructs a TrapezoidProfile from its phase transition samples.
   *
   * @param samples The samples of the trajectory.
   * @throws std::invalid_argument if the vector of samples is empty.
   */
  explicit TrapezoidProfile(std::vector<Sample> samples)
      : Trajectory<Sample>(std::move(samples)) {}

  /**
   * Returns the acceleration of a profile at the given time.
   *
   * @param time The time relative to the profile start.
   * @param timing The time spent in each segment of the profile.
   * @param firstLegAcceleration The acceleration during the first segment of
   *     the profile.
   * @param lastLegAcceleration The acceleration during the last segment of the
   *     profile.
   * @return The acceleration of the profile at the given time.
   */
  static constexpr Acceleration_t AccelerationAt(
      wpi::units::second_t time, const Timing& timing,
      Acceleration_t firstLegAcceleration, Acceleration_t lastLegAcceleration) {
    if (time < timing.t_1) {
      return firstLegAcceleration;
    } else if (time < timing.t_1 + timing.t_2) {
      return Acceleration_t{0.0};
    } else if (time < timing.t_1 + timing.t_2 + timing.t_3) {
      return lastLegAcceleration;
    } else {
      return Acceleration_t{0.0};
    }
  }

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
   * @param constraints The constraints on the profile, like maximum velocity.
   * @param current The current state to be adjusted.
   * @param goal The goal state state to be adjusted.
   * @return The time taken to make the current state valid.
   */
  static constexpr wpi::units::second_t AdjustStates(
      const Constraints& constraints, State& current, State& goal) {
    if (wpi::units::math::abs(goal.velocity) > constraints.maxVelocity) {
      goal.velocity =
          wpi::units::math::copysign(constraints.maxVelocity, goal.velocity);
    }

    wpi::units::second_t recoveryTime{0.0};
    Velocity_t violationAmount =
        wpi::units::math::abs(current.velocity) - constraints.maxVelocity;

    if (violationAmount > Velocity_t{0.0}) {
      recoveryTime = violationAmount / constraints.maxAcceleration;
      // x = x_i + v_i t + at² / 2   (2)
      current.position += current.velocity * recoveryTime +
                          wpi::units::math::copysign(
                              constraints.maxAcceleration, -current.velocity) *
                              recoveryTime * recoveryTime / 2.0;
      // The closest valid velocity will have the magnitude of the max velocity.
      current.velocity =
          wpi::units::math::copysign(constraints.maxVelocity, current.velocity);
    }

    return recoveryTime;
  }

  /**
   * Returns the sign of the profile.
   *
   * The current and goal states must be within the profile constraints for a
   * valid sign.
   *
   * @param constraints The constraints on the profile, like maximum velocity.
   * @param current The initial state, adjusted not to violate the constraints.
   * @param goal The goal state of the profile.
   * @return 1.0 if the profile direction is positive, -1.0 if it is not.
   */
  static constexpr double GetSign(const Constraints& constraints,
                                  const State& current, const State& goal) {
    Distance_t dx = goal.position - current.position;

    // Calculate threshold displacement
    // d = |v_t - v_i|(v_t + v_i) / (2 a_m)   (9)
    Distance_t d = wpi::units::math::abs(goal.velocity - current.velocity) *
                   (goal.velocity + current.velocity) /
                   (2.0 * constraints.maxAcceleration);

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
};

}  // namespace wpi::math

#include "wpi/math/trajectory/struct/TrapezoidProfileStruct.hpp"
