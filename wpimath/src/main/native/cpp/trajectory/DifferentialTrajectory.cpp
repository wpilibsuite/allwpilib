// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/trajectory/DifferentialTrajectory.hpp"

#include <utility>
#include <vector>

#include "wpi/math/trajectory/TrajectorySampleBase.hpp"
#include "wpi/util/MathExtras.hpp"

using namespace wpi::math;

DifferentialSample DifferentialTrajectory::Interpolate(
    const DifferentialSample& start, const DifferentialSample& end,
    double t) const {
  // Use kinematic interpolation for base TrajectorySample fields
  auto baseSample = KinematicInterpolate(start, end, t);

  // Interpolate wheel speeds
  auto leftSpeed = wpi::util::Lerp(start.leftSpeed, end.leftSpeed, t);
  auto rightSpeed = wpi::util::Lerp(start.rightSpeed, end.rightSpeed, t);

  return DifferentialSample(baseSample.timestamp, baseSample.pose,
                            baseSample.velocity, baseSample.acceleration,
                            leftSpeed, rightSpeed);
}

DifferentialTrajectory DifferentialTrajectory::TransformBy(
    const Transform2d& transform) const {
  const Pose2d& firstPose = Start().pose;
  Pose2d transformedFirstPose = firstPose.TransformBy(transform);

  std::vector<DifferentialSample> transformedSamples;
  transformedSamples.reserve(m_samples.size());

  // Transform first sample
  transformedSamples.push_back(DifferentialSample(
      Start().timestamp, transformedFirstPose, Start().velocity,
      Start().acceleration, Start().leftSpeed, Start().rightSpeed));

  // Transform remaining samples
  for (size_t i = 1; i < m_samples.size(); ++i) {
    const auto& sample = m_samples[i];
    transformedSamples.push_back(DifferentialSample(
        sample.timestamp, transformedFirstPose + (sample.pose - firstPose),
        sample.velocity, sample.acceleration, sample.leftSpeed,
        sample.rightSpeed));
  }

  return DifferentialTrajectory(std::move(transformedSamples));
}

DifferentialTrajectory DifferentialTrajectory::RelativeTo(
    const Pose2d& pose) const {
  std::vector<DifferentialSample> relativeSamples;
  relativeSamples.reserve(m_samples.size());

  for (const auto& sample : m_samples) {
    relativeSamples.push_back(sample.RelativeTo(pose));
  }

  return DifferentialTrajectory(std::move(relativeSamples));
}

DifferentialTrajectory DifferentialTrajectory::Concatenate(
    const Trajectory<DifferentialSample, DifferentialTrajectory>& other) const {
  if (other.Samples().empty()) {
    return *this;
  }

  std::vector<DifferentialSample> combined;
  combined.reserve(m_samples.size() + other.Samples().size());

  // Add this trajectory's samples
  combined.insert(combined.end(), m_samples.begin(), m_samples.end());

  // Add other trajectory's samples with adjusted timestamps
  for (const auto& sample : other.Samples()) {
    combined.push_back(sample.WithNewTimestamp(sample.timestamp + m_duration));
  }

  return DifferentialTrajectory(std::move(combined));
}
