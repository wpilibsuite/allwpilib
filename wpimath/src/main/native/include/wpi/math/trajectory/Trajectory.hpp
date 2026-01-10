// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <algorithm>
#include <map>
#include <stdexcept>
#include <utility>
#include <vector>

#include "wpi/math/geometry/Pose2d.hpp"
#include "wpi/math/geometry/Transform2d.hpp"
#include "wpi/math/trajectory/TrajectorySample.hpp"
#include "wpi/units/time.hpp"
#include "wpi/util/MathExtras.hpp"
#include "wpi/util/SymbolExports.hpp"

namespace wpi::math {

// Forward declarations
class DifferentialDriveKinematics;
class MecanumDriveKinematics;
template <size_t NumModules>
class SwerveDriveKinematics;

/**
 * Represents a trajectory consisting of a list of samples,
 * kinematically interpolating between them.
 *
 * This non-CRTP base stores samples and provides sampling utilities.
 * Derived classes implement drivetrain-specific interpolation via the
 * virtual Interpolate() hook and expose by-value APIs (e.g., TransformBy)
 * by constructing themselves using the protected helper methods that
 * return transformed sample vectors.
 *
 * @tparam SampleType The type of sample (e.g., SplineSample,
 * DifferentialSample)
 */
template <typename SampleType>
class WPILIB_DLLEXPORT Trajectory {
 public:
  /**
   * Constructs a Trajectory from a vector of samples.
   *
   * @param samples The samples of the trajectory. Order does not matter as
   *                they will be sorted internally.
   * @throws std::invalid_argument if the vector of samples is empty.
   */
  explicit Trajectory(std::vector<SampleType> samples) {
    if (samples.empty()) {
      throw std::invalid_argument(
          "Trajectory manually initialized with no samples.");
    }

    // Sort samples by timestamp
    std::sort(samples.begin(), samples.end(), [](const auto& a, const auto& b) {
      return a.timestamp < b.timestamp;
    });

    m_samples = std::move(samples);

    // Build interpolating map
    for (const auto& sample : m_samples) {
      m_sampleMap[sample.timestamp] = sample;
    }

    m_duration = m_samples.back().timestamp;
  }

  /**
   * Returns the overall duration of the trajectory.
   *
   * @return The duration of the trajectory.
   */
  wpi::units::second_t TotalTime() const { return m_duration; }

  /**
   * Returns the samples of the trajectory.
   *
   * @return The samples of the trajectory.
   */
  const std::vector<SampleType>& Samples() const { return m_samples; }

  /**
   * Returns the first sample in the trajectory.
   *
   * @return The first sample.
   */
  const SampleType& Start() const { return m_samples.front(); }

  /**
   * Returns the last sample in the trajectory.
   *
   * @return The last sample.
   */
  const SampleType& End() const { return m_samples.back(); }

  /**
   * Sample the trajectory at a point in time.
   *
   * @param t The point in time since the beginning of the trajectory to sample.
   * @return The sample at that point in time.
   * @throws std::runtime_error if the trajectory has no samples.
   */
  SampleType SampleAt(wpi::units::second_t t) const {
    if (m_samples.empty()) {
      throw std::runtime_error(
          "Trajectory cannot be sampled if it has no samples.");
    }

    if (t <= m_samples.front().timestamp) {
      return m_samples.front();
    }
    if (t >= m_duration) {
      return m_samples.back();
    }

    // Find the two samples to interpolate between
    auto upper = m_sampleMap.upper_bound(t);
    if (upper == m_sampleMap.begin()) {
      return upper->second;
    }

    auto lower = std::prev(upper);

    // Calculate interpolation parameter
    const double t_param = (t - lower->first) / (upper->first - lower->first);

    // Use derived class's interpolation (runtime polymorphism)
    return Interpolate(lower->second, upper->second, t_param);
  }

  /**
   * Sample the trajectory at a point in time.
   *
   * @param t The point in time since the beginning of the trajectory to sample
   *          (in seconds).
   * @return The sample at that point in time.
   */
  SampleType SampleAt(double t) const {
    return SampleAt(wpi::units::second_t{t});
  }

  /**
   * Interpolates between two samples. This method must be implemented by
   * subclasses to provide drivetrain-specific interpolation logic.
   *
   * @param start The starting sample.
   * @param end The ending sample.
   * @param t The interpolation parameter between 0 and 1.
   * @return The interpolated sample.
   */
  virtual SampleType Interpolate(const SampleType& start, const SampleType& end,
                                 double t) const = 0;

  /**
   * Returns the initial pose of the trajectory.
   *
   * @return The initial pose of the trajectory.
   */
  Pose2d InitialPose() const { return m_samples.front().pose; }

  /**
   * Checks equality between this Trajectory and another object.
   */
  bool operator==(const Trajectory& other) const {
    return m_samples == other.m_samples;
  }

  virtual ~Trajectory() = default;

 protected:
  // Helper: make all samples relative to a Pose2d
  std::vector<SampleType> RelativeSamples(const Pose2d& pose) const {
    std::vector<SampleType> out;
    out.reserve(m_samples.size());
    for (const auto& s : m_samples) {
      out.emplace_back(s.RelativeTo(pose));
    }
    return out;
  }

  // Helper: concatenate with another list of samples, offsetting their
  // timestamps by this trajectory's duration
  std::vector<SampleType> ConcatenateSamples(
      const std::vector<SampleType>& other) const {
    std::vector<SampleType> out;
    out.reserve(m_samples.size() + other.size());
    // copy existing
    out.insert(out.end(), m_samples.begin(), m_samples.end());
    // append other with timestamp offset
    for (const auto& s : other) {
      out.emplace_back(s.WithNewTimestamp(s.timestamp + m_duration));
    }
    return out;
  }

  std::vector<SampleType> m_samples;
  std::map<wpi::units::second_t, SampleType> m_sampleMap;
  wpi::units::second_t m_duration{0};
};

}  // namespace wpi::math
