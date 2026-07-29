// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/trajectory/DrivetrainSplineTrajectoryGenerator.hpp"

#include <functional>
#include <utility>
#include <vector>

#include "wpi/math/geometry/Transform2d.hpp"
#include "wpi/math/geometry/Translation2d.hpp"
#include "wpi/math/kinematics/ChassisAccelerations.hpp"
#include "wpi/math/kinematics/ChassisVelocities.hpp"
#include "wpi/math/spline/Spline.hpp"
#include "wpi/math/spline/SplineHelper.hpp"
#include "wpi/math/spline/SplineParameterizer.hpp"
#include "wpi/math/trajectory/DrivetrainSplineSample.hpp"
#include "wpi/math/trajectory/DrivetrainSplineTrajectory.hpp"
#include "wpi/math/trajectory/DrivetrainSplineTrajectoryParameterizer.hpp"
#include "wpi/math/trajectory/TrajectoryConfig.hpp"
#include "wpi/units/angle.hpp"
#include "wpi/units/curvature.hpp"
#include "wpi/units/time.hpp"
#include "wpi/util/print.hpp"

using namespace wpi::math;

const DrivetrainSplineTrajectory
    DrivetrainSplineTrajectoryGenerator::kDoNothingTrajectory(
        std::vector<DrivetrainSplineSample>{DrivetrainSplineSample{
            0_s, Pose2d{}, ChassisVelocities{}, ChassisAccelerations{},
            wpi::units::curvature_t{0.0}}});
std::function<void(const char*)>
    DrivetrainSplineTrajectoryGenerator::s_errorFunc;

void DrivetrainSplineTrajectoryGenerator::ReportError(const char* error) {
  if (s_errorFunc) {
    s_errorFunc(error);
  } else {
    wpi::util::print(stderr, "DrivetrainSplineTrajectoryGenerator error: {}\n",
                     error);
  }
}

DrivetrainSplineTrajectory DrivetrainSplineTrajectoryGenerator::Generate(
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

  std::vector<wpi::math::SplineParameterizer::PoseWithCurvature> points;
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

  return DrivetrainSplineTrajectoryParameterizer::Parameterize(
      points, config.Constraints(), config.StartVelocity(),
      config.EndVelocity(), config.MaxVelocity(), config.MaxAcceleration(),
      config.IsReversed());
}

DrivetrainSplineTrajectory DrivetrainSplineTrajectoryGenerator::Generate(
    const Pose2d& start, const std::vector<Translation2d>& interiorWaypoints,
    const Pose2d& end, const TrajectoryConfig& config) {
  auto [startCV, endCV] = SplineHelper::CubicControlVectorsFromWaypoints(
      start, interiorWaypoints, end);
  return Generate(startCV, interiorWaypoints, endCV, config);
}

DrivetrainSplineTrajectory DrivetrainSplineTrajectoryGenerator::Generate(
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

  std::vector<wpi::math::SplineParameterizer::PoseWithCurvature> points;
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

  return DrivetrainSplineTrajectoryParameterizer::Parameterize(
      points, config.Constraints(), config.StartVelocity(),
      config.EndVelocity(), config.MaxVelocity(), config.MaxAcceleration(),
      config.IsReversed());
}

DrivetrainSplineTrajectory DrivetrainSplineTrajectoryGenerator::Generate(
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

  return DrivetrainSplineTrajectoryParameterizer::Parameterize(
      points, config.Constraints(), config.StartVelocity(),
      config.EndVelocity(), config.MaxVelocity(), config.MaxAcceleration(),
      config.IsReversed());
}

void DrivetrainSplineTrajectoryGenerator::SetErrorHandler(
    std::function<void(const char*)> func) {
  s_errorFunc = std::move(func);
}
