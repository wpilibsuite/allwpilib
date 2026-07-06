// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/trajectory/SplineTrajectory.hpp"

#include <cstddef>
#include <utility>
#include <vector>

#include "wpi/math/geometry/Pose2d.hpp"
#include "wpi/math/geometry/Rotation2d.hpp"
#include "wpi/math/trajectory/SplineSample.hpp"
#include "wpi/units/acceleration.hpp"
#include "wpi/units/curvature.hpp"
#include "wpi/units/math.hpp"
#include "wpi/units/time.hpp"
#include "wpi/units/velocity.hpp"
#include "wpi/util/MathExtras.hpp"

using namespace wpi::math;

SplineSample SplineTrajectory::Interpolate(const SplineSample& start,
                                           const SplineSample& end,
                                           double t) const {
  // Find the new t value.
  const auto newT = wpi::util::Lerp(start.timestamp, end.timestamp, t);

  // Find the delta time between the current state and the interpolated state.
  const auto deltaT = newT - start.timestamp;

  // If delta time is negative, flip the order of interpolation.
  if (deltaT < 0_s) {
    return Interpolate(end, start, 1.0 - t);
  }

  // Velocities and accelerations are stored field-relative; project them onto
  // the sample's heading to recover the signed forward (path-relative) scalars.
  const auto startForwardVelocity = start.ForwardVelocity();
  const auto startForwardAccel = start.ForwardAcceleration();

  // Check whether the robot is reversing at this stage.
  const bool reversing = startForwardVelocity < 0_mps ||
                         (units::math::abs(startForwardVelocity) < 1e-9_mps &&
                          startForwardAccel < 0_mps_sq);

  // Calculate the new velocity
  // v_f = v_0 + at
  const auto newV = startForwardVelocity + (startForwardAccel * deltaT);

  // Calculate the change in position.
  // delta_s = v_0 t + 0.5at²
  const auto newS = (startForwardVelocity * deltaT +
                     0.5 * startForwardAccel * deltaT * deltaT) *
                    (reversing ? -1.0 : 1.0);

  // Return the new state. To find the new position for the new state, we
  // need to interpolate between the two endpoint poses. The fraction for
  // interpolation is the change in position (delta s) divided by the total
  // distance between the two endpoints.
  const auto interpolationFrac =
      newS / end.pose.Translation().Distance(start.pose.Translation());

  Pose2d newPose = start.pose + (end.pose - start.pose) * interpolationFrac;
  auto newAccel =
      wpi::util::Lerp(startForwardAccel, end.ForwardAcceleration(), t);
  auto newCurvature = wpi::util::Lerp(start.curvature, end.curvature, t);

  return SplineSample(newT, newPose, newV, newAccel, newCurvature);
}

SplineTrajectory SplineTrajectory::TransformBy(
    const Transform2d& transform) const {
  const Pose2d& firstPose = Start().pose;
  Pose2d transformedFirstPose = firstPose.TransformBy(transform);

  // The whole trajectory is rigidly rotated by the transform's rotation, so the
  // field-relative velocities and accelerations rotate by the same amount.
  const Rotation2d& rotation = transform.Rotation();

  std::vector<SplineSample> transformedSamples;
  transformedSamples.reserve(Samples().size());

  // Transform first sample
  transformedSamples.push_back(SplineSample(
      Start().timestamp, transformedFirstPose,
      Start().velocity.ToFieldRelative(rotation),
      Start().acceleration.ToFieldRelative(rotation), Start().curvature));

  // Transform remaining samples
  for (size_t i = 1; i < Samples().size(); ++i) {
    const auto& sample = Samples()[i];
    transformedSamples.push_back(SplineSample(
        sample.timestamp, transformedFirstPose + (sample.pose - firstPose),
        sample.velocity.ToFieldRelative(rotation),
        sample.acceleration.ToFieldRelative(rotation), sample.curvature));
  }

  return SplineTrajectory(std::move(transformedSamples));
}

SplineTrajectory SplineTrajectory::RelativeTo(const Pose2d& pose) const {
  return SplineTrajectory{this->RelativeSamples(pose)};
}

SplineTrajectory SplineTrajectory::Concatenate(
    const SplineTrajectory& other) const {
  return SplineTrajectory{this->ConcatenateSamples(other.Samples())};
}
