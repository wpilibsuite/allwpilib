// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "units/math.h"
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
  TrapezoidProfile(Constraints constraints)  // NOLINT
      : m_constraints(constraints) {}

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
   * @param current The current state.
   * @param goal The desired state when the profile is complete.
   * @return The position and velocity of the profile at time t.
   */
  State Calculate(units::second_t t, State current, State goal) {
    m_direction = ShouldFlipAcceleration(current, goal) ? -1 : 1;
    m_current = Direct(current);
    goal = Direct(goal);
    if (m_current.velocity > m_constraints.maxVelocity) {
      m_current.velocity = m_constraints.maxVelocity;
    }

    // Deal with a possibly truncated motion profile (with nonzero initial or
    // final velocity) by calculating the parameters as if the profile began and
    // ended at zero velocity
    units::second_t cutoffBegin =
        m_current.velocity / m_constraints.maxAcceleration;
    Distance_t cutoffDistBegin =
        cutoffBegin * cutoffBegin * m_constraints.maxAcceleration / 2.0;

    units::second_t cutoffEnd = goal.velocity / m_constraints.maxAcceleration;
    Distance_t cutoffDistEnd =
        cutoffEnd * cutoffEnd * m_constraints.maxAcceleration / 2.0;

    // Now we can calculate the parameters as if it was a full trapezoid instead
    // of a truncated one

    Distance_t fullTrapezoidDist =
        cutoffDistBegin + (goal.position - m_current.position) + cutoffDistEnd;
    units::second_t accelerationTime =
        m_constraints.maxVelocity / m_constraints.maxAcceleration;

    Distance_t fullSpeedDist =
        fullTrapezoidDist -
        accelerationTime * accelerationTime * m_constraints.maxAcceleration;

    // Handle the case where the profile never reaches full speed
    if (fullSpeedDist < Distance_t{0}) {
      accelerationTime =
          units::math::sqrt(fullTrapezoidDist / m_constraints.maxAcceleration);
      fullSpeedDist = Distance_t{0};
    }

    m_endAccel = accelerationTime - cutoffBegin;
    m_endFullSpeed = m_endAccel + fullSpeedDist / m_constraints.maxVelocity;
    m_endDecel = m_endFullSpeed + accelerationTime - cutoffEnd;
    State result = m_current;

    if (t < m_endAccel) {
      result.velocity += t * m_constraints.maxAcceleration;
      result.position +=
          (m_current.velocity + t * m_constraints.maxAcceleration / 2.0) * t;
    } else if (t < m_endFullSpeed) {
      result.velocity = m_constraints.maxVelocity;
      result.position += (m_current.velocity +
                          m_endAccel * m_constraints.maxAcceleration / 2.0) *
                             m_endAccel +
                         m_constraints.maxVelocity * (t - m_endAccel);
    } else if (t <= m_endDecel) {
      result.velocity =
          goal.velocity + (m_endDecel - t) * m_constraints.maxAcceleration;
      units::second_t timeLeft = m_endDecel - t;
      result.position =
          goal.position -
          (goal.velocity + timeLeft * m_constraints.maxAcceleration / 2.0) *
              timeLeft;
    } else {
      result = goal;
    }

    return Direct(result);
  }

  /**
   * Returns the time left until a target distance in the profile is reached.
   *
   * @param target The target distance.
   * @return The time left until a target distance in the profile is reached.
   */
  units::second_t TimeLeftUntil(Distance_t target) const {
    Distance_t position = m_current.position * m_direction;
    Velocity_t velocity = m_current.velocity * m_direction;

    units::second_t endAccel = m_endAccel * m_direction;
    units::second_t endFullSpeed = m_endFullSpeed * m_direction - endAccel;

    if (target < position) {
      endAccel *= -1.0;
      endFullSpeed *= -1.0;
      velocity *= -1.0;
    }

    endAccel = units::math::max(endAccel, 0_s);
    endFullSpeed = units::math::max(endFullSpeed, 0_s);

    const Acceleration_t acceleration = m_constraints.maxAcceleration;
    const Acceleration_t deceleration = -m_constraints.maxAcceleration;

    Distance_t distToTarget = units::math::abs(target - position);

    if (distToTarget < Distance_t{1e-6}) {
      return 0_s;
    }

    Distance_t accelDist =
        velocity * endAccel + 0.5 * acceleration * endAccel * endAccel;

    Velocity_t decelVelocity;
    if (endAccel > 0_s) {
      decelVelocity = units::math::sqrt(
          units::math::abs(velocity * velocity + 2 * acceleration * accelDist));
    } else {
      decelVelocity = velocity;
    }

    Distance_t fullSpeedDist = m_constraints.maxVelocity * endFullSpeed;
    Distance_t decelDist;

    if (accelDist > distToTarget) {
      accelDist = distToTarget;
      fullSpeedDist = Distance_t{0};
      decelDist = Distance_t{0};
    } else if (accelDist + fullSpeedDist > distToTarget) {
      fullSpeedDist = distToTarget - accelDist;
      decelDist = Distance_t{0};
    } else {
      decelDist = distToTarget - fullSpeedDist - accelDist;
    }

    units::second_t accelTime =
        (-velocity + units::math::sqrt(units::math::abs(
                         velocity * velocity + 2 * acceleration * accelDist))) /
        acceleration;

    units::second_t decelTime =
        (-decelVelocity +
         units::math::sqrt(units::math::abs(decelVelocity * decelVelocity +
                                            2 * deceleration * decelDist))) /
        deceleration;

    units::second_t fullSpeedTime = fullSpeedDist / m_constraints.maxVelocity;

    return accelTime + fullSpeedTime + decelTime;
  }

  /**
   * Returns the total time the profile takes to reach the goal.
   *
   * @return The total time the profile takes to reach the goal.
   */
  units::second_t TotalTime() const { return m_endDecel; }

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

  units::second_t m_endAccel;
  units::second_t m_endFullSpeed;
  units::second_t m_endDecel;
};

}  // namespace frc
