/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/trajectory/TrajectoryGenerator.h"

#include <utility>

#include "frc/spline/SplineHelper.h"
#include "frc/trajectory/TrajectoryParameterizer.h"

using namespace frc;

Trajectory TrajectoryGenerator::GenerateTrajectory(
    const std::vector<Pose2d>& waypoints,
    std::vector<std::unique_ptr<TrajectoryConstraint>>&& constraints,
    units::meters_per_second_t startVelocity,
    units::meters_per_second_t endVelocity,
    units::meters_per_second_t maxVelocity,
    units::meters_per_second_squared_t maxAcceleration, bool reversed) {
  const Transform2d flip{Translation2d(), Rotation2d(180_deg)};

  // Make theta normal for trajectory generation if path is reversed.
  std::vector<Pose2d> newWaypoints;
  newWaypoints.reserve(waypoints.size());
  for (auto&& point : waypoints) {
    newWaypoints.push_back(reversed ? point + flip : point);
  }

  auto points = SplinePointsFromSplines(
      SplineHelper::QuinticSplinesFromWaypoints(newWaypoints));

  // After trajectory generation, flip theta back so it's relative to the
  // field. Also fix curvature.
  if (reversed) {
    for (auto& point : points) {
      point = {point.first + flip, -point.second};
    }
  }

  return TrajectoryParameterizer::TimeParameterizeTrajectory(
      points, std::move(constraints), startVelocity, endVelocity, maxVelocity,
      maxAcceleration, reversed);
}

Trajectory TrajectoryGenerator::GenerateTrajectory(
    const Pose2d& start, const std::vector<Translation2d>& waypoints,
    const Pose2d& end,
    std::vector<std::unique_ptr<TrajectoryConstraint>>&& constraints,
    units::meters_per_second_t startVelocity,
    units::meters_per_second_t endVelocity,
    units::meters_per_second_t maxVelocity,
    units::meters_per_second_squared_t maxAcceleration, bool reversed) {
  const Transform2d flip{Translation2d(), Rotation2d(180_deg)};
  // Make theta normal for trajectory generation if path is reversed.

  const Pose2d newStart = reversed ? start + flip : start;
  const Pose2d newEnd = reversed ? end + flip : end;

  auto points = SplinePointsFromSplines(
      SplineHelper::CubicSplinesFromWaypoints(newStart, waypoints, newEnd));

  // After trajectory generation, flip theta back so it's relative to the
  // field. Also fix curvature.
  if (reversed) {
    for (auto& point : points) {
      point = {point.first + flip, -point.second};
    }
  }

  return TrajectoryParameterizer::TimeParameterizeTrajectory(
      points, std::move(constraints), startVelocity, endVelocity, maxVelocity,
      maxAcceleration, reversed);
}
