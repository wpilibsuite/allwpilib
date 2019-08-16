/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/trajectory/TrajectoryGenerator.h"

#include <cmath>
#include <iostream>
#include <utility>

#include "frc/spline/SplineHelper.h"

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

  return TimeParameterizeTrajectory(points, std::move(constraints),
                                    startVelocity, endVelocity, maxVelocity,
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

  return TimeParameterizeTrajectory(points, std::move(constraints),
                                    startVelocity, endVelocity, maxVelocity,
                                    maxAcceleration, reversed);
}

Trajectory TrajectoryGenerator::TimeParameterizeTrajectory(
    const std::vector<PoseWithCurvature>& points,
    std::vector<std::unique_ptr<TrajectoryConstraint>>&& constraints,
    units::meters_per_second_t startVelocity,
    units::meters_per_second_t endVelocity,
    units::meters_per_second_t maxVelocity,
    units::meters_per_second_squared_t maxAcceleration, bool reversed) {
  std::vector<ConstrainedState> constrainedStates(points.size());

  ConstrainedState predecessor{points.front(), 0_m, startVelocity,
                               -maxAcceleration, maxAcceleration};

  constrainedStates[0] = predecessor;

  // Forward pass
  for (int i = 0; i < points.size(); i++) {
    auto& constrainedState = constrainedStates[i];
    constrainedState.pose = points[i];

    // Begin constraining based on predecessor
    units::meter_t ds = constrainedState.pose.first.Translation().Distance(
        predecessor.pose.first.Translation());
    constrainedState.distance = ds + predecessor.distance;

    // We may need to iterate to find the maximum end velocity and common
    // acceleration, since acceleration limits may be a function of velocity.
    while (true) {
      // Enforce global max velocity and max reachable velocity by global
      // acceleration limit. vf = std::sqrt(vi^2 + 2*a*d).

      constrainedState.maxVelocity = units::math::min(
          maxVelocity,
          units::math::sqrt(predecessor.maxVelocity * predecessor.maxVelocity +
                            predecessor.maxAcceleration * ds * 2.0));

      constrainedState.minAcceleration = -maxAcceleration;
      constrainedState.maxAcceleration = maxAcceleration;

      // At this point, the constrained state is fully constructed apart from
      // all the custom-defined user constraints.
      for (const auto& constraint : constraints) {
        constrainedState.maxVelocity = units::math::min(
            constrainedState.maxVelocity,
            constraint->MaxVelocity(constrainedState.pose.first,
                                    constrainedState.pose.second));
      }

      // Now enforce all acceleration limits.
      EnforceAccelerationLimits(reversed, constraints, &constrainedState);

      if (ds.to<double>() < kEpsilon) break;

      // If the actual acceleration for this state is higher than the max
      // acceleration that we applied, then we need to reduce the max
      // acceleration of the predecessor and try again.
      units::meters_per_second_squared_t actualAcceleration =
          (constrainedState.maxVelocity * constrainedState.maxVelocity -
           predecessor.maxVelocity * predecessor.maxVelocity) /
          (ds * 2.0);

      // If we violate the max acceleration constraint, let's modify the
      // predecessor.
      if (constrainedState.maxAcceleration < actualAcceleration - 1E-6_mps_sq) {
        predecessor.maxAcceleration = constrainedState.maxAcceleration;
      } else {
        // Constrain the predecessor's max acceleration to the current
        // acceleration.
        if (actualAcceleration > predecessor.minAcceleration + 1E-6_mps_sq) {
          predecessor.maxAcceleration = actualAcceleration;
        }
        // If the actual acceleration is less than the predecessor's min
        // acceleration, it will be repaired in the backward pass.
        break;
      }
    }
    predecessor = constrainedState;
  }

  ConstrainedState successor{points.back(), constrainedStates.back().distance,
                             endVelocity, -maxAcceleration, maxAcceleration};

  // Backward pass
  for (int i = points.size() - 1; i >= 0; i--) {
    auto& constrainedState = constrainedStates[i];
    units::meter_t ds =
        constrainedState.distance - successor.distance;  // negative

    while (true) {
      // Enforce max velocity limit (reverse)
      // vf = std::sqrt(vi^2 + 2*a*d), where vi = successor.
      units::meters_per_second_t newMaxVelocity =
          units::math::sqrt(successor.maxVelocity * successor.maxVelocity +
                            successor.minAcceleration * ds * 2.0);

      // No more limits to impose! This state can be finalized.
      if (newMaxVelocity >= constrainedState.maxVelocity) break;

      constrainedState.maxVelocity = newMaxVelocity;

      // Check all acceleration constraints with the new max velocity.
      EnforceAccelerationLimits(reversed, constraints, &constrainedState);

      if (ds.to<double>() > -kEpsilon) break;

      // If the actual acceleration for this state is lower than the min
      // acceleration, then we need to lower the min acceleration of the
      // successor and try again.
      units::meters_per_second_squared_t actualAcceleration =
          (constrainedState.maxVelocity * constrainedState.maxVelocity -
           successor.maxVelocity * successor.maxVelocity) /
          (ds * 2.0);
      if (constrainedState.minAcceleration > actualAcceleration + 1E-6_mps_sq) {
        successor.minAcceleration = constrainedState.minAcceleration;
      } else {
        successor.minAcceleration = actualAcceleration;
        break;
      }
    }
    successor = constrainedState;
  }

  // Now we can integrate the constrained states forward in time to obtain our
  // trajectory states.

  std::vector<Trajectory::State> states(points.size());
  units::second_t t = 0_s;
  units::meter_t s = 0_m;
  units::meters_per_second_t v = 0_mps;

  for (int i = 0; i < constrainedStates.size(); i++) {
    auto state = constrainedStates[i];

    // Calculate the change in position between the current state and the
    // previous state.
    units::meter_t ds = state.distance - s;

    // Calculate the acceleration between the current state and the previous
    // state.
    units::meters_per_second_squared_t accel =
        (state.maxVelocity * state.maxVelocity - v * v) / (ds * 2);

    // Calculate dt.
    units::second_t dt = 0_s;
    if (i > 0) {
      states.at(i - 1).acceleration = reversed ? -accel : accel;
      if (units::math::abs(accel) > 1E-6_mps_sq) {
        // v_f = v_0 + a * t
        dt = (state.maxVelocity - v) / accel;
      } else if (units::math::abs(v) > 1E-6_mps) {
        // delta_x = v * t
        dt = ds / v;
      } else {
        throw std::exception(
            "Something went wrong during trajectory generation.");
      }
    }

    v = state.maxVelocity;
    s = state.distance;

    t += dt;

    states[i] = {t, reversed ? -v : v, reversed ? -accel : accel,
                 state.pose.first, state.pose.second};
  }

  return Trajectory(states);
}

void TrajectoryGenerator::EnforceAccelerationLimits(
    bool reverse,
    const std::vector<std::unique_ptr<TrajectoryConstraint>>& constraints,
    ConstrainedState* state) {
  for (auto&& constraint : constraints) {
    double factor = reverse ? -1.0 : 1.0;

    auto minMaxAccel = constraint->MinMaxAcceleration(
        state->pose.first, state->pose.second, state->maxVelocity * factor);

    state->minAcceleration = units::math::max(
        state->minAcceleration,
        reverse ? -minMaxAccel.maxAcceleration : minMaxAccel.minAcceleration);

    state->maxAcceleration = units::math::min(
        state->maxAcceleration,
        reverse ? -minMaxAccel.minAcceleration : minMaxAccel.maxAcceleration);
  }
}
