// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <algorithm>
#include <cstddef>
#include <map>
#include <stdexcept>
#include <utility>
#include <vector>

#include "wpi/units/time.hpp"

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
 * @tparam SampleType The type of sample (e.g., SplineSample,
 * DifferentialSample)
 */
template <typename SampleType>
class Trajectory {
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

    // Sort samples by time
    std::sort(samples.begin(), samples.end(),
              [](const auto& a, const auto& b) { return a.time < b.time; });

    m_samples = std::move(samples);

    // Build interpolating map
    for (const auto& sample : m_samples) {
      m_sampleMap[sample.time] = sample;
    }

    m_duration = m_samples.back().time;
  }

  /**
   * Returns the overall duration of the trajectory.
   *
   * @return The duration of the trajectory.
   */
  wpi::units::second_t Duration() const { return m_duration; }

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

    if (t <= m_samples.front().time) {
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
   * Checks equality between this Trajectory and another object.
   *
   * @return True if the trajectories are equal.
   */
  bool operator==(const Trajectory& other) const {
    return m_samples == other.m_samples;
  }

  virtual ~Trajectory() = default;

 protected:
  std::vector<SampleType> m_samples;
  std::map<wpi::units::second_t, SampleType> m_sampleMap;
  wpi::units::second_t m_duration{0};
};

}  // namespace wpi::math
