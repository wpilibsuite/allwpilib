/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "CtrlSys/SCurveProfile.h"

#include <cmath>

using namespace frc;

/**
 * Constructs SCurveProfile.
 *
 * @param maxV       maximum velocity
 * @param maxA       maximum acceleration
 * @param timeToMaxA time to maximum acceleration from no acceleration
 */
SCurveProfile::SCurveProfile(double maxV, double maxA, double timeToMaxA) {
  SetMaxVelocity(maxV);
  SetMaxAcceleration(maxA);
  SetTimeToMaxA(timeToMaxA);
}

/**
 * Sets goal state of profile.
 *
 * @param goal a distance to which to travel
 * @param currentSource the current position
 */
void SCurveProfile::SetGoal(double goal, double currentSource) {
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

  // If profile can't accelerate up to max velocity before decelerating
  bool shortProfile =
      m_maxVelocity * (m_timeToMaxA + m_maxVelocity / m_acceleration) >
      m_sign * m_goal;

  if (shortProfile) {
    m_profileMaxVelocity =
        m_acceleration * (std::sqrt(m_sign * m_goal / m_acceleration -
                                    0.75 * std::pow(m_timeToMaxA, 2)) -
                          0.5 * m_timeToMaxA);
  } else {
    m_profileMaxVelocity = m_maxVelocity;
  }

  // Find times at critical points
  m_t2 = m_profileMaxVelocity / m_acceleration;
  m_t3 = m_t2 + m_timeToMaxA;
  if (shortProfile) {
    m_t4 = m_t3;
  } else {
    m_t4 = m_sign * m_goal / m_profileMaxVelocity;
  }
  m_t5 = m_t4 + m_timeToMaxA;
  m_t6 = m_t4 + m_t2;
  m_t7 = m_t6 + m_timeToMaxA;
  m_timeTotal = m_t7;

  // Restore desired goal
  m_goal = goal;

  Reset();
}

/**
 * Sets maximum velocity of profile.
 *
 * @param velocity maximum velocity
 */
void SCurveProfile::SetMaxVelocity(double velocity) {
  std::lock_guard<std::mutex> lock(m_mutex);
  m_maxVelocity = velocity;
}

/**
 * Returns maximum velocity of profile.
 */
double SCurveProfile::GetMaxVelocity() const {
  std::lock_guard<std::mutex> lock(m_mutex);
  return m_maxVelocity;
}

/**
 * Sets maximum acceleration of profile.
 *
 * @param acceleration maximum acceleration
 */
void SCurveProfile::SetMaxAcceleration(double acceleration) {
  std::lock_guard<std::mutex> lock(m_mutex);
  m_acceleration = acceleration;
  m_jerk = m_acceleration / m_timeToMaxA;
}

/**
 * Sets time to maximum acceleration from no acceleration.
 *
 * @param timeToMaxA time to maximum acceleration
 */
void SCurveProfile::SetTimeToMaxA(double timeToMaxA) {
  std::lock_guard<std::mutex> lock(m_mutex);
  m_timeToMaxA = timeToMaxA;
  m_jerk = m_acceleration / m_timeToMaxA;
}

MotionProfile::State SCurveProfile::UpdateSetpoint(double currentTime) {
  if (currentTime < m_timeToMaxA) {
    // Ramp up acceleration
    std::get<2>(m_ref) = m_jerk * currentTime;
    std::get<1>(m_ref) = 0.5 * m_jerk * std::pow(currentTime, 2);
  } else if (currentTime < m_t2) {
    // Increase speed at max acceleration
    std::get<2>(m_ref) = m_acceleration;
    std::get<1>(m_ref) = m_acceleration * (currentTime - 0.5 * m_timeToMaxA);
  } else if (currentTime < m_t3) {
    // Ramp down acceleration
    std::get<2>(m_ref) = m_acceleration - m_jerk * (m_t2 - currentTime);
    std::get<1>(m_ref) =
        m_acceleration * m_t2 - 0.5 * m_jerk * std::pow(m_t2 - currentTime, 2);
  } else if (currentTime < m_t4) {
    // Maintain max velocity
    std::get<2>(m_ref) = 0.0;
    std::get<1>(m_ref) = m_profileMaxVelocity;
  } else if (currentTime < m_t5) {
    // Ramp down acceleration
    std::get<2>(m_ref) = -m_jerk * (currentTime - m_t4);
    std::get<1>(m_ref) =
        m_profileMaxVelocity - 0.5 * m_jerk * std::pow(currentTime - m_t4, 2);
  } else if (currentTime < m_t6) {
    // Decrease speed at max acceleration
    std::get<2>(m_ref) = m_acceleration;
    std::get<1>(m_ref) = m_acceleration * (m_t2 + m_t5 - currentTime);
  } else if (currentTime < m_t7) {
    // Ramp up acceleration
    std::get<2>(m_ref) = m_jerk * (m_t6 - currentTime);
    std::get<1>(m_ref) = 0.5 * m_jerk * std::pow(m_t6 - currentTime, 2);
  } else {
    std::get<2>(m_ref) = 0.0;
    std::get<1>(m_ref) = 0.0;
  }

  if (currentTime < m_t7) {
    std::get<0>(m_ref) +=
        m_sign * std::get<1>(m_ref) * (currentTime - m_lastTime);
    m_lastTime = currentTime;
  }

  return m_ref;
}
