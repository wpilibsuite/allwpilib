// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "units/time.h"
#include "wpimath/MathShared.h"

namespace frc {

/**
 * A Exponential-shaped velocity profile.
 *
 * While this class can be used for a profiled movement from start to finish,
 * the intended usage is to filter a reference's dynamics based on
 * ExponentialProfile velocity constraints. To compute the reference obeying
 * this constraint, do the following.
 *
 * Initialization:
 * @code{.cpp}
 * ExponentialProfile::Constraints constraints{kMaxV, kV, kA};
 * State previousProfiledReference = {initialReference, 0_mps};
 * @endcode
 *
 * Run on update:
 * @code{.cpp}
 * previousProfiledReference = profile.Calculate(timeSincePreviousUpdate,
 * previousProfiledReference, unprofiledReference);
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
template <class Distance, class Input>
class ExponentialProfile {
 public:
  using Distance_t = units::unit_t<Distance>;
  using Velocity =
      units::compound_unit<Distance, units::inverse<units::seconds>>;
  using Velocity_t = units::unit_t<Velocity>;
  using Acceleration =
      units::compound_unit<Velocity, units::inverse<units::seconds>>;
  using Input_t = units::unit_t<Input>;
  using A_t = units::unit_t<units::inverse<units::seconds>>;
  using B_t =
      units::unit_t<units::compound_unit<Acceleration, units::inverse<Input>>>;
  using KV = units::compound_unit<Input, units::inverse<Velocity>>;
  using kV_t = units::unit_t<KV>;
  using KA = units::compound_unit<Input, units::inverse<Acceleration>>;
  using kA_t = units::unit_t<KA>;

  /**
   * Profile timing.
   */
  class ProfileTiming {
   public:
    /// Profile inflection time.
    units::second_t inflectionTime;

    /// Total profile time.
    units::second_t totalTime;

    /**
     * Decides if the profile is finished by time t.
     *
     * @param t The time since the beginning of the profile.
     * @return if the profile is finished at time t.
     */
    bool IsFinished(const units::second_t& t) const { return t >= totalTime; }
  };

  /**
   * Profile constraints.
   */
  class Constraints {
   public:
    /**
     * Construct constraints for an ExponentialProfile.
     *
     * @param maxInput maximum unsigned input voltage
     * @param A The State-Space 1x1 system matrix.
     * @param B The State-Space 1x1 input matrix.
     */
    Constraints(Input_t maxInput, A_t A, B_t B)
        : maxInput{maxInput}, A{A}, B{B} {}

    /**
     * Construct constraints for an ExponentialProfile from characteristics.
     *
     * @param maxInput maximum unsigned input voltage
     * @param kV The velocity gain.
     * @param kA The acceleration gain.
     */
    Constraints(Input_t maxInput, kV_t kV, kA_t kA)
        : maxInput{maxInput}, A{-kV / kA}, B{1 / kA} {}

    /**
     * Computes the max achievable velocity for an Exponential Profile.
     *
     * @return The seady-state velocity achieved by this profile.
     */
    Velocity_t MaxVelocity() const { return -maxInput * B / A; }

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

    bool operator==(const State&) const = default;
  };

  /**
   * Construct a ExponentialProfile.
   *
   * @param constraints The constraints on the profile, like maximum input.
   */
  explicit ExponentialProfile(Constraints constraints);

  ExponentialProfile(const ExponentialProfile&) = default;
  ExponentialProfile& operator=(const ExponentialProfile&) = default;
  ExponentialProfile(ExponentialProfile&&) = default;
  ExponentialProfile& operator=(ExponentialProfile&&) = default;

  /**
   * Calculate the correct position and velocity for the profile at a time t
   * where the current state is at time t = 0.
   */
  State Calculate(const units::second_t& t, const State& current,
                  const State& goal) const;

  /**
   * Calculate the point after which the fastest way to reach the goal state is
   * to apply input in the opposite direction.
   */
  State CalculateInflectionPoint(const State& current, const State& goal) const;

  /**
   * Calculate the time it will take for this profile to reach the goal state.
   */
  units::second_t TimeLeftUntil(const State& current, const State& goal) const;

  /**
   * Calculate the time it will take for this profile to reach the inflection
   * point, and the time it will take for this profile to reach the goal state.
   */
  ProfileTiming CalculateProfileTiming(const State& current,
                                       const State& goal) const;

 private:
  /**
   * Calculate the point after which the fastest way to reach the goal state is
   * to apply input in the opposite direction.
   */
  State CalculateInflectionPoint(const State& current, const State& goal,
                                 const Input_t& input) const;

  /**
   * Calculate the time it will take for this profile to reach the inflection
   * point, and the time it will take for this profile to reach the goal state.
   */
  ProfileTiming CalculateProfileTiming(const State& current,
                                       const State& inflectionPoint,
                                       const State& goal,
                                       const Input_t& input) const;

  /**
   * Calculate the velocity reached after t seconds when applying an input from
   * the initial state.
   */
  Velocity_t ComputeVelocityFromTime(const units::second_t& time,
                                     const Input_t& input,
                                     const State& initial) const;

  /**
   * Calculate the position reached after t seconds when applying an input from
   * the initial state.
   */
  Distance_t ComputeDistanceFromTime(const units::second_t& time,
                                     const Input_t& input,
                                     const State& initial) const;

  /**
   * Calculate the distance reached at the same time as the given velocity when
   * applying the given input from the initial state.
   */
  Distance_t ComputeDistanceFromVelocity(const Velocity_t& velocity,
                                         const Input_t& input,
                                         const State& initial) const;

  /**
   * Calculate the time required to reach a specified velocity given the initial
   * velocity.
   */
  units::second_t ComputeTimeFromVelocity(const Velocity_t& velocity,
                                          const Input_t& input,
                                          const Velocity_t& initial) const;

  /**
   * Calculate the velocity at which input should be reversed in order to reach
   * the goal state from the current state.
   */
  Velocity_t SolveForInflectionVelocity(const Input_t& input,
                                        const State& current,
                                        const State& goal) const;

  /**
   * Returns true if the profile should be inverted.
   *
   * <p>The profile is inverted if we should first apply negative input in order
   * to reach the goal state.
   */
  bool ShouldFlipInput(const State& current, const State& goal) const;

  Constraints m_constraints;
};
}  // namespace frc

#include "ExponentialProfile.inc"
