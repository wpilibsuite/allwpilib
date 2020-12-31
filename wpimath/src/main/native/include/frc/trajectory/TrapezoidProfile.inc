// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <algorithm>

#include "frc/trajectory/TrapezoidProfile.h"
#include "units/math.h"

namespace frc {
template <class Distance>
TrapezoidProfile<Distance>::TrapezoidProfile(Constraints constraints,
                                             State goal, State initial)
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

  Distance_t fullTrapezoidDist =
      cutoffDistBegin + (m_goal.position - m_initial.position) + cutoffDistEnd;
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

template <class Distance>
typename TrapezoidProfile<Distance>::State
TrapezoidProfile<Distance>::Calculate(units::second_t t) const {
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

template <class Distance>
units::second_t TrapezoidProfile<Distance>::TimeLeftUntil(
    Distance_t target) const {
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

  Distance_t deccelDist =
      deccelVelocity * endDeccel + 0.5 * decceleration * endDeccel * endDeccel;

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
}  // namespace frc
