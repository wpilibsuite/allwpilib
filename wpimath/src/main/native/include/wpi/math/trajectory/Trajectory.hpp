// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <algorithm>
#include <stdexcept>
#include <vector>

#include <wpi/MathExtras.h>
#include <wpi/SymbolExports.h>
#include <wpi/json_fwd.h>

#include "frc/geometry/Pose2d.h"
#include "frc/geometry/Transform2d.h"
#include "units/acceleration.h"
#include "units/curvature.h"
#include "units/math.h"
#include "units/time.h"
#include "units/velocity.h"

namespace frc {
/**
 * Represents a time-parameterized trajectory. The trajectory contains of
 * various States that represent the pose, curvature, time elapsed, velocity,
 * and acceleration at that point.
 */
class WPILIB_DLLEXPORT Trajectory {
 public:
  /**
   * Represents one point on the trajectory.
   */
  struct WPILIB_DLLEXPORT State {
    /// The time elapsed since the beginning of the trajectory.
    units::second_t t = 0_s;

    /// The speed at that point of the trajectory.
    units::meters_per_second_t velocity = 0_mps;

    /// The acceleration at that point of the trajectory.
    units::meters_per_second_squared_t acceleration = 0_mps_sq;

    /// The pose at that point of the trajectory.
    Pose2d pose;

    /// The curvature at that point of the trajectory.
    units::curvature_t curvature{0.0};

    /**
     * Checks equality between this State and another object.
     */
    constexpr bool operator==(const State&) const = default;

    /**
     * Interpolates between two States.
     *
     * @param endValue The end value for the interpolation.
     * @param i The interpolant (fraction).
     *
     * @return The interpolated state.
     */
    constexpr State Interpolate(State endValue, double i) const {
      // Find the new [t] value.
      const auto newT = wpi::Lerp(t, endValue.t, i);

      // Find the delta time between the current state and the interpolated
      // state.
      const auto deltaT = newT - t;

      // If delta time is negative, flip the order of interpolation.
      if (deltaT < 0_s) {
        return endValue.Interpolate(*this, 1.0 - i);
      }

      // Check whether the robot is reversing at this stage.
      const auto reversing =
          velocity < 0_mps ||
          (units::math::abs(velocity) < 1E-9_mps && acceleration < 0_mps_sq);

      // Calculate the new velocity.
      // v = v_0 + at
      const units::meters_per_second_t newV =
          velocity + (acceleration * deltaT);

      // Calculate the change in position.
      // delta_s = v_0 t + 0.5at²
      const units::meter_t newS =
          (velocity * deltaT + 0.5 * acceleration * deltaT * deltaT) *
          (reversing ? -1.0 : 1.0);

      // Return the new state. To find the new position for the new state, we
      // need to interpolate between the two endpoint poses. The fraction for
      // interpolation is the change in position (delta s) divided by the total
      // distance between the two endpoints.
      const double interpolationFrac =
          newS / endValue.pose.Translation().Distance(pose.Translation());

      return {newT, newV, acceleration,
              wpi::Lerp(pose, endValue.pose, interpolationFrac),
              wpi::Lerp(curvature, endValue.curvature, interpolationFrac)};
    }
  };

  Trajectory() = default;

  /**
   * Constructs a trajectory from a vector of states.
   *
   * @throws std::invalid_argument if the vector of states is empty.
   */
  explicit Trajectory(const std::vector<State>& states) : m_states(states) {
    if (m_states.empty()) {
      throw std::invalid_argument(
          "Trajectory manually initialized with no states.");
    }

    m_totalTime = states.back().t;
  }

  /**
   * Returns the overall duration of the trajectory.
   * @return The duration of the trajectory.
   */
  units::second_t TotalTime() const { return m_totalTime; }

  /**
   * Return the states of the trajectory.
   *
   * @return The states of the trajectory.
   */
  const std::vector<State>& States() const { return m_states; }

  /**
   * Sample the trajectory at a point in time.
   *
   * @param t The point in time since the beginning of the trajectory to sample.
   * @return The state at that point in time.
   * @throws std::runtime_error if the trajectory has no states.
   */
  State Sample(units::second_t t) const {
    if (m_states.empty()) {
      throw std::runtime_error(
          "Trajectory cannot be sampled if it has no states.");
    }

    if (t <= m_states.front().t) {
      return m_states.front();
    }
    if (t >= m_totalTime) {
      return m_states.back();
    }

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

  /**
   * Transforms all poses in the trajectory by the given transform. This is
   * useful for converting a robot-relative trajectory into a field-relative
   * trajectory. This works with respect to the first pose in the trajectory.
   *
   * @param transform The transform to transform the trajectory by.
   * @return The transformed trajectory.
   */
  Trajectory TransformBy(const Transform2d& transform) {
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

  /**
   * Transforms all poses in the trajectory so that they are relative to the
   * given pose. This is useful for converting a field-relative trajectory
   * into a robot-relative trajectory.
   *
   * @param pose The pose that is the origin of the coordinate frame that
   *             the current trajectory will be transformed into.
   * @return The transformed trajectory.
   */
  Trajectory RelativeTo(const Pose2d& pose) {
    auto newStates = m_states;
    for (auto& state : newStates) {
      state.pose = state.pose.RelativeTo(pose);
    }
    return Trajectory(newStates);
  }

  /**
   * Concatenates another trajectory to the current trajectory. The user is
   * responsible for making sure that the end pose of this trajectory and the
   * start pose of the other trajectory match (if that is the desired behavior).
   *
   * @param other The trajectory to concatenate.
   * @return The concatenated trajectory.
   */
  Trajectory operator+(const Trajectory& other) const {
    // If this is a default constructed trajectory with no states, then we can
    // simply return the rhs trajectory.
    if (m_states.empty()) {
      return other;
    }

    auto states = m_states;
    auto otherStates = other.States();
    for (auto& otherState : otherStates) {
      otherState.t += m_totalTime;
    }

    // Here we omit the first state of the other trajectory because we don't
    // want two time points with different states. Sample() will automatically
    // interpolate between the end of this trajectory and the second state of
    // the other trajectory.
    states.insert(states.end(), otherStates.begin() + 1, otherStates.end());
    return Trajectory(states);
  }

  /**
   * Returns the initial pose of the trajectory.
   *
   * @return The initial pose of the trajectory.
   */
  Pose2d InitialPose() const { return Sample(0_s).pose; }

  /**
   * Checks equality between this Trajectory and another object.
   */
  bool operator==(const Trajectory&) const = default;

 private:
  std::vector<State> m_states;
  units::second_t m_totalTime = 0_s;
};

WPILIB_DLLEXPORT
void to_json(wpi::json& json, const Trajectory::State& state);

WPILIB_DLLEXPORT
void from_json(const wpi::json& json, Trajectory::State& state);

}  // namespace frc

#include "frc/trajectory/proto/TrajectoryProto.h"
#include "frc/trajectory/proto/TrajectoryStateProto.h"
