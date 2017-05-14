/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "CtrlSys/TrapezoidProfile.h"

#include <cmath>

using namespace frc;

TrapezoidProfile::TrapezoidProfile(double maxV, double timeToMaxV) {
  SetMaxVelocity(maxV);
  SetTimeToMaxV(timeToMaxV);
}

/**
 * Sets goal state of profile.
 *
 * @param goal a distance to which to travel
 * @param currentSource the current position
 */
void TrapezoidProfile::SetGoal(double goal, double currentSource) {
  std::lock_guard<std::mutex> lock(m_mutex);

  // Subtract current source for profile calculations
  m_goal = goal - currentSource;

  // Set setpoint to current distance since setpoint hasn't moved yet
  m_ref = std::make_tuple(currentSource, 0.0, 0.0);

  if (m_goal < 0.0) {
    m_sign = -1.0;
  } else {
    m_sign = 1.0;
  }
  m_timeToMaxVelocity = m_velocity / m_acceleration;

  /* d is distance traveled when accelerating to/from max velocity
   *       = 1/2 * (v0 + v) * t
   * t is m_timeToMaxVelocity
   * delta is distance traveled at max velocity
   * delta = totalDist - 2 * d
   *       = setpoint - 2 * ((v0 + v)/2 * t)
   * v0 = 0 therefore:
   * delta = setpoint - 2 * (v/2 * t)
   *       = setpoint - v * t
   *       = m_setpoint - m_velocity * m_timeToMaxVelocity
   *
   * t is time at maximum velocity
   * t = delta (from previous comment) / m_velocity (where m_velocity is maximum
   * velocity)
   *   = (m_setpoint - m_velocity * m_timeToMaxVelocity) / m_velocity
   *   = m_setpoint/m_velocity - m_timeToMaxVelocity
   */
  double timeAtMaxV = m_sign * m_goal / m_velocity - m_timeToMaxVelocity;

  /* if ( 1/2 * a * t^2 > m_setpoint / 2 ) // if distance travelled before
   *     reaching maximum speed is more than half of the total distance to
   *     travel
   * if ( a * t^2 > m_setpoint )
   * if ( a * (v/a)^2 > m_setpoint )
   * if ( a * v^2/a^2 > m_setpoint )
   * if ( v^2/a > m_setpoint )
   * if ( v * v/a > m_setpoint )
   * if ( v * m_timeToMaxVelocity > m_setpoint )
   */
  if (m_velocity * m_timeToMaxVelocity > m_sign * m_goal) {
    /* Solve for t:
     * 1/2 * a * t^2 = m_setpoint/2
     * a * t^2 = m_setpoint
     * t^2 = m_setpoint / a
     * t = std::sqrt( m_setpoint / a )
     */
    m_timeToMaxVelocity = std::sqrt(m_sign * m_goal / m_acceleration);
    m_timeFromMaxVelocity = m_timeToMaxVelocity;
    m_timeTotal = 2 * m_timeToMaxVelocity;
    m_profileMaxVelocity = m_acceleration * m_timeToMaxVelocity;
  } else {
    m_timeFromMaxVelocity = m_timeToMaxVelocity + timeAtMaxV;
    m_timeTotal = m_timeFromMaxVelocity + m_timeToMaxVelocity;
    m_profileMaxVelocity = m_velocity;
  }

  // Restore desired goal
  m_goal = goal;

  Reset();
}
/**
 * Sets maximum velocity of profile.
 */
void TrapezoidProfile::SetMaxVelocity(double velocity) {
  std::lock_guard<std::mutex> lock(m_mutex);
  m_velocity = velocity;
}

/**
 * Returns maximum velocity of profile.
 */
double TrapezoidProfile::GetMaxVelocity() const {
  std::lock_guard<std::mutex> lock(m_mutex);
  return m_velocity;
}

/**
 * Sets time to max velocity of profile from rest.
 */
void TrapezoidProfile::SetTimeToMaxV(double timeToMaxV) {
  std::lock_guard<std::mutex> lock(m_mutex);
  m_acceleration = m_velocity / timeToMaxV;
}

MotionProfile::State TrapezoidProfile::UpdateSetpoint(double currentTime) {
  if (currentTime < m_timeToMaxVelocity) {
    // Accelerate up
    std::get<2>(m_ref) = m_acceleration;
    std::get<1>(m_ref) = std::get<2>(m_ref) * currentTime;
  } else if (currentTime < m_timeFromMaxVelocity) {
    // Maintain max velocity
    std::get<2>(m_ref) = 0.0;
    std::get<1>(m_ref) = m_profileMaxVelocity;
  } else if (currentTime < m_timeTotal) {
    // Accelerate down
    double decelTime = currentTime - m_timeFromMaxVelocity;
    std::get<2>(m_ref) = -m_acceleration;
    std::get<1>(m_ref) = m_profileMaxVelocity + std::get<2>(m_ref) * decelTime;
  } else {
    std::get<2>(m_ref) = 0.0;
    std::get<1>(m_ref) = 0.0;
  }

  if (currentTime < m_timeTotal) {
    std::get<0>(m_ref) +=
        m_sign * std::get<1>(m_ref) * (currentTime - m_lastTime);
    m_lastTime = currentTime;
  }

  return m_ref;
}
