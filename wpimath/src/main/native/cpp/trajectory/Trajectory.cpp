/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/trajectory/Trajectory.h"

#include <algorithm>

#include <wpi/json.h>

#include "units/math.h"

using namespace frc;

bool Trajectory::State::operator==(const Trajectory::State& other) const {
  return t == other.t && velocity == other.velocity &&
         acceleration == other.acceleration && pose == other.pose &&
         curvature == other.curvature;
}

bool Trajectory::State::operator!=(const Trajectory::State& other) const {
  return !operator==(other);
}

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

  // Use binary search to get the element with a timestamp no less than the
  // requested timestamp. This starts at 1 because we use the previous state
  // later on for interpolation.
  auto sample =
      std::lower_bound(m_states.cbegin() + 1, m_states.cend(), t,
                       [](const auto& a, const auto& b) { return a.t < b; });

  auto prevSample = sample - 1;

  // The sample's timestamp is now greater than or equal to the requested
  // timestamp. If it is greater, we need to interpolate between the
  // previous state and the current state to get the exact state that we
  // want.

  // If the difference in states is negligible, then we are spot on!
  if (units::math::abs(sample->t - prevSample->t) < 1E-9_s) {
    return *sample;
  }
  // Interpolate between the two states for the state that we want.
  return prevSample->Interpolate(
      *sample, (t - prevSample->t) / (sample->t - prevSample->t));
}

Trajectory Trajectory::TransformBy(const Transform2d& transform) {
  auto& firstState = m_states[0];
  auto& firstPose = firstState.pose;

  // Calculate the transformed first pose.
  auto newFirstPose = firstPose + transform;
  auto newStates = m_states;
  newStates[0].pose = newFirstPose;

  for (unsigned int i = 1; i < newStates.size(); i++) {
    auto& state = newStates[i];
    // We are transforming relative to the coordinate frame of the new initial
    // pose.
    state.pose = newFirstPose + (state.pose - firstPose);
  }

  return Trajectory(newStates);
}

Trajectory Trajectory::RelativeTo(const Pose2d& pose) {
  auto newStates = m_states;
  for (auto& state : newStates) {
    state.pose = state.pose.RelativeTo(pose);
  }
  return Trajectory(newStates);
}

void frc::to_json(wpi::json& json, const Trajectory::State& state) {
  json = wpi::json{{"time", state.t.to<double>()},
                   {"velocity", state.velocity.to<double>()},
                   {"acceleration", state.acceleration.to<double>()},
                   {"pose", state.pose},
                   {"curvature", state.curvature.to<double>()}};
}

void frc::from_json(const wpi::json& json, Trajectory::State& state) {
  state.pose = json.at("pose").get<Pose2d>();
  state.t = units::second_t{json.at("time").get<double>()};
  state.velocity =
      units::meters_per_second_t{json.at("velocity").get<double>()};
  state.acceleration =
      units::meters_per_second_squared_t{json.at("acceleration").get<double>()};
  state.curvature = units::curvature_t{json.at("curvature").get<double>()};
}

bool Trajectory::operator==(const Trajectory& other) const {
  return m_states == other.States();
}

bool Trajectory::operator!=(const Trajectory& other) const {
  return !operator==(other);
}
