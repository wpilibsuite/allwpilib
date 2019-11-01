/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

/*
 * MIT License
 *
 * Copyright (c) 2018 Team 254
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#pragma once

#include <memory>
#include <utility>
#include <vector>

#include "frc/trajectory/Trajectory.h"
#include "frc/trajectory/constraint/TrajectoryConstraint.h"

namespace frc {
/**
 * Class used to parameterize a trajectory by time.
 */
class TrajectoryParameterizer {
 public:
  using PoseWithCurvature = std::pair<Pose2d, curvature_t>;

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
      const std::vector<std::unique_ptr<TrajectoryConstraint>>& constraints,
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
