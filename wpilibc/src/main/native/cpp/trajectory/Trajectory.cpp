/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/trajectory/Trajectory.h"

using namespace frc;

Trajectory::State Trajectory::State::Interpolate(State endValue,
                                                 double i) const {
  // Find the new [t] value.
  const auto newT = Lerp(t, endValue.t, i);

  // Find the delta time between the current state and the interpolated state.
  const auto deltaT = newT - t;

  // If delta time is negative, flip the order of interpolation.
  if (deltaT < 0_s) return endValue.Interpolate(*this, 1.0 - i);

  // Check whether the robot is reversing at this stage.
  const auto reversing =
      velocity < 0_mps ||
      (units::math::abs(velocity) < 1E-9_mps && acceleration < 0_mps_sq);

  // Calculate the new velocity.
  // v = v_0 + at
  const units::meters_per_second_t newV = velocity + (acceleration * deltaT);

  // Calculate the change in position.
  // delta_s = v_0 t + 0.5 at^2
  const units::meter_t newS =
      (velocity * deltaT + 0.5 * acceleration * deltaT * deltaT) *
      (reversing ? -1.0 : 1.0);

  // Return the new state. To find the new position for the new state, we need
  // to interpolate between the two endpoint poses. The fraction for
  // interpolation is the change in position (delta s) divided by the total
  // distance between the two endpoints.
  const double interpolationFrac =
      newS / endValue.pose.Translation().Distance(pose.Translation());

  return {newT, newV, acceleration,
          Lerp(pose, endValue.pose, interpolationFrac),
          Lerp(curvature, endValue.curvature, interpolationFrac)};
}

Trajectory::Trajectory(const std::vector<State>& states) : m_states(states) {
  m_totalTime = states.back().t;
}

Trajectory::State Trajectory::Sample(units::second_t t) const {
  if (t <= m_states.front().t) return m_states.front();
  if (t >= m_totalTime) return m_states.back();

  // To get the element that we want, we will use a binary search algorithm
  // instead of iterating over a for-loop. A binary search is O(std::log(n))
  // whereas searching using a loop is O(n).

  // This starts at 1 because we use the previous state later on for
  // interpolation.
  int low = 1;
  int high = m_states.size() - 1;

  while (low != high) {
    int mid = (low + high) / 2;
    if (m_states[mid].t < t) {
      // This index and everything under it are less than the requested
      // timestamp. Therefore, we can discard them.
      low = mid + 1;
    } else {
      // t is at least as large as the element at this index. This means that
      // anything after it cannot be what we are looking for.
      high = mid;
    }
  }

  // High and Low should be the same.

  // The sample's timestamp is now greater than or equal to the requested
  // timestamp. If it is greater, we need to interpolate between the
  // previous state and the current state to get the exact state that we
  // want.
  const auto sample = m_states[low];
  const auto prevSample = m_states[low - 1];

  // If the difference in states is negligible, then we are spot on!
  if (units::math::abs(sample.t - prevSample.t) < 1E-9_s) {
    return sample;
  }
  // Interpolate between the two states for the state that we want.
  return prevSample.Interpolate(sample,
                                (t - prevSample.t) / (sample.t - prevSample.t));
}
