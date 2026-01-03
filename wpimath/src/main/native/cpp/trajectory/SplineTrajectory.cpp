// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/trajectory/SplineTrajectory.hpp"

#include <utility>
#include <vector>

#include "wpi/units/math.hpp"
#include "wpi/util/MathExtras.hpp"

using namespace wpi::math;

SplineSample SplineTrajectory::Interpolate(const SplineSample& start,
                                           const SplineSample& end,
                                           double t) const {
  // Find the new t value.
  const double newT =
      wpi::util::Lerp(start.timestamp.value(), end.timestamp.value(), t);

  // Find the delta time between the current state and the interpolated state.
  const double deltaT = newT - start.timestamp.value();

  // If delta time is negative, flip the order of interpolation.
  if (deltaT < 0) {
    return Interpolate(end, start, 1.0 - t);
  }

  // Check whether the robot is reversing at this stage.
  const bool reversing = start.velocity.vx < 0_mps ||
                         (wpi::units::math::abs(start.velocity.vx) < 1E-9_mps &&
                          start.acceleration.ax < 0_mps_sq);

  // Calculate the new velocity
  // v_f = v_0 + at
  const double newV =
      start.velocity.vx.value() + (start.acceleration.ax.value() * deltaT);

  // Calculate the change in position.
  // delta_s = v_0 t + 0.5at²
  const double newS = (start.velocity.vx.value() * deltaT +
                       0.5 * start.acceleration.ax.value() * deltaT * deltaT) *
                      (reversing ? -1.0 : 1.0);

  // Return the new state. To find the new position for the new state, we
  // need to interpolate between the two endpoint poses. The fraction for
  // interpolation is the change in position (delta s) divided by the total
  // distance between the two endpoints.
  const double interpolationFrac =
      newS / end.pose.Translation().Distance(start.pose.Translation()).value();

  Pose2d newPose = start.pose + (end.pose - start.pose) * interpolationFrac;
  double newAccel = wpi::util::Lerp(start.acceleration.ax.value(),
                                    end.acceleration.ax.value(), t);
  wpi::units::curvature_t newCurvature =
      wpi::util::Lerp(start.curvature, end.curvature, t);

  return SplineSample(newT, newPose, newV, newAccel, newCurvature.value());
}
