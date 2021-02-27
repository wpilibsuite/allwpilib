// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "units/time.h"
#include "wpimath/MathShared.h"

namespace frc {

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
 * TrapezoidalMotionProfile::Constraints constraints{kMaxV, kMaxA};
 * double previousProfiledReference = initialReference;
 * @endcode
 *
 * Run on update:
 * @code{.cpp}
 * TrapezoidalMotionProfile profile{constraints, unprofiledReference,
 *                                  previousProfiledReference};
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
template <class Distance>
class TrapezoidProfile {
 public:
  using Distance_t = units::unit_t<Distance>;
  using Velocity =
      units::compound_unit<Distance, units::inverse<units::seconds>>;
  using Velocity_t = units::unit_t<Velocity>;
  using Acceleration =
      units::compound_unit<Velocity, units::inverse<units::seconds>>;
  using Acceleration_t = units::unit_t<Acceleration>;

  class Constraints {
   public:
    Constraints() {
      wpi::math::MathSharedStore::ReportUsage(
          wpi::math::MathUsageId::kTrajectory_TrapezoidProfile, 1);
    }
    Constraints(Velocity_t maxVelocity_, Acceleration_t maxAcceleration_)
        : maxVelocity{maxVelocity_}, maxAcceleration{maxAcceleration_} {
      wpi::math::MathSharedStore::ReportUsage(
          wpi::math::MathUsageId::kTrajectory_TrapezoidProfile, 1);
    }
    Velocity_t maxVelocity{0};
    Acceleration_t maxAcceleration{0};
  };

  class State {
   public:
    Distance_t position{0};
    Velocity_t velocity{0};
    bool operator==(const State& rhs) const {
      return position == rhs.position && velocity == rhs.velocity;
    }
    bool operator!=(const State& rhs) const { return !(*this == rhs); }
  };

  /**
   * Construct a TrapezoidProfile.
   *
   * @param constraints The constraints on the profile, like maximum velocity.
   * @param goal        The desired state when the profile is complete.
   * @param initial     The initial state (usually the current state).
   */
  TrapezoidProfile(Constraints constraints, State goal,
                   State initial = State{Distance_t(0), Velocity_t(0)});

  TrapezoidProfile(const TrapezoidProfile&) = default;
  TrapezoidProfile& operator=(const TrapezoidProfile&) = default;
  TrapezoidProfile(TrapezoidProfile&&) = default;
  TrapezoidProfile& operator=(TrapezoidProfile&&) = default;

  /**
   * Calculate the correct position and velocity for the profile at a time t
   * where the beginning of the profile was at time t = 0.
   *
   * @param t The time since the beginning of the profile.
   */
  State Calculate(units::second_t t) const;

  /**
   * Returns the time left until a target distance in the profile is reached.
   *
   * @param target The target distance.
   */
  units::second_t TimeLeftUntil(Distance_t target) const;

  /**
   * Returns the total time the profile takes to reach the goal.
   */
  units::second_t TotalTime() const { return m_endDeccel; }

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
   * The profile is inverted if goal position is less than the initial position.
   *
   * @param initial The initial state (usually the current state).
   * @param goal    The desired state when the profile is complete.
   */
  static bool ShouldFlipAcceleration(const State& initial, const State& goal) {
    return initial.position > goal.position;
  }

  // Flip the sign of the velocity and position if the profile is inverted
  State Direct(const State& in) const {
    State result = in;
    result.position *= m_direction;
    result.velocity *= m_direction;
    return result;
  }

  // The direction of the profile, either 1 for forwards or -1 for inverted
  int m_direction;

  Constraints m_constraints;
  State m_initial;
  State m_goal;

  units::second_t m_endAccel;
  units::second_t m_endFullSpeed;
  units::second_t m_endDeccel;
};
}  // namespace frc

#include "TrapezoidProfile.inc"
