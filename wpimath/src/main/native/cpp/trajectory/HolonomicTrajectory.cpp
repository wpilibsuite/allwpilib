#include "wpi/math/trajectory/HolonomicTrajectory.hpp"

#include "wpi/math/trajectory/TrajectorySample.hpp"

using namespace wpi::math;

TrajectorySample HolonomicTrajectory::Interpolate(
    const TrajectorySample& start, const TrajectorySample& end,
    double t) const {
  return KinematicInterpolate(start, end, t);
}

HolonomicTrajectory HolonomicTrajectory::TransformBy(
    const Transform2d& transform) const {
  const Pose2d& firstPose = Start().pose;
  Pose2d transformedFirstPose = firstPose.TransformBy(transform);

  std::vector<TrajectorySample> transformedSamples;
  transformedSamples.reserve(m_samples.size());

  // Transform first sample
  transformedSamples.push_back(TrajectorySample(
      Start().timestamp, transformedFirstPose, Start().velocity,
      Start().acceleration));

  // Transform remaining samples
  for (size_t i = 1; i < m_samples.size(); ++i) {
    const auto& sample = m_samples[i];
    transformedSamples.push_back(TrajectorySample(
        sample.timestamp, transformedFirstPose + (sample.pose - firstPose),
        sample.velocity, sample.acceleration));
  }

  return HolonomicTrajectory(std::move(transformedSamples));
}

HolonomicTrajectory HolonomicTrajectory::RelativeTo(
    const Pose2d& pose) const {
  std::vector<TrajectorySample> relativeSamples;
  relativeSamples.reserve(m_samples.size());

  for (const auto& sample : m_samples) {
    relativeSamples.push_back(sample.RelativeTo(pose));
  }

  return HolonomicTrajectory(std::move(relativeSamples));
}

HolonomicTrajectory HolonomicTrajectory::Concatenate(
    const Trajectory<TrajectorySample, HolonomicTrajectory>& other)
    const {
  if (other.Samples().empty()) {
    return *this;
  }

  std::vector<TrajectorySample> combined;
  combined.reserve(m_samples.size() + other.Samples().size());

  // Add this trajectory's samples
  combined.insert(combined.end(), m_samples.begin(), m_samples.end());

  // Add other trajectory's samples with adjusted timestamps
  for (const auto& sample : other.Samples()) {
    combined.push_back(sample.WithNewTimestamp(sample.timestamp + m_duration));
  }

  return HolonomicTrajectory(std::move(combined));
}


