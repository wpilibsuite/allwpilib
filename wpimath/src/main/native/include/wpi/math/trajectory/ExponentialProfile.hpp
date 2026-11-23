// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <utility>
#include <vector>

#include "wpi/math/trajectory/ExponentialProfileSample.hpp"
#include "wpi/math/trajectory/Trajectory.hpp"
#include "wpi/units/base.hpp"
#include "wpi/units/math.hpp"
#include "wpi/units/time.hpp"

namespace wpi::math {

/**
 * A trajectory that follows an exponential curve-shaped velocity profile.
 *
 * While this class can be used for a profiled movement from start to finish,
 * the intended usage is to filter a reference's dynamics based on
 * ExponentialProfile velocity constraints. To compute the reference obeying
 * this constraint, do the following.
 *
 * Initialization:
 * @code{.cpp}
 * using Profile = ExponentialProfile<wpi::units::meters, wpi::units::volts>;
 * Profile::Constraints constraints{kMaxV, kV, kA};
 * Profile::State previousProfiledReference{initialReference, 0_mps};
 * @endcode
 *
 * Run on update:
 * @code{.cpp}
 * auto sample =
 *   Profile::Generate(constraints, previousProfiledReference,
 *                     unprofiledReference)
 *     .SampleAt(timeSincePreviousUpdate);
 * previousProfiledReference = {sample.position, sample.velocity};
 * @endcode
 *
 * where `unprofiledReference` is free to change between calls. Note that when
 * the unprofiled reference is within the constraints, sampling returns the
 * unprofiled reference unchanged.
 *
 * Otherwise, a timer can be started to provide monotonic values for
 * `SampleAt()` and to determine when the profile has completed by comparing
 * against `Duration()`.
 */
template <class Distance, class Input>
class ExponentialProfile
    : public Trajectory<ExponentialProfileSample<Distance, Input>> {
 public:
  using Sample = ExponentialProfileSample<Distance, Input>;
  using Distance_t = wpi::units::unit_t<Distance>;
  using Velocity =
      wpi::units::compound_unit<Distance,
                                wpi::units::inverse<wpi::units::seconds>>;
  using Velocity_t = wpi::units::unit_t<Velocity>;
  using Acceleration =
      wpi::units::compound_unit<Velocity,
                                wpi::units::inverse<wpi::units::seconds>>;
  using Input_t = wpi::units::unit_t<Input>;
  using A_t = wpi::units::unit_t<wpi::units::inverse<wpi::units::seconds>>;
  using B_t = wpi::units::unit_t<
      wpi::units::compound_unit<Acceleration, wpi::units::inverse<Input>>>;
  using KV = wpi::units::compound_unit<Input, wpi::units::inverse<Velocity>>;
  using kV_t = wpi::units::unit_t<KV>;
  using KA =
      wpi::units::compound_unit<Input, wpi::units::inverse<Acceleration>>;
  using kA_t = wpi::units::unit_t<KA>;

  using Acceleration_t = wpi::units::unit_t<Acceleration>;

  /**
   * Profile constraints.
   */
  class Constraints {
   public:
    /**
     * Constructs constraints for an ExponentialProfile.
     *
     * @param maxInput maximum unsigned input voltage
     * @param A The State-Space 1x1 system matrix.
     * @param B The State-Space 1x1 input matrix.
     */
    constexpr Constraints(Input_t maxInput, A_t A, B_t B)
        : maxInput{maxInput}, A{A}, B{B} {}

    /**
     * Constructs constraints for an ExponentialProfile from characteristics.
     *
     * @param maxInput maximum unsigned input voltage
     * @param kV The velocity gain.
     * @param kA The acceleration gain.
     */
    constexpr Constraints(Input_t maxInput, kV_t kV, kA_t kA)
        : maxInput{maxInput}, A{-kV / kA}, B{1 / kA} {}

    /**
     * Computes the max achievable velocity for an Exponential Profile.
     *
     * @return The steady-state velocity achieved by this profile.
     */
    constexpr Velocity_t MaxVelocity() const { return -maxInput * B / A; }

    /// Maximum unsigned input voltage.
    Input_t maxInput{0};

    /// The State-Space 1x1 system matrix.
    A_t A{0};

    /// The State-Space 1x1 input matrix.
    B_t B{0};
  };

  /** Profile state. */
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
   * The profile's samples are its phase transitions, so the input is constant
   * between consecutive samples. The start sample's state is integrated
   * forward using its input, so the result lies exactly on the profile.
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
    const Input_t u = start.input;

    const State initial{start.position, start.velocity};
    Distance_t position =
        ComputeDistanceFromTime(m_constraints, dt, u, initial);
    Velocity_t velocity =
        ComputeVelocityFromTime(m_constraints, dt, u, initial);

    return Sample{start.time + dt, position, velocity,
                  AccelerationAt(m_constraints, velocity, u), u};
  }

  /**
   * Generates a profile from the current state to the goal state.
   *
   * The trajectory starts at the current state at t = 0 and ends at the goal
   * state once the profile completes. Sampling past the end of the trajectory
   * returns the goal state.
   *
   * @param constraints The constraints on the profile, like maximum input.
   * @param current The current state.
   * @param goal The desired state when the profile is complete.
   * @return The trajectory from the current state to the goal state.
   */
  static ExponentialProfile Generate(const Constraints& constraints,
                                     const State& current, const State& goal) {
    auto direction = ShouldFlipInput(constraints, current, goal) ? -1 : 1;
    Input_t u = direction * constraints.maxInput;

    State inflectionPoint =
        CalculateInflectionPoint(constraints, current, goal, u);
    Timing timing =
        CalculateTiming(constraints, current, inflectionPoint, goal, u);

    // Floating point error can make segment durations slightly negative, which
    // would place samples before the start of the trajectory or out of order,
    // so they're clamped.
    wpi::units::second_t inflectionTime =
        wpi::units::math::max(timing.inflectionTime, 0_s);
    wpi::units::second_t totalTime =
        wpi::units::math::max(timing.totalTime, inflectionTime);

    // The input that holds the goal velocity at steady state (0 = Av + Bu)
    Input_t goalInput = -constraints.A * goal.velocity / constraints.B;

    // The samples are the profile's phase transitions: the start, the
    // inflection point, and the end of the profile. Each sample's input is the
    // input of the segment that starts at it. The sampled trajectory starts at
    // the current state, regardless of validity.
    //
    // The inflection point is computed backward from the goal, so integrating
    // forward from it along the last segment reaches the goal.
    std::vector<Sample> samples{
        Sample{0_s, current.position, current.velocity,
               AccelerationAt(constraints, current.velocity, u), u},
        Sample{inflectionTime, inflectionPoint.position,
               inflectionPoint.velocity,
               AccelerationAt(constraints, inflectionPoint.velocity, -u), -u},
        Sample{totalTime, goal.position, goal.velocity, Acceleration_t{0},
               goalInput}};

    return ExponentialProfile{constraints, std::move(samples)};
  }

 private:
  /**
   * ExponentialProfile timings.
   */
  struct Timing {
    /// The time at which the profile reaches the inflection point.
    wpi::units::second_t inflectionTime;

    /// The duration of the profile.
    wpi::units::second_t totalTime;
  };

  /**
   * Constructs an ExponentialProfile from its phase transition samples.
   *
   * @param constraints The constraints on the profile.
   * @param samples The samples of the trajectory.
   * @throws std::invalid_argument if the vector of samples is empty.
   */
  ExponentialProfile(const Constraints& constraints,
                     std::vector<Sample> samples)
      : Trajectory<Sample>(std::move(samples)), m_constraints{constraints} {}

  /**
   * Returns the acceleration of the system at the given velocity and input.
   *
   * @param constraints The constraints on the profile.
   * @param velocity The velocity.
   * @param input The input.
   * @return The acceleration.
   */
  static constexpr Acceleration_t AccelerationAt(const Constraints& constraints,
                                                 const Velocity_t& velocity,
                                                 const Input_t& input) {
    // dv/dt = Av + Bu
    return constraints.A * velocity + constraints.B * input;
  }

  /**
   * Calculates the point after which the fastest way to reach the goal state is
   * to apply input in the opposite direction.
   *
   * @param constraints The constraints on the profile.
   * @param current The current state.
   * @param goal The desired state when the profile is complete.
   * @param input The signed input applied to this profile from the current
   *     state.
   * @return The position and velocity of the profile at the inflection point.
   */
  static constexpr State CalculateInflectionPoint(
      const Constraints& constraints, const State& current, const State& goal,
      const Input_t& input) {
    auto u = input;

    if (current == goal) {
      return current;
    }

    auto inflectionVelocity =
        SolveForInflectionVelocity(constraints, u, current, goal);
    auto inflectionPosition =
        ComputeDistanceFromVelocity(constraints, inflectionVelocity, -u, goal);

    return {inflectionPosition, inflectionVelocity};
  }

  /**
   * Calculates the time it will take for this profile to reach the inflection
   * point, and the time it will take for this profile to reach the goal state.
   *
   * @param constraints The constraints on the profile.
   * @param current The current state.
   * @param inflectionPoint The inflection point of this profile.
   * @param goal The desired state when the profile is complete.
   * @param input The signed input applied to this profile from the current
   *     state.
   * @return The timing information for this profile.
   */
  static constexpr Timing CalculateTiming(const Constraints& constraints,
                                          const State& current,
                                          const State& inflectionPoint,
                                          const State& goal,
                                          const Input_t& input) {
    auto u = input;
    auto u_dir = wpi::units::math::abs(u) / u;

    wpi::units::second_t inflectionT_forward;

    // We need to handle 5 cases here:
    //
    // - Approaching -maxVelocity from below
    // - Approaching -maxVelocity from above
    // - Approaching maxVelocity from below
    // - Approaching maxVelocity from above
    // - At +-maxVelocity
    //
    // For cases 1 and 3, we want to subtract epsilon from the inflection point
    // velocity For cases 2 and 4, we want to add epsilon to the inflection
    // point velocity. For case 5, we have reached inflection point velocity.
    auto epsilon = Velocity_t(1e-9);
    if (wpi::units::math::abs(u_dir * constraints.MaxVelocity() -
                              inflectionPoint.velocity) < epsilon) {
      auto solvableV = inflectionPoint.velocity;
      wpi::units::second_t t_to_solvable_v;
      Distance_t x_at_solvable_v;
      if (wpi::units::math::abs(current.velocity - inflectionPoint.velocity) <
          epsilon) {
        t_to_solvable_v = 0_s;
        x_at_solvable_v = current.position;
      } else {
        if (wpi::units::math::abs(current.velocity) >
            constraints.MaxVelocity()) {
          solvableV += u_dir * epsilon;
        } else {
          solvableV -= u_dir * epsilon;
        }

        t_to_solvable_v = ComputeTimeFromVelocity(constraints, solvableV, u,
                                                  current.velocity);
        x_at_solvable_v =
            ComputeDistanceFromVelocity(constraints, solvableV, u, current);
      }

      inflectionT_forward =
          t_to_solvable_v + u_dir *
                                (inflectionPoint.position - x_at_solvable_v) /
                                constraints.MaxVelocity();
    } else {
      inflectionT_forward = ComputeTimeFromVelocity(
          constraints, inflectionPoint.velocity, u, current.velocity);
    }

    auto inflectionT_backward = ComputeTimeFromVelocity(
        constraints, inflectionPoint.velocity, -u, goal.velocity);

    return {inflectionT_forward, inflectionT_forward - inflectionT_backward};
  }

  /**
   * Calculates the position reached after t seconds when applying an input from
   * the initial state.
   *
   * @param constraints The constraints on the profile.
   * @param t The time since the initial state.
   * @param input The signed input applied to this profile from the initial
   *     state.
   * @param initial The initial state.
   * @return The distance travelled by this profile.
   */
  static constexpr Distance_t ComputeDistanceFromTime(
      const Constraints& constraints, const wpi::units::second_t& time,
      const Input_t& input, const State& initial) {
    auto A = constraints.A;
    auto B = constraints.B;
    auto u = input;

    return initial.position +
           (-B * u * time + (initial.velocity + B * u / A) *
                                (wpi::units::math::exp(A * time) - 1)) /
               A;
  }

  /**
   * Calculates the velocity reached after t seconds when applying an input from
   * the initial state.
   *
   * @param constraints The constraints on the profile.
   * @param t The time since the initial state.
   * @param input The signed input applied to this profile from the initial
   *     state.
   * @param initial The initial state.
   * @return The distance travelled by this profile.
   */
  static constexpr Velocity_t ComputeVelocityFromTime(
      const Constraints& constraints, const wpi::units::second_t& time,
      const Input_t& input, const State& initial) {
    auto A = constraints.A;
    auto B = constraints.B;
    auto u = input;

    return (initial.velocity + B * u / A) * wpi::units::math::exp(A * time) -
           B * u / A;
  }

  /**
   * Calculates the time required to reach a specified velocity given the
   * initial velocity.
   *
   * @param constraints The constraints on the profile.
   * @param velocity The goal velocity.
   * @param input The signed input applied to this profile from the initial
   *     state.
   * @param initial The initial velocity.
   * @return The time required to reach the goal velocity.
   */
  static constexpr wpi::units::second_t ComputeTimeFromVelocity(
      const Constraints& constraints, const Velocity_t& velocity,
      const Input_t& input, const Velocity_t& initial) {
    auto A = constraints.A;
    auto B = constraints.B;
    auto u = input;

    return wpi::units::math::log((A * velocity + B * u) /
                                 (A * initial + B * u)) /
           A;
  }

  /**
   * Calculates the distance reached at the same time as the given velocity when
   * applying the given input from the initial state.
   *
   * @param constraints The constraints on the profile.
   * @param velocity The velocity reached by this profile
   * @param input The signed input applied to this profile from the initial
   *     state.
   * @param initial The initial state.
   * @return The distance reached when the given velocity is reached.
   */
  static constexpr Distance_t ComputeDistanceFromVelocity(
      const Constraints& constraints, const Velocity_t& velocity,
      const Input_t& input, const State& initial) {
    auto A = constraints.A;
    auto B = constraints.B;
    auto u = input;

    return initial.position + (velocity - initial.velocity) / A -
           B * u / (A * A) *
               wpi::units::math::log((A * velocity + B * u) /
                                     (A * initial.velocity + B * u));
  }

  /**
   * Calculates the velocity at which input should be reversed in order to reach
   * the goal state from the current state.
   *
   * @param constraints The constraints on the profile.
   * @param input The signed input applied to this profile from the current
   *     state.
   * @param current The current state.
   * @param goal The goal state.
   * @return The inflection velocity.
   */
  static constexpr Velocity_t SolveForInflectionVelocity(
      const Constraints& constraints, const Input_t& input,
      const State& current, const State& goal) {
    auto A = constraints.A;
    auto B = constraints.B;
    auto u = input;

    auto u_dir = u / wpi::units::math::abs(u);

    auto position_delta = goal.position - current.position;
    auto velocity_delta = goal.velocity - current.velocity;

    auto scalar = (A * current.velocity + B * u) * (A * goal.velocity - B * u);
    auto power = -A / B / u * (A * position_delta - velocity_delta);

    auto a = -A * A;
    auto c = B * B * u * u + scalar * wpi::units::math::exp(power);

    if (-1e-9 < c.value() && c.value() < 0) {
      // numeric instability - the heuristic gets it right but c is around
      // -1e-13
      return Velocity_t(0);
    }

    return u_dir * wpi::units::math::sqrt(-c / a);
  }

  /**
   * Returns true if the profile should be inverted.
   *
   * The profile is inverted if we should first apply negative input in order to
   * reach the goal state.
   *
   * @param constraints The constraints on the profile.
   * @param current The initial state (usually the current state).
   * @param goal The desired state when the profile is complete.
   */
  static constexpr bool ShouldFlipInput(const Constraints& constraints,
                                        const State& current,
                                        const State& goal) {
    auto u = constraints.maxInput;

    auto v0 = current.velocity;
    auto xf = goal.position;
    auto vf = goal.velocity;

    auto x_forward = ComputeDistanceFromVelocity(constraints, vf, u, current);
    auto x_reverse = ComputeDistanceFromVelocity(constraints, vf, -u, current);

    if (v0 >= constraints.MaxVelocity()) {
      return xf < x_reverse;
    }

    if (v0 <= -constraints.MaxVelocity()) {
      return xf < x_forward;
    }

    auto a = v0 >= Velocity_t(0);
    auto b = vf >= Velocity_t(0);
    auto c = xf >= x_forward;
    auto d = xf >= x_reverse;

    return (a && !d) || (b && !c) || (!c && !d);
  }

  Constraints m_constraints;
};

}  // namespace wpi::math
