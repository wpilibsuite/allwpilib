// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

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

#include "frc/trajectory/TrajectoryParameterizer.h"

#include <vector>

#include <fmt/format.h>

#include "units/math.h"

using namespace frc;

Trajectory TrajectoryParameterizer::TimeParameterizeTrajectory(
    const std::vector<PoseWithCurvature>& points,
    const std::vector<std::unique_ptr<TrajectoryConstraint>>& constraints,
    units::meters_per_second_t startVelocity,
    units::meters_per_second_t endVelocity,
    units::meters_per_second_t maxVelocity,
    units::meters_per_second_squared_t maxAcceleration, bool reversed) {
  std::vector<ConstrainedState> constrainedStates(points.size());

  ConstrainedState predecessor{points.front(), 0_m, startVelocity,
                               -maxAcceleration, maxAcceleration};

  constrainedStates[0] = predecessor;

  // Forward pass
  for (unsigned int i = 0; i < points.size(); i++) {
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
      // acceleration limit. v_f = √(v_i² + 2ad).

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
                                    constrainedState.pose.second,
                                    constrainedState.maxVelocity));
      }

      // Now enforce all acceleration limits.
      EnforceAccelerationLimits(reversed, constraints, &constrainedState);

      if (ds.value() < kEpsilon) {
        break;
      }

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
      // v_f = √(v_i² + 2ad), where v_i = successor.
      units::meters_per_second_t newMaxVelocity =
          units::math::sqrt(successor.maxVelocity * successor.maxVelocity +
                            successor.minAcceleration * ds * 2.0);

      // No more limits to impose! This state can be finalized.
      if (newMaxVelocity >= constrainedState.maxVelocity) {
        break;
      }

      constrainedState.maxVelocity = newMaxVelocity;

      // Check all acceleration constraints with the new max velocity.
      EnforceAccelerationLimits(reversed, constraints, &constrainedState);

      if (ds.value() > -kEpsilon) {
        break;
      }

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

  for (unsigned int i = 0; i < constrainedStates.size(); i++) {
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
        // v_f = v_0 + at
        dt = (state.maxVelocity - v) / accel;
      } else if (units::math::abs(v) > 1E-6_mps) {
        // delta_x = vt
        dt = ds / v;
      } else {
        throw std::runtime_error(fmt::format(
            "Something went wrong at iteration {} of time parameterization.",
            i));
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

void TrajectoryParameterizer::EnforceAccelerationLimits(
    bool reverse,
    const std::vector<std::unique_ptr<TrajectoryConstraint>>& constraints,
    ConstrainedState* state) {
  for (auto&& constraint : constraints) {
    double factor = reverse ? -1.0 : 1.0;

    auto minMaxAccel = constraint->MinMaxAcceleration(
        state->pose.first, state->pose.second, state->maxVelocity * factor);

    if (minMaxAccel.minAcceleration > minMaxAccel.maxAcceleration) {
      throw std::runtime_error(
          "There was an infeasible trajectory constraint. To determine which "
          "one, remove all constraints from the TrajectoryConfig and add them "
          "back one-by-one.");
    }

    state->minAcceleration = units::math::max(
        state->minAcceleration,
        reverse ? -minMaxAccel.maxAcceleration : minMaxAccel.minAcceleration);

    state->maxAcceleration = units::math::min(
        state->maxAcceleration,
        reverse ? -minMaxAccel.minAcceleration : minMaxAccel.maxAcceleration);
  }
}
