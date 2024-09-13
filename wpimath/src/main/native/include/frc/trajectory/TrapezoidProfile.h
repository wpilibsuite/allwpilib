// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/deprecated.h>

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
 * TrapezoidProfile::Constraints constraints{kMaxV, kMaxA};
 * double previousProfiledReference = initialReference;
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
  using Distance_t = units::unit_t<Distance>;
  using Velocity =
      units::compound_unit<Distance, units::inverse<units::seconds>>;
  using Velocity_t = units::unit_t<Velocity>;
  using Acceleration =
      units::compound_unit<Velocity, units::inverse<units::seconds>>;
  using Acceleration_t = units::unit_t<Acceleration>;

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
    Constraints() {
      wpi::math::MathSharedStore::ReportUsage(
          wpi::math::MathUsageId::kTrajectory_TrapezoidProfile, 1);
    }

    /**
     * Constructs constraints for a Trapezoid Profile.
     *
     * @param maxVelocity Maximum velocity.
     * @param maxAcceleration Maximum acceleration.
     */
    Constraints(Velocity_t maxVelocity, Acceleration_t maxAcceleration)
        : maxVelocity{maxVelocity}, maxAcceleration{maxAcceleration} {
      wpi::math::MathSharedStore::ReportUsage(
          wpi::math::MathUsageId::kTrajectory_TrapezoidProfile, 1);
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

    bool operator==(const State&) const = default;
  };

  /**
   * Constructs a TrapezoidProfile.
   *
   * @param constraints The constraints on the profile, like maximum velocity.
   */
  TrapezoidProfile(Constraints constraints);  // NOLINT

  /**
   * Constructs a TrapezoidProfile.
   *
   * @param constraints The constraints on the profile, like maximum velocity.
   * @param goal        The desired state when the profile is complete.
   * @param initial     The initial state (usually the current state).
   * @deprecated Pass the desired and current state into calculate instead of
   * constructing a new TrapezoidProfile with the desired and current state
   */
  WPI_DEPRECATED(
      "Pass the desired and current state into calculate instead of "
      "constructing a new TrapezoidProfile with the desired and current "
      "state")
  TrapezoidProfile(Constraints constraints, State goal,
                   State initial = State{Distance_t{0}, Velocity_t{0}});

  TrapezoidProfile(const TrapezoidProfile&) = default;
  TrapezoidProfile& operator=(const TrapezoidProfile&) = default;
  TrapezoidProfile(TrapezoidProfile&&) = default;
  TrapezoidProfile& operator=(TrapezoidProfile&&) = default;

  /**
   * Calculates the position and velocity for the profile at a time t where the
   * current state is at time t = 0.
   *
   * @param t How long to advance from the current state toward the desired
   *     state.
   * @return The position and velocity of the profile at time t.
   * @deprecated Pass the desired and current state into calculate instead of
   * constructing a new TrapezoidProfile with the desired and current state
   */
  [[deprecated(
      "Pass the desired and current state into calculate instead of "
      "constructing a new TrapezoidProfile with the desired and current "
      "state")]]
  State Calculate(units::second_t t) const;

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
  State Calculate(units::second_t t, State current, State goal);

  /**
   * Returns the time left until a target distance in the profile is reached.
   *
   * @param target The target distance.
   * @return The time left until a target distance in the profile is reached.
   */
  units::second_t TimeLeftUntil(Distance_t target) const;

  /**
   * Returns the total time the profile takes to reach the goal.
   *
   * @return The total time the profile takes to reach the goal.
   */
  units::second_t TotalTime() const { return m_endDeccel; }

  /**
   * Returns true if the profile has reached the goal.
   *
   * The profile has reached the goal if the time since the profile started has
   * exceeded the profile's total time.
   *
   * @param t The time since the beginning of the profile.
   * @return True if the profile has reached the goal.
   */
  bool IsFinished(units::second_t t) const { return t >= TotalTime(); }

 private:
  /**
   * Returns true if the profile inverted.
   *
   * The profile is inverted if goal position is less than the initial position.
   *
   * @param initial The initial state (usually the current state).
   * @param goal The desired state when the profile is complete.
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
  State m_current;
  State m_goal;   // TODO: remove
  bool m_newAPI;  // TODO: remove

  units::second_t m_endAccel;
  units::second_t m_endFullSpeed;
  units::second_t m_endDeccel;
};
}  // namespace frc

#include "TrapezoidProfile.inc"
