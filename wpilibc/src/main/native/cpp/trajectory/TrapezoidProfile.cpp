/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/trajectory/TrapezoidProfile.h"

using namespace frc;

TrapezoidProfile::TrapezoidProfile(Constraints constraints, State goal,
                                   State initial)
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
  units::meter_t cutoffDistBegin =
      cutoffBegin * cutoffBegin * m_constraints.maxAcceleration / 2.0;

  units::second_t cutoffEnd = m_goal.velocity / m_constraints.maxAcceleration;
  units::meter_t cutoffDistEnd =
      cutoffEnd * cutoffEnd * m_constraints.maxAcceleration / 2.0;

  // Now we can calculate the parameters as if it was a full trapezoid instead
  // of a truncated one

  units::meter_t fullTrapezoidDist =
      cutoffDistBegin + (m_goal.position - m_initial.position) + cutoffDistEnd;
  units::second_t accelerationTime =
      m_constraints.maxVelocity / m_constraints.maxAcceleration;

  units::meter_t fullSpeedDist =
      fullTrapezoidDist -
      accelerationTime * accelerationTime * m_constraints.maxAcceleration;

  // Handle the case where the profile never reaches full speed
  if (fullSpeedDist < 0_m) {
    accelerationTime =
        units::math::sqrt(fullTrapezoidDist / m_constraints.maxAcceleration);
    fullSpeedDist = 0_m;
  }

  m_endAccel = accelerationTime - cutoffBegin;
  m_endFullSpeed = m_endAccel + fullSpeedDist / m_constraints.maxVelocity;
  m_endDeccel = m_endFullSpeed + accelerationTime - cutoffEnd;
}

TrapezoidProfile::State TrapezoidProfile::Calculate(units::second_t t) const {
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

units::second_t TrapezoidProfile::TimeLeftUntil(units::meter_t target) const {
  units::meter_t position = m_initial.position * m_direction;
  units::meters_per_second_t velocity = m_initial.velocity * m_direction;

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

  const units::meters_per_second_squared_t acceleration =
      m_constraints.maxAcceleration;
  const units::meters_per_second_squared_t decceleration =
      -m_constraints.maxAcceleration;

  units::meter_t distToTarget = units::math::abs(target - position);

  if (distToTarget < 1e-6_m) {
    return 0_s;
  }

  units::meter_t accelDist =
      velocity * endAccel + 0.5 * acceleration * endAccel * endAccel;

  units::meters_per_second_t deccelVelocity;
  if (endAccel > 0_s) {
    deccelVelocity = units::math::sqrt(
        units::math::abs(velocity * velocity + 2 * acceleration * accelDist));
  } else {
    deccelVelocity = velocity;
  }

  units::meter_t deccelDist =
      deccelVelocity * endDeccel + 0.5 * decceleration * endDeccel * endDeccel;

  deccelDist = units::math::max(deccelDist, 0_m);

  units::meter_t fullSpeedDist = m_constraints.maxVelocity * endFullSpeed;

  if (accelDist > distToTarget) {
    accelDist = distToTarget;
    fullSpeedDist = 0_m;
    deccelDist = 0_m;
  } else if (accelDist + fullSpeedDist > distToTarget) {
    fullSpeedDist = distToTarget - accelDist;
    deccelDist = 0_m;
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
