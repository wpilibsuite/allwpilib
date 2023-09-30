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
 * the intended usage is to filter a reference's dynamics based on ExponentialProfile
 * velocity constraints. To compute the reference obeying this constraint, do
 * the following.
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
  using B_t = units::unit_t<units::compound_unit<Acceleration, units::inverse<Input>>>;

  class Constraints {
   public:
    Constraints(Input_t maxInput_, A_t A_, B_t B_)
        : maxInput{maxInput_}, A{A_}, B{B_} {
      wpi::math::MathSharedStore::ReportUsage(
          wpi::math::MathUsageId::kTrajectory_ExponentialProfile, 1);
    }
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

  /**
   * Construct a ExponentialProfile.
   *
   * @param constraints The constraints on the profile, like maximum input and and .
   * @param goal        The desired state when the profile is complete.
   * @param initial     The initial state (usually the current state).
   */
  ExponentialProfile(Constraints constraints, State goal,
                   State initial = State{Distance_t{0}, Velocity_t{0}});

  ExponentialProfile(const ExponentialProfile&) = default;
  ExponentialProfile& operator=(const ExponentialProfile&) = default;
  ExponentialProfile(ExponentialProfile&&) = default;
  ExponentialProfile& operator=(ExponentialProfile&&) = default;

  /**
   * Calculate the correct position and velocity for the profile at a time t
   * where the beginning of the profile was at time t = 0.
   *
   * @param t The time since the beginning of the profile.
   */
  State Calculate(const units::second_t &t) const;

  /**
   * Returns the total time the profile takes to reach the goal.
   */
  units::second_t TotalTime() const { return m_totalTime; }

  /**
   * Returns the time at which the profile reverses input.
   */
  units::second_t InflectionTime() const { return m_inflectionTime; }

  /**
   * Returns the state the profile is in when input is reversed.
   */
  State InflectionState() const { return m_inflectionPoint; }

  /**
   * Returns true if the profile has reached the goal.
   *
   * The profile has reached the goal if the time since the profile started
   * has exceeded the profile's total time.
   *
   * @param t The time since the beginning of the profile.
   */
  bool IsFinished(units::second_t t) const { return t >= TotalTime(); }

 private:
  /**
   * Returns true if the profile inverted.
   *
   * The profile is inverted if goal state is more quickly achieved by starting with negative input than with positive input.
   */
  bool ShouldFlipInput() const;

  Velocity_t SolveForInflectionVelocity(const Input_t &input) const;

  Velocity_t VelocityAtTime(const units::second_t& time, const Input_t& input) const;
  Distance_t DistanceAtTime(const units::second_t& time, const Input_t& input) const;

  Distance_t DistanceForVelocityOnForwardPass(const Velocity_t& velocity, const Input_t &input) const;
  Distance_t DistanceForVelocityOnBackwardPass(const Velocity_t& velocity, const Input_t &input) const;

  // The direction of the profile, either 1 for forwards or -1 for inverted
  int m_direction;

  Constraints m_constraints;
  State m_initial;
  State m_goal;

  units::second_t m_inflectionTime;
  State m_inflectionPoint;
  units::second_t m_totalTime;
  
};
}  // namespace frc

#include "ExponentialProfile.inc"
