// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/trajectory/HolonomicTrajectory.hpp"

#include <utility>
#include <vector>

#include "wpi/math/trajectory/TrajectorySample.hpp"

using namespace wpi::math;

TrajectorySample HolonomicTrajectory::Interpolate(const TrajectorySample& start,
                                                  const TrajectorySample& end,
                                                  double t) const {
  return KinematicInterpolate(start, end, t);
}

HolonomicTrajectory HolonomicTrajectory::TransformBy(
    const Transform2d& transform) const {
  const Pose2d& firstPose = Start().pose;
  Pose2d transformedFirstPose = firstPose.TransformBy(transform);

  std::vector<TrajectorySample> transformedSamples;
  transformedSamples.reserve(Samples().size());

  // Transform first sample
  transformedSamples.push_back(
      TrajectorySample(Start().timestamp, transformedFirstPose,
                       Start().velocity, Start().acceleration));

  // Transform remaining samples
  for (size_t i = 1; i < Samples().size(); ++i) {
    const auto& sample = Samples()[i];
    transformedSamples.push_back(TrajectorySample(
        sample.timestamp, transformedFirstPose + (sample.pose - firstPose),
        sample.velocity, sample.acceleration));
  }

  return HolonomicTrajectory(std::move(transformedSamples));
}

HolonomicTrajectory HolonomicTrajectory::RelativeTo(const Pose2d& pose) const {
  return HolonomicTrajectory{this->RelativeSamples(pose)};
}

HolonomicTrajectory HolonomicTrajectory::Concatenate(
    const HolonomicTrajectory& other) const {
  return HolonomicTrajectory{this->ConcatenateSamples(other.Samples())};
}
