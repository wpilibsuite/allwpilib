/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <memory>
#include <utility>
#include <vector>

#include "frc/trajectory/constraint/TrajectoryConstraint.h"
#include "frc/trajectory/Trajectory.h"

namespace frc {
/**
 * Helper class used to generate trajectories with various constraints.
 */
class TrajectoryGenerator {
 public:
  using PoseWithCurvature = std::pair<Pose2d, curvature_t>;

  /**
   * Generates a trajectory with the given waypoints and constraints.
   *
   * @param waypoints A vector of points that the trajectory must go through.
   * @param constraints A vector of various velocity and acceleration
   * constraints.
   * @param startVelocity The start velocity for the trajectory.
   * @param endVelocity The end velocity for the trajectory.
   * @param maxVelocity The max velocity for the trajectory.
   * @param maxAcceleration The max acceleration for the trajectory.
   * @param reversed Whether the robot should move backwards. Note that the
   * robot will still move from a -> b -> ... -> z as defined in the waypoints.
   *
   * @return The trajectory.
   */
  static Trajectory GenerateTrajectory(
      const std::vector<Pose2d>& waypoints,
      std::vector<std::unique_ptr<TrajectoryConstraint>>&& constraints,
      units::meters_per_second_t startVelocity,
      units::meters_per_second_t endVelocity,
      units::meters_per_second_t maxVelocity,
      units::meters_per_second_squared_t maxAcceleration, bool reversed);

  /**
   * Generates a trajectory with the given waypoints and constraints.
   *
   * @param start The starting pose for the trajectory.
   * @param waypoints The interior waypoints for the trajectory. The headings
   * will be determined automatically to ensure continuous curvature.
   * @param end The ending pose for the trajectory.
   * @param constraints A vector of various velocity and acceleration
   * constraints.
   * @param startVelocity The start velocity for the trajectory.
   * @param endVelocity The end velocity for the trajectory.
   * @param maxVelocity The max velocity for the trajectory.
   * @param maxAcceleration The max acceleration for the trajectory.
   * @param reversed Whether the robot should move backwards. Note that the
   * robot will still move from a -> b -> ... -> z as defined in the waypoints.
   *
   * @return The trajectory.
   */
  static Trajectory GenerateTrajectory(
      const Pose2d& start, const std::vector<Translation2d>& waypoints,
      const Pose2d& end,
      std::vector<std::unique_ptr<TrajectoryConstraint>>&& constraints,
      units::meters_per_second_t startVelocity,
      units::meters_per_second_t endVelocity,
      units::meters_per_second_t maxVelocity,
      units::meters_per_second_squared_t maxAcceleration, bool reversed);

 private:
  constexpr static double kEpsilon = 1E-6;

  /**
   * Represents a constrained state that is used when time parameterizing a
   * trajectory. Each state has the pose, curvature, distance from the start of
   * the trajectory, max velocity, min acceleration and max acceleration.
   */
  struct ConstrainedState {
    PoseWithCurvature pose = {Pose2d(), curvature_t(0.0)};
    units::meter_t distance = 0_m;
    units::meters_per_second_t maxVelocity = 0_mps;
    units::meters_per_second_squared_t minAcceleration = 0_mps_sq;
    units::meters_per_second_squared_t maxAcceleration = 0_mps_sq;
  };

  /**
   * Generate spline points from a vector of splines by parameterizing the
   * splines.
   *
   * @param splines The splines to parameterize.
   *
   * @return The spline points.
   */
  template <typename Spline>
  static std::vector<PoseWithCurvature> SplinePointsFromSplines(
      const std::vector<Spline>& splines) {
    // Create the vector of spline points.
    std::vector<PoseWithCurvature> splinePoints;

    // Add the first point to the vector.
    splinePoints.push_back(splines.front().GetPoint(0.0));

    // Iterate through the vector and parameterize each spline, adding the
    // parameterized points to the final vector.
    for (auto&& spline : splines) {
      auto points = spline.Parameterize();
      // Append the array of poses to the vector. We are removing the first
      // point because it's a duplicate of the last point from the previous
      // spline.
      splinePoints.insert(std::end(splinePoints), std::begin(points) + 1,
                          std::end(points));
    }
    return splinePoints;
  }

  /**
   * Parameterize the trajectory by time. This is where the velocity profile is
   * generated.
   *
   * The derivation of the algorithm used can be found here:
   * <http://www2.informatik.uni-freiburg.de/~lau/students/Sprunk2008.pdf>
   *
   * @param points Reference to the spline points.
   * @param constraints A vector of various velocity and acceleration
   * constraints.
   * @param startVelocity The start velocity for the trajectory.
   * @param endVelocity The end velocity for the trajectory.
   * @param maxVelocity The max velocity for the trajectory.
   * @param maxAcceleration The max acceleration for the trajectory.
   * @param reversed Whether the robot should move backwards. Note that the
   * robot will still move from a -> b -> ... -> z as defined in the waypoints.
   *
   * @return The trajectory.
   */
  static Trajectory TimeParameterizeTrajectory(
      const std::vector<PoseWithCurvature>& points,
      std::vector<std::unique_ptr<TrajectoryConstraint>>&& constraints,
      units::meters_per_second_t startVelocity,
      units::meters_per_second_t endVelocity,
      units::meters_per_second_t maxVelocity,
      units::meters_per_second_squared_t maxAcceleration, bool reversed);

  /**
   * Enforces acceleration limits as defined by the constraints. This function
   * is used when time parameterizing a trajectory.
   *
   * @param reverse Whether the robot is traveling backwards.
   * @param constraints A vector of the user-defined velocity and acceleration
   * constraints.
   * @param state Pointer to the constrained state that we are operating on.
   * This is mutated in place.
   */
  static void EnforceAccelerationLimits(
      bool reverse,
      const std::vector<std::unique_ptr<TrajectoryConstraint>>& constraints,
      ConstrainedState* state);
};
}  // namespace frc
