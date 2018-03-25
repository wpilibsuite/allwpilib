/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "MotionProfile.h"

#include <cmath>

using namespace frc;

/**
 * Constructs a motion profile that updates an output at the given period.
 *
 * @param output The output to update.
 * @param period The period after which to update the output. The default is
 *               50ms.
 */
MotionProfile::MotionProfile(PIDOutput& output, double period)
    : m_output(output) {
  m_period = period;

  m_timer.Start();
  m_notifier.StartPeriodic(period);
}

/**
 * Starts periodic output.
 */
void MotionProfile::Enable() { m_notifier.StartPeriodic(m_period); }

/**
 * Stops periodic output.
 */
void MotionProfile::Disable() { m_notifier.Stop(); }

/**
 * Returns profile's goal state.
 */
double MotionProfile::GetGoal() const {
  std::lock_guard<std::mutex> lock(m_mutex);
  return m_goal;
}

/**
 * Returns profile's current state.
 */
double MotionProfile::GetReference() const {
  std::lock_guard<std::mutex> lock(m_mutex);
  return std::get<0>(m_ref);
}

/**
 * Returns true if motion profile has reached goal state.
 */
bool MotionProfile::AtGoal() const {
  std::lock_guard<std::mutex> lock(m_mutex);
  return m_timer.Get() >= m_totalTime ||
         std::abs(m_goal - std::get<0>(m_ref)) < 0.001;
}

/**
 * Resets profile time.
 */
void MotionProfile::Reset() {
  m_lastTime = 0.0;
  m_timer.Reset();
}

/**
 * Returns the total profile time.
 */
double MotionProfile::GetTotalTime() const { return m_totalTime; }

/**
 * Used by the Notifier to update the output with the latest reference.
 */
void MotionProfile::Update() {
  std::lock_guard<std::mutex> lock(m_mutex);
  m_ref = UpdateReference(m_timer.Get());
  m_output.PIDWrite(std::get<0>(m_ref));
}
