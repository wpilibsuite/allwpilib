/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <algorithm>

#include <units/units.h>

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
  using Distance_t = units::unit_t<Distance>;
  using Velocity =
      units::compound_unit<Distance, units::inverse<units::seconds>>;
  using Velocity_t = units::unit_t<Velocity>;
  using Acceleration =
      units::compound_unit<Velocity, units::inverse<units::seconds>>;
  using Acceleration_t = units::unit_t<Acceleration>;

 public:
  class Constraints {
   public:
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
                   State initial = State{Distance_t(0), Velocity_t(0)})
      : m_direction{ShouldFlipAcceleration(initial, goal) ? -1 : 1},
        m_constraints(constraints),
        m_initial(Direct(initial)),
        m_goal(Direct(goal)) {
    if (m_initial.velocity > m_constraints.maxVelocity) {
      m_initial.velocity = m_constraints.maxVelocity;
    }

    // Deal with a possibly truncated motion profile (with nonzero initial or
    // final velocity) by calculating the parameters as if the profile began and
    // ended at zero velocity
    units::second_t cutoffBegin =
        m_initial.velocity / m_constraints.maxAcceleration;
    Distance_t cutoffDistBegin =
        cutoffBegin * cutoffBegin * m_constraints.maxAcceleration / 2.0;

    units::second_t cutoffEnd = m_goal.velocity / m_constraints.maxAcceleration;
    Distance_t cutoffDistEnd =
        cutoffEnd * cutoffEnd * m_constraints.maxAcceleration / 2.0;

    // Now we can calculate the parameters as if it was a full trapezoid instead
    // of a truncated one

    Distance_t fullTrapezoidDist = cutoffDistBegin +
                                   (m_goal.position - m_initial.position) +
                                   cutoffDistEnd;
    units::second_t accelerationTime =
        m_constraints.maxVelocity / m_constraints.maxAcceleration;

    Distance_t fullSpeedDist =
        fullTrapezoidDist -
        accelerationTime * accelerationTime * m_constraints.maxAcceleration;

    // Handle the case where the profile never reaches full speed
    if (fullSpeedDist < Distance_t(0)) {
      accelerationTime =
          units::math::sqrt(fullTrapezoidDist / m_constraints.maxAcceleration);
      fullSpeedDist = Distance_t(0);
    }

    m_endAccel = accelerationTime - cutoffBegin;
    m_endFullSpeed = m_endAccel + fullSpeedDist / m_constraints.maxVelocity;
    m_endDeccel = m_endFullSpeed + accelerationTime - cutoffEnd;
  }

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
  State Calculate(units::second_t t) const {
    State result = m_initial;

    if (t < m_endAccel) {
      result.velocity += t * m_constraints.maxAcceleration;
      result.position +=
          (m_initial.velocity + t * m_constraints.maxAcceleration / 2.0) * t;
    } else if (t < m_endFullSpeed) {
      result.velocity = m_constraints.maxVelocity;
      result.position += (m_initial.velocity +
                          m_endAccel * m_constraints.maxAcceleration / 2.0) *
                             m_endAccel +
                         m_constraints.maxVelocity * (t - m_endAccel);
    } else if (t <= m_endDeccel) {
      result.velocity =
          m_goal.velocity + (m_endDeccel - t) * m_constraints.maxAcceleration;
      units::second_t timeLeft = m_endDeccel - t;
      result.position =
          m_goal.position -
          (m_goal.velocity + timeLeft * m_constraints.maxAcceleration / 2.0) *
              timeLeft;
    } else {
      result = m_goal;
    }

    return Direct(result);
  }

  /**
   * Returns the time left until a target distance in the profile is reached.
   *
   * @param target The target distance.
   */
  units::second_t TimeLeftUntil(Distance_t target) const {
    Distance_t position = m_initial.position * m_direction;
    Velocity_t velocity = m_initial.velocity * m_direction;

    units::second_t endAccel = m_endAccel * m_direction;
    units::second_t endFullSpeed = m_endFullSpeed * m_direction - endAccel;

    if (target < position) {
      endAccel *= -1.0;
      endFullSpeed *= -1.0;
      velocity *= -1.0;
    }

    endAccel = units::math::max(endAccel, 0_s);
    endFullSpeed = units::math::max(endFullSpeed, 0_s);
    units::second_t endDeccel = m_endDeccel - endAccel - endFullSpeed;
    endDeccel = units::math::max(endDeccel, 0_s);

    const Acceleration_t acceleration = m_constraints.maxAcceleration;
    const Acceleration_t decceleration = -m_constraints.maxAcceleration;

    Distance_t distToTarget = units::math::abs(target - position);

    if (distToTarget < Distance_t(1e-6)) {
      return 0_s;
    }

    Distance_t accelDist =
        velocity * endAccel + 0.5 * acceleration * endAccel * endAccel;

    Velocity_t deccelVelocity;
    if (endAccel > 0_s) {
      deccelVelocity = units::math::sqrt(
          units::math::abs(velocity * velocity + 2 * acceleration * accelDist));
    } else {
      deccelVelocity = velocity;
    }

    Distance_t deccelDist = deccelVelocity * endDeccel +
                            0.5 * decceleration * endDeccel * endDeccel;

    deccelDist = units::math::max(deccelDist, Distance_t(0));

    Distance_t fullSpeedDist = m_constraints.maxVelocity * endFullSpeed;

    if (accelDist > distToTarget) {
      accelDist = distToTarget;
      fullSpeedDist = Distance_t(0);
      deccelDist = Distance_t(0);
    } else if (accelDist + fullSpeedDist > distToTarget) {
      fullSpeedDist = distToTarget - accelDist;
      deccelDist = Distance_t(0);
    } else {
      deccelDist = distToTarget - fullSpeedDist - accelDist;
    }

    units::second_t accelTime =
        (-velocity + units::math::sqrt(units::math::abs(
                         velocity * velocity + 2 * acceleration * accelDist))) /
        acceleration;

    units::second_t deccelTime =
        (-deccelVelocity +
         units::math::sqrt(units::math::abs(deccelVelocity * deccelVelocity +
                                            2 * decceleration * deccelDist))) /
        decceleration;

    units::second_t fullSpeedTime = fullSpeedDist / m_constraints.maxVelocity;

    return accelTime + fullSpeedTime + deccelTime;
  }

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
