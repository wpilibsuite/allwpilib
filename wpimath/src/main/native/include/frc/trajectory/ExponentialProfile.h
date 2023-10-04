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
 * ExponentialProfile::Constraints constraints{kMaxV, -kV/kA, 1/kA};
 * State previousProfiledReference = initialReference;
 * @endcode
 *
 * Run on update:
 * @code{.cpp}
 * ExponentialProfile profile{constraints, unprofiledReference,
 *                          previousProfiledReference};
 * previousProfiledReference = profile.Calculate(timeSincePreviousUpdate);
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

  class Constraints {
   public:
    Constraints(Input_t maxInput_, A_t A_, B_t B_)
        : maxInput{maxInput_}, A{A_}, B{B_} {}
    Constraints(Input_t maxInput_, kV_t kV_, kA_t kA_)
        : maxInput{maxInput_}, A{-kV_ / kA_}, B{1 / kA_} {}
    Velocity_t MaxVelocity() const { return -maxInput * B / A; }

    Input_t maxInput{0};
    A_t A{0};
    B_t B{0};
  };

  class State {
   public:
    Distance_t position{0};
    Velocity_t velocity{0};
    bool operator==(const State&) const = default;
  };

  class ProfileTiming {
    public:
      units::second_t inflectionTime;
      units::second_t totalTime;

      bool IsFinished(const units::second_t &time) const {
        return time > totalTime;
      }
  };

  /**
   * Construct a ExponentialProfile.
   *
   * @param constraints The constraints on the profile, like maximum input and
   * and .
   * @param goal        The desired state when the profile is complete.
   * @param initial     The initial state (usually the current state).
   */
  ExponentialProfile(Constraints constraints);

  ExponentialProfile(const ExponentialProfile&) = default;
  ExponentialProfile& operator=(const ExponentialProfile&) = default;
  ExponentialProfile(ExponentialProfile&&) = default;
  ExponentialProfile& operator=(ExponentialProfile&&) = default;

  /**
   * Calculate the instantaneous input to apply at time t in order to follow
   * this profile.
   */
  Input_t CalculateInput(const units::second_t& t, const State &current, const State &goal) const;

  /**
   * Calculate the correct position and velocity for the profile at a time t
   * where the beginning of the profile was at time t = 0.
   */
  State Calculate(const units::second_t& t, const State &current, const State &goal) const;

  /**
   * Calculate the correct position and velocity for the profile at a time t
   * where the beginning of the profile was at time t = 0.
   */
  State CalculateInflectionPoint(const State& current, const State &goal) const;

  units::second_t TimeLeftUntil(const State &current, const State &goal) const;

  ProfileTiming CalculateProfileTiming(const State &current, const State &goal) const;

 private:
  /**
   * Calculate the correct position and velocity for the profile at a time t
   * where the beginning of the profile was at time t = 0.
   */
  State CalculateInflectionPoint(const State& current, const State &goal, const Input_t &input) const;

  ProfileTiming CalculateProfileTiming(const State &current, const State &inflectionPoint, const State &goal, const Input_t &input) const;

  /**
   * Returns the velocity of the profile at the given input time and signed
   * input.
   */
  Velocity_t ComputeVelocityFromTime(const units::second_t& time, const Input_t& input, const State &initial) const;

  /**
   * Returns the position of the profile at the given input time and signed
   * input.
   */
  Distance_t ComputeDistanceFromTime(const units::second_t& time,
                            const Input_t& input, const State &initial) const;

  /**
   * Solve the Phase-space equation x(v, U) where x is the trajectory taken from
   * (x0, v0) with a given signed input.
   */
  Distance_t ComputeDistanceFromVelocity(const Velocity_t& velocity,
                                                const Input_t& input,
                                                const State& initial) const;

  /**
   * Solve the equation v(t, U) = v for t where v(0) = v0.
   */
 units::second_t ComputeTimeFromVelocity(
      const Velocity_t& velocity, const Input_t& input,
      const Velocity_t& initial) const;


  /**
   * Returns the velocity at which the profile reverses input.
   */
  Velocity_t SolveForInflectionVelocity(const Input_t& input, const State &current, const State &goal) const;

    /**
   * Returns true if the profile inverted - ie, -maxInput is applied
   * immediately, instead of maxInput.
   *
   * The profile is inverted if goal state is more quickly achieved by starting
   * with negative input than with positive input.
   */
  bool ShouldFlipInput(const State& current, const State& goal) const;

  Constraints m_constraints;
};
}  // namespace frc

#include "ExponentialProfile.inc"
