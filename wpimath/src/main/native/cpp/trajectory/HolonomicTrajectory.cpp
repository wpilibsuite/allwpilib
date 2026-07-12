// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/trajectory/HolonomicTrajectory.hpp"

#include <utility>
#include <vector>

#include "wpi/util/json.hpp"

using namespace wpi::math;

HolonomicSample HolonomicTrajectory::Interpolate(const HolonomicSample& start,
                                                 const HolonomicSample& end,
                                                 double t) const {
  return KinematicInterpolate(start, end, t);
}

HolonomicTrajectory HolonomicTrajectory::TransformBy(
    const Transform2d& transform) const {
  const Pose2d& firstPose = Start().pose;
  Pose2d transformedFirstPose = firstPose.TransformBy(transform);

  std::vector<HolonomicSample> transformedSamples;
  transformedSamples.reserve(Samples().size());

  // Transform first sample
  transformedSamples.push_back(
      HolonomicSample(Start().time, transformedFirstPose, Start().velocity,
                      Start().acceleration));

  // Transform remaining samples
  for (size_t i = 1; i < Samples().size(); ++i) {
    const auto& sample = Samples()[i];
    transformedSamples.push_back(HolonomicSample(
        sample.time, transformedFirstPose + (sample.pose - firstPose),
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

void wpi::math::to_json(wpi::util::json& json,
                        const HolonomicTrajectory& trajectory) {
  json = wpi::util::json::object("samples", trajectory.Samples());
}

void wpi::math::from_json(const wpi::util::json& json,
                          HolonomicTrajectory& trajectory) {
  std::vector<HolonomicSample> samples;
  from_json(json.at("samples"), samples);
  trajectory = HolonomicTrajectory{std::move(samples)};
}
