// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/trajectory/TrajectoryGenerator.h"

#include <utility>
#include <vector>

#include <wpi/print.h>

#include "frc/spline/SplineHelper.h"
#include "frc/spline/SplineParameterizer.h"
#include "frc/trajectory/TrajectoryParameterizer.h"

using namespace frc;

const Trajectory TrajectoryGenerator::kDoNothingTrajectory(
    std::vector<Trajectory::State>{Trajectory::State()});
std::function<void(const char*)> TrajectoryGenerator::s_errorFunc;

void TrajectoryGenerator::ReportError(const char* error) {
  if (s_errorFunc) {
    s_errorFunc(error);
  } else {
    wpi::print(stderr, "TrajectoryGenerator error: {}\n", error);
  }
}

Trajectory TrajectoryGenerator::GenerateTrajectory(
    Spline<3>::ControlVector initial,
    const std::vector<Translation2d>& interiorWaypoints,
    Spline<3>::ControlVector end, const TrajectoryConfig& config) {
  const Transform2d flip{Translation2d{}, 180_deg};

  // Make theta normal for trajectory generation if path is reversed.
  // Flip the headings.
  if (config.IsReversed()) {
    initial.x[1] *= -1;
    initial.y[1] *= -1;
    end.x[1] *= -1;
    end.y[1] *= -1;
  }

  std::vector<frc::SplineParameterizer::PoseWithCurvature> points;
  try {
    points =
        SplinePointsFromSplines(SplineHelper::CubicSplinesFromControlVectors(
            initial, interiorWaypoints, end));
  } catch (SplineParameterizer::MalformedSplineException& e) {
    ReportError(e.what());
    return kDoNothingTrajectory;
  }

  // After trajectory generation, flip theta back so it's relative to the
  // field. Also fix curvature.
  if (config.IsReversed()) {
    for (auto& point : points) {
      point = {point.first + flip, -point.second};
    }
  }

  return TrajectoryParameterizer::TimeParameterizeTrajectory(
      points, config.Constraints(), config.StartVelocity(),
      config.EndVelocity(), config.MaxVelocity(), config.MaxAcceleration(),
      config.IsReversed());
}

Trajectory TrajectoryGenerator::GenerateTrajectory(
    const Pose2d& start, const std::vector<Translation2d>& interiorWaypoints,
    const Pose2d& end, const TrajectoryConfig& config) {
  auto [startCV, endCV] = SplineHelper::CubicControlVectorsFromWaypoints(
      start, interiorWaypoints, end);
  return GenerateTrajectory(startCV, interiorWaypoints, endCV, config);
}

Trajectory TrajectoryGenerator::GenerateTrajectory(
    std::vector<Spline<5>::ControlVector> controlVectors,
    const TrajectoryConfig& config) {
  const Transform2d flip{Translation2d{}, 180_deg};
  // Make theta normal for trajectory generation if path is reversed.
  if (config.IsReversed()) {
    for (auto& vector : controlVectors) {
      // Flip the headings.
      vector.x[1] *= -1;
      vector.y[1] *= -1;
    }
  }

  std::vector<frc::SplineParameterizer::PoseWithCurvature> points;
  try {
    points = SplinePointsFromSplines(
        SplineHelper::QuinticSplinesFromControlVectors(controlVectors));
  } catch (SplineParameterizer::MalformedSplineException& e) {
    ReportError(e.what());
    return kDoNothingTrajectory;
  }

  // After trajectory generation, flip theta back so it's relative to the
  // field. Also fix curvature.
  if (config.IsReversed()) {
    for (auto& point : points) {
      point = {point.first + flip, -point.second};
    }
  }

  return TrajectoryParameterizer::TimeParameterizeTrajectory(
      points, config.Constraints(), config.StartVelocity(),
      config.EndVelocity(), config.MaxVelocity(), config.MaxAcceleration(),
      config.IsReversed());
}

Trajectory TrajectoryGenerator::GenerateTrajectory(
    const std::vector<Pose2d>& waypoints, const TrajectoryConfig& config) {
  auto newWaypoints = waypoints;
  const Transform2d flip{Translation2d{}, 180_deg};
  if (config.IsReversed()) {
    for (auto& waypoint : newWaypoints) {
      waypoint = waypoint + flip;
    }
  }

  std::vector<SplineParameterizer::PoseWithCurvature> points;
  try {
    points = SplinePointsFromSplines(SplineHelper::OptimizeCurvature(
        SplineHelper::QuinticSplinesFromWaypoints(newWaypoints)));
  } catch (SplineParameterizer::MalformedSplineException& e) {
    ReportError(e.what());
    return kDoNothingTrajectory;
  }

  // After trajectory generation, flip theta back so it's relative to the
  // field. Also fix curvature.
  if (config.IsReversed()) {
    for (auto& point : points) {
      point = {point.first + flip, -point.second};
    }
  }

  return TrajectoryParameterizer::TimeParameterizeTrajectory(
      points, config.Constraints(), config.StartVelocity(),
      config.EndVelocity(), config.MaxVelocity(), config.MaxAcceleration(),
      config.IsReversed());
}

void TrajectoryGenerator::SetErrorHandler(
    std::function<void(const char*)> func) {
  s_errorFunc = std::move(func);
}
