/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <vector>

#include <units/units.h>

#include "frc/geometry/Pose2d.h"
#include "frc/geometry/Transform2d.h"

namespace wpi {
class json;
}  // namespace wpi

namespace frc {

/**
 * Define a unit for curvature.
 */
using curvature_t = units::unit_t<
    units::compound_unit<units::radian, units::inverse<units::meter>>>;

/**
 * Represents a time-parameterized trajectory. The trajectory contains of
 * various States that represent the pose, curvature, time elapsed, velocity,
 * and acceleration at that point.
 */
class Trajectory {
 public:
  /**
   * Represents one point on the trajectory.
   */
  struct State {
    // The time elapsed since the beginning of the trajectory.
    units::second_t t = 0_s;

    // The speed at that point of the trajectory.
    units::meters_per_second_t velocity = 0_mps;

    // The acceleration at that point of the trajectory.
    units::meters_per_second_squared_t acceleration = 0_mps_sq;

    // The pose at that point of the trajectory.
    Pose2d pose;

    // The curvature at that point of the trajectory.
    curvature_t curvature{0.0};

    /**
     * Checks equality between this State and another object.
     *
     * @param other The other object.
     * @return Whether the two objects are equal.
     */
    bool operator==(const State& other) const;

    /**
     * Checks inequality between this State and another object.
     *
     * @param other The other object.
     * @return Whether the two objects are not equal.
     */
    bool operator!=(const State& other) const;

    /**
     * Interpolates between two States.
     *
     * @param endValue The end value for the interpolation.
     * @param i The interpolant (fraction).
     *
     * @return The interpolated state.
     */
    State Interpolate(State endValue, double i) const;
  };

  Trajectory() = default;

  /**
   * Constructs a trajectory from a vector of states.
   */
  explicit Trajectory(const std::vector<State>& states);

  /**
   * Returns the overall duration of the trajectory.
   * @return The duration of the trajectory.
   */
  units::second_t TotalTime() const { return m_totalTime; }

  /**
   * Return the states of the trajectory.
   * @return The states of the trajectory.
   */
  const std::vector<State>& States() const { return m_states; }

  /**
   * Sample the trajectory at a point in time.
   *
   * @param t The point in time since the beginning of the trajectory to sample.
   * @return The state at that point in time.
   */
  State Sample(units::second_t t) const;

  /**
   * Transforms all poses in the trajectory by the given transform. This is
   * useful for converting a robot-relative trajectory into a field-relative
   * trajectory. This works with respect to the first pose in the trajectory.
   *
   * @param transform The transform to transform the trajectory by.
   * @return The transformed trajectory.
   */
  Trajectory TransformBy(const Transform2d& transform);

  /**
   * Transforms all poses in the trajectory so that they are relative to the
   * given pose. This is useful for converting a field-relative trajectory
   * into a robot-relative trajectory.
   *
   * @param pose The pose that is the origin of the coordinate frame that
   *             the current trajectory will be transformed into.
   * @return The transformed trajectory.
   */
  Trajectory RelativeTo(const Pose2d& pose);

  /**
   * Returns the initial pose of the trajectory.
   *
   * @return The initial pose of the trajectory.
   */
  Pose2d InitialPose() const { return Sample(0_s).pose; }

 private:
  std::vector<State> m_states;
  units::second_t m_totalTime;

  /**
   * Linearly interpolates between two values.
   *
   * @param startValue The start value.
   * @param endValue The end value.
   * @param t The fraction for interpolation.
   *
   * @return The interpolated value.
   */
  template <typename T>
  static T Lerp(const T& startValue, const T& endValue, const double t) {
    return startValue + (endValue - startValue) * t;
  }
};

void to_json(wpi::json& json, const Trajectory::State& state);

void from_json(const wpi::json& json, Trajectory::State& state);

}  // namespace frc
